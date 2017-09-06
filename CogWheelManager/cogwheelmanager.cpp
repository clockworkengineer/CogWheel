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
CogWheelManager::CogWheelManager(QObject *parent)
{

    Q_UNUSED(parent);

    m_controllerCommandTable.insert("STATUS", &CogWheelManager::serverStatus);
    m_controllerCommandTable.insert("CONNECTIONS", &CogWheelManager::connectionList);

}

/**
 * @brief CogWheelManager::~CogWheelManager
 *
 * Destructor.
 *
 */
CogWheelManager::~CogWheelManager()
{

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
        manager.setValue("servername", "CogWheel");
    }
    if (!manager.childKeys().contains("serverpath")) {
        manager.setValue("serverpath", "");
    }
    manager.endGroup();

    manager.beginGroup("Manager");
    m_serverName = manager.value("servername").toString();
    m_serverPath = manager.value("serverpath").toString();
    manager.endGroup();

}

/**
 * @brief CogWheelManager::startManager
 *
 * Create a local socket and try to connect to server. If can't connect
 * then listen on a socket and wait for server to connect.
 *
 * @return  == true then connection made.
 */

bool CogWheelManager::startManager()
{

    // Already active

    if (m_active) {
        return(m_active);
    }

    qDebug() << "Start manager";

    m_managerSocket = new QLocalSocket();
    if (m_managerSocket==nullptr) {
        qDebug() << "Error in creating manager socket.";
    }

    connect(m_managerSocket,&QLocalSocket::connected, this, &CogWheelManager::connected);
    connect(m_managerSocket,&QLocalSocket::disconnected, this, &CogWheelManager::disconnected);
    connect(m_managerSocket,static_cast<void(QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error), this, &CogWheelManager::error);
    connect(m_managerSocket,&QLocalSocket::readyRead, this, &CogWheelManager::readyRead);
    connect(m_managerSocket,&QLocalSocket::bytesWritten, this, &CogWheelManager::bytesWritten);

    m_managerSocket->connectToServer(m_serverName);
    m_managerSocket->waitForConnected(-1);

    if (m_managerSocket->state() == QLocalSocket::UnconnectedState) {
        qInfo() << "Manager failed to connect to server. Waiting for it.";
        resetManagerSocket();
        m_active=false;
        if (!m_listening && !listen(m_serverName+"Manager")) {
            qDebug() << "Manager unable listen on socket name: " << m_serverName;
            return(m_active);
        }
        m_listening=true;
    }

    m_active=true;

    return(m_active);

}

/**
 * @brief CogWheelManager::stopManager
 *
 * Disconnect socket and set to not active.
 *
 */
void CogWheelManager::stopManager()
{

    qInfo() << "Stopping manager";

    if (m_managerSocket) {
        m_managerSocket->disconnectFromServer();
    }
    m_active=false;

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
        qDebug() << "Error: Could not create manager socket.";
        return;
    }

    if (!m_managerSocket->setSocketDescriptor(handle)) {
        qDebug() << "Error setting up socket for manager controller.";
        resetManagerSocket();
        return;
    }

    connect(m_managerSocket,&QLocalSocket::connected, this, &CogWheelManager::connected);
    connect(m_managerSocket,&QLocalSocket::disconnected, this, &CogWheelManager::disconnected);
    connect(m_managerSocket,static_cast<void(QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error), this, &CogWheelManager::error);
    connect(m_managerSocket,&QLocalSocket::readyRead, this, &CogWheelManager::readyRead);
    connect(m_managerSocket,&QLocalSocket::bytesWritten, this, &CogWheelManager::bytesWritten);

    m_active=true;

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
    if (m_active) {
        qDebug() << "Manager socket error" << socketError;
    }
}

/**
 * @brief CogWheelManager::readyRead
 *
 * Bytes received on manager socket (controller commands).
 *
 */
void CogWheelManager::readyRead()
{

    // Datastream for commands
;
    QDataStream in(m_managerSocket);
    in.setVersion(QDataStream::Qt_4_7);

    // Read blocksize header

    if (m_commandResponseBlockSize == 0) {
        if (m_managerSocket->bytesAvailable() < (int)sizeof(quint32)) {
            return;
        }
        in >> m_commandResponseBlockSize;
    }

    // Bytes still to read

    if (m_managerSocket->bytesAvailable() < m_commandResponseBlockSize || in.atEnd()) {
        return;
    }

    // Read response and if valid process. Note data stream passed to command function to
    // enable any response parameters to be processed.

    QString command;
    in >> command;

    if (m_controllerCommandTable.contains(command)) {
        (this->*m_controllerCommandTable[command])(in);
        m_commandResponseBlockSize=0;
    } else {
        qDebug() << "Response [" << command << "] not valid.";
    }

}

/**
 * @brief CogWheelManager::bytesWritten
 *
 * Bytes written on the manager socket.
 *
 * @param bytes
 */
void CogWheelManager::bytesWritten(qint64 bytes)
{
  qDebug() << "Manager bytesWritten" << bytes;
}

/**
 * @brief CogWheelManager::
 *
 * Write command to server on manager socket. These are just simple
 * text strings with no parameters at present.
 *
 * @param command   Command.
 *
 */
void CogWheelManager::writeCommandToController(const QString &command)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_7);
    out << (quint32)0;
    out << command;
    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));
    m_managerSocket->write(block);
    m_managerSocket->flush();
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

// ============================
// CLASS PRIVATE DATA ACCESSORS
// ============================

/**
 * @brief CogWheelManager::isActive
 * @return
 */
bool CogWheelManager::isActive() const
{
    return m_active;
}

/**
 * @brief CogWheelManager::setActive
 * @param active
 */
void CogWheelManager::setActive(bool active)
{
    m_active = active;
}

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
