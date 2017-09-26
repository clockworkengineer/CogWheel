/*
 * File:   cogwheelmanager.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

//
// Class: CogWheelManager
//
// Description: Class to open up a local socket to server and send commands
// to control it. At present this is just stop, start and kill. It also supports
// commands sent from the server (controller) such as update connection list.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelmanager.h"

// Controller command table

QHash<QString, CogWheelManager::CommandFunction> CogWheelManager::m_controllerCommandTable;

/**
 * @brief CogWheelManager::CogWheelManager
 *
 * Constructor.
 *
 * @param parent
 */
CogWheelManager::CogWheelManager(QObject *parent) : QLocalServer(parent)
{

    Q_UNUSED(parent);

    m_controllerCommandTable.insert(kCWCommandSTATUS, &CogWheelManager::serverStatus);
    m_controllerCommandTable.insert(kCWCommandCONNECTIONS, &CogWheelManager::connectionList);
    m_controllerCommandTable.insert(kCWCommandLOGOUTPUT,&CogWheelManager::logOutput);

}

/**
 * @brief CogWheelManager::~CogWheelManager
 *
 * Destructor.
 *
 */
CogWheelManager::~CogWheelManager()
{
    disconnectFromServer();
}

/**
 * @brief CogWheelManager::load
 *
 * Load manager related settings from config.
 *
 */
void CogWheelManager::load()
{

    QSettings manager;

    manager.beginGroup("Manager");
    if (!manager.childKeys().contains("servername")) {
        manager.setValue("servername", kCWApplicationName);
    }
    if (!manager.childKeys().contains("serverpath")) {
        manager.setValue("serverpath", "");
    }
    if (!manager.childKeys().contains("serverautostart")) {
        manager.setValue("serverautostart", false);
    }
    manager.endGroup();

    manager.beginGroup("Manager");
    m_serverName = manager.value("servername").toString();
    m_serverPath = manager.value("serverpath").toString();
    m_serverAutoStart = manager.value("serverautostart").toBool();
    manager.endGroup();

}

/**
 * @brief CogWheelManager::connectUpManagerSocket
 *
 * Connect up signal/slots for manager socket and setup read/write data streams.
 *
 */
void CogWheelManager::connectUpManagerSocket()
{

    connect(m_managerSocket,&QLocalSocket::connected, this, &CogWheelManager::connected);
    connect(m_managerSocket,&QLocalSocket::disconnected, this, &CogWheelManager::disconnected);
    connect(m_managerSocket,static_cast<void(QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error), this, &CogWheelManager::error);
    connect(m_managerSocket,&QLocalSocket::readyRead, this, &CogWheelManager::readyRead);

    m_managerReadStream.setVersion(QDataStream::Qt_4_7);
    m_managerReadStream.setDevice(m_managerSocket);

    m_writeQBuffer.setBuffer(&m_writeRawDataBuffer);
    m_writeQBuffer.open(QIODevice::WriteOnly);
    m_managerWriteStream.setVersion(QDataStream::Qt_4_7);
    m_managerWriteStream.setDevice(&m_writeQBuffer);

}

/**
 * @brief CogWheelManager::startManager
 *
 * Create a local socket and try to connect to server. Also set up listen
 * for incoming server connections.
 *
 * @return  == true then connection made.
 */

void CogWheelManager::startUpManager()
{

    qDebug() << "Starting up manager...";

    // Setup socket listen

    if (!listen(m_serverName+kCWManagerPostfix)) {
        throw CogWheelManager::Exception("Manager unable listen on socket name: "+m_serverName);
    }

    // Try to connect to server

    connectToServer();

}

/**
 * @brief CogWheelManager::disconnectFromServer
 *
 * Disconnect manager socket if present.
 *
 */
void CogWheelManager::disconnectFromServer()
{

    if (m_managerSocket) {
        m_managerSocket->disconnectFromServer();
    }

}

/**
 * @brief CogWheelManager::connectToServer
 *
 * Connect to server/controller.
 *
 */
void CogWheelManager::connectToServer()
{

    // Create manager socket

    m_managerSocket = new QLocalSocket();
    if (m_managerSocket==nullptr) {
        throw CogWheelManager::Exception("Could not create manager local socket");
    }

    // Connect up its signals/slots

    connectUpManagerSocket();

    // Attempt server connect

    m_managerSocket->connectToServer(m_serverName);
    m_managerSocket->waitForConnected(-1);

    // Failure wait for server connect

    if (m_managerSocket->state() == QLocalSocket::UnconnectedState) {
        resetManagerSocket();
    } else {
        qInfo() << "Manager connected to controller.";
    }

}

/**
 * @brief CogWheelManager::resetManagerSocket
 *
 * Close manger socket if open and remove its object data.
 *
 */
void CogWheelManager::resetManagerSocket()
{
    if (m_managerSocket) {
        if (m_managerSocket->isOpen()) {
            m_managerSocket->close();
        }
        m_managerSocket->deleteLater();
        m_managerSocket=nullptr;
        if (m_writeQBuffer.isOpen()) {
            m_writeQBuffer.close();
        }
    }
}

/**
 * @brief CogWheelManager::incomingConnection
 *
 * Handle incoming controller connection.
 *
 * @param handle   Socket handle for connection.
 */
void CogWheelManager::incomingConnection(quintptr handle)
{

    qDebug() << "Incoming CogWheel Controller connection";

    resetManagerSocket();

    m_managerSocket = new QLocalSocket();
    if (m_managerSocket==nullptr) {
        throw CogWheelManager::Exception("Could not create manager local socket");
    }

    if (!m_managerSocket->setSocketDescriptor(handle)) {
        throw CogWheelManager::Exception("Could not assign manager local socket descriptor.");
    }

    connectUpManagerSocket();

}

/**
 * @brief CogWheelManager::connected
 *
 * Manager socket connected.
 *
 */
void CogWheelManager::connected()
{

    qDebug() << "Manager Connected";

}

/**
 * @brief CogWheelManager::disconnected
 *
 * Manager socket disconnected. Remove socket.
 */
void CogWheelManager::disconnected()
{

    qDebug() << "Manager disconnected";

    resetManagerSocket();

}

/**
 * @brief CogWheelManager::error
 *
 * Report socket manager error.
 *
 * @param socketError
 */
void CogWheelManager::error(QLocalSocket::LocalSocketError socketError)
{

    if (socketError==QLocalSocket::ConnectionRefusedError) {
        qDebug() << "Server not running waiting for it to connect";
        return;
    }
    qDebug() << static_cast<QLocalSocket*>(sender())->errorString();

}

/**
 * @brief CogWheelManager::readyRead
 *
 * Bytes received on manager socket (controller commands).Even if a command
 * has successfully been processed there still may be buffered commands
 * so loop.
 *
 */
void CogWheelManager::readyRead()
{

    while (m_managerSocket->bytesAvailable()) {

        // Read blocksize header

        if (m_commandResponseBlockSize == 0) {
            if (m_managerSocket->bytesAvailable() < (int)sizeof(quint32)) {
                return;
            }
            m_managerReadStream >> m_commandResponseBlockSize;
        }

        // Bytes still to read

        if (m_managerSocket->bytesAvailable() < m_commandResponseBlockSize || m_managerReadStream.atEnd()) {
            return;
        }

        // Read response and if valid process. Note data stream passed to command function to
        // enable any response parameters to be processed.

        QString command;
        m_managerReadStream >> command;

        if (m_controllerCommandTable.contains(command)) {
            (this->*m_controllerCommandTable[command])(m_managerReadStream);
            m_commandResponseBlockSize=0;
        } else {
            qDebug() << "Controller command [" << command << "] not valid.";
        }

        if (m_managerSocket->bytesAvailable()) {
            qDebug() << "Still Bytes To Read" << m_managerSocket->bytesAvailable();
        }

    }

}

QLocalSocket *CogWheelManager::managerSocket() const
{
    return m_managerSocket;
}

bool CogWheelManager::serverAutoStart() const
{
    return m_serverAutoStart;
}

void CogWheelManager::setServerAutoStart(bool serverAutoStart)
{
    m_serverAutoStart = serverAutoStart;
}

/**
 * @brief CogWheelManager::writeCommandToController
 *
 * Write command to server on manager socket. These are just simple
 * text strings with no parameters at present.
 *
 * @param command   Command.
 *
 */
void CogWheelManager::writeCommandToController(const QString &command)
{
    if (m_managerSocket) {
        m_writeRawDataBuffer.clear();
        m_managerWriteStream.device()->seek(0);
        m_managerWriteStream << (quint32)0;
        m_managerWriteStream << command;
        m_managerWriteStream.device()->seek(0);
        m_managerWriteStream << (quint32)(m_writeRawDataBuffer.size() - sizeof(quint32));
        m_managerSocket->write(m_writeRawDataBuffer);
        m_managerSocket->flush();
    }
}

/**
 * @brief CogWheelManager::writeCommandToController
 *
 * Write command to server on manager socket. These are just simple
 * text strings with no parameters at present.
 *
 * @param command   Command.
 * @param param1    String paramter.
 */
void CogWheelManager::writeCommandToController(const QString &command, const QString &param1)
{
    if (m_managerSocket) {
        m_writeRawDataBuffer.clear();
        m_managerWriteStream.device()->seek(0);
        m_managerWriteStream << (quint32)0;
        m_managerWriteStream << command;
        m_managerWriteStream << param1;
        m_managerWriteStream.device()->seek(0);
        m_managerWriteStream << (quint32)(m_writeRawDataBuffer.size() - sizeof(quint32));
        m_managerSocket->write(m_writeRawDataBuffer);
        m_managerSocket->flush();
    }
}

/**
 * @brief CogWheelManager::serverStatus
 *
 * Server status command recieved from controller.
 *
 * @param input
 */
void CogWheelManager::serverStatus(QDataStream &input)
{
    QString status;

    input >> status;

    emit serverStatusUpdate(status);

}

/**
 * @brief CogWheelManager::connectionList
 *
 * Server conenction list command recieved from controller.
 *
 * @param input
 */
void CogWheelManager::connectionList(QDataStream &input)
{
    QStringList connections;

    input >> connections;

    emit connectionListUpdate(connections);

}

/**
 * @brief CogWheelManager::logOutput
 *
 * Update server logging window.
 *
 * @param input
 */
void CogWheelManager::logOutput(QDataStream &input)
{

    QStringList logBuffer;

    input >> logBuffer;

    emit logWindowUpdate(logBuffer);

}

// ============================
// CLASS PRIVATE DATA ACCESSORS
// ============================

QString CogWheelManager::serverName() const
{
    return m_serverName;
}

/**
 * @brief CogWheelManager::setServerName
 * @param serverName
 */
void CogWheelManager::setServerName(const QString &serverName)
{
    m_serverName = serverName;
}

/**
 * @brief CogWheelManager::serverPath
 * @return
 */
QString CogWheelManager::serverPath() const
{
    return m_serverPath;
}

/**
 * @brief CogWheelManager::setServerPath
 * @param serverPath
 */
void CogWheelManager::setServerPath(const QString &serverPath)
{
    m_serverPath = serverPath;
}
