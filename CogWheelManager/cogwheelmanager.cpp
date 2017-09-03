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
// to control it. At present this is just stop, start and kill but will be expanded
// in future and enable replies from the server.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelmanager.h"

// Manager response table

QHash<QString, CogWheelManager::ResponseFunction> CogWheelManager::m_managerResponseTable;

/**
 * @brief CogWheelManager::CogWheelManager
 *
 * Constructor.
 *
 * @param parent
 */
CogWheelManager::CogWheelManager(QObject *parent) : QObject(parent)
{

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
 * @brief CogWheelManager::startManager
 *
 * Create a local socket and try to connect to server.
 *
 * @param serverName  Name of server local socket.
 *
 * @return  == true then connection made.
 */

bool CogWheelManager::startManager(const QString &serverName)
{

    m_managerSocket = new QLocalSocket();
    if (m_managerSocket==nullptr) {
        qDebug() << "Error in creating manager socket.";
    }

    connect(m_managerSocket,&QLocalSocket::connected, this, &CogWheelManager::connected);
    connect(m_managerSocket,&QLocalSocket::disconnected, this, &CogWheelManager::disconnected);
    connect(m_managerSocket,static_cast<void(QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error), this, &CogWheelManager::error);
    connect(m_managerSocket,&QLocalSocket::readyRead, this, &CogWheelManager::readyRead);
    connect(m_managerSocket,&QLocalSocket::bytesWritten, this, &CogWheelManager::bytesWritten);

    m_managerSocket->connectToServer(serverName);
    m_managerSocket->waitForConnected(-1);

    if (m_managerSocket->state() == QLocalSocket::UnconnectedState) {
        m_managerSocket->deleteLater();
        m_managerSocket=nullptr;
        m_active=false;
    } else {
        m_active=true;
    }

    m_managerResponseTable.insert("STATUS", &CogWheelManager::serverStatus);

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

    m_managerSocket->disconnectFromServer();
    m_active=false;

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

    if (m_managerSocket) {
        m_managerSocket->deleteLater();
        m_managerSocket=nullptr;
    }

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
 * Bytes received on manager socket.
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

    if (m_managerResponseTable.contains(command)) {
        (this->*m_managerResponseTable[command])(in);
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
 * @param input
 */
void CogWheelManager::serverStatus(QDataStream &input)
{
    QString status;

    input >> status;

    emit serverStatusUpdate(status);

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
