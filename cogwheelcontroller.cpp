/*
 * File:   cogwheelcontroller.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

//
// Class: CogWheelController
//
// Description: Class to control CogWheel server by recieving commands from
// the CogWheelManager and also sending replies back. Note: This class creates
// and destroys the server instance.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelcontroller.h"
#include "cogwheelserver.h"

// Server instance and Qt application pointer

CogWheelServer *CogWheelController::m_server=nullptr;
QCoreApplication *CogWheelController::m_cogWheelApplication=nullptr;

// Command table

QHash<QString, CogWheelController::CommandFunction> CogWheelController::m_controllerCommandTable;

/**
 * @brief CogWheelController::CogWheelController
 *
 * Create controller for server on a given named socket.
 *
 * @param socketName
 * @param parent
 */
CogWheelController::CogWheelController(QCoreApplication *cogWheelApp, const QString &socketName, QObject *parent)
{

   Q_UNUSED(parent);

    // Socket name

    m_socketName = socketName;

    // Load command table

    m_controllerCommandTable.insert("START", startServer);
    m_controllerCommandTable.insert("STOP", stopServer);
    m_controllerCommandTable.insert("KILL", killServer);

    // Create server instance

    m_server = new CogWheelServer(true);
    if (m_server==nullptr) {
        qDebug() << "Unable to allocate server object.";
    }

    m_cogWheelApplication = cogWheelApp;

}

/**
 * @brief CogWheelController::~CogWheelController
 */
CogWheelController::~CogWheelController()
{
    // Delete server instance

    if (m_server) {
        m_server->stopServer();
        m_server->deleteLater();
        m_server=nullptr;
    }

    // Delete coontroller socket

    if (m_controllerSocket) {
        if (m_controllerSocket->isOpen()) {
            m_controllerSocket->close();
        }
        m_controllerSocket->deleteLater();
        m_controllerSocket=nullptr;
    }

}

/**
 * @brief CogWheelController::
 *
 * Start controller listening for incoming commands.
 *
 */
void CogWheelController::startController()
{

    removeServer(m_socketName);

    if (!listen(m_socketName)) {
        qDebug() << "Unable listen on socket name: " << m_socketName;
        return;
    }

    qInfo() << "CogWheel Controller Started on [" << m_socketName << "]";

}

/**
 * @brief CogWheelController::stopController
 *
 * Stop controller.
 *
 */
void CogWheelController::stopController()
{

}

/**
 * @brief CogWheelController::incomingConnection
 *
 * Handle incoming manager connection.
 *
 * @param handle   Socket handle for connection.
 */
void CogWheelController::incomingConnection(quintptr handle)
{

    qDebug() << "Incoming CogWheel Manager connection";

    if (m_controllerSocket) {
        m_controllerSocket->deleteLater();
        m_controllerSocket=nullptr;
    }

    m_controllerSocket = new QLocalSocket();
    if (m_controllerSocket==nullptr) {
        qDebug() << "Error: Could not create controller socket.";
        return;
    }

    if (!m_controllerSocket->setSocketDescriptor(handle)) {
        qDebug() << "Error setting up socket for control controller.";
        m_controllerSocket->deleteLater();
        return;
    }

    connect(m_controllerSocket,&QLocalSocket::connected, this, &CogWheelController::connected);
    connect(m_controllerSocket,&QLocalSocket::disconnected, this, &CogWheelController::disconnected);
    connect(m_controllerSocket,static_cast<void(QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error), this, &CogWheelController::error);
    connect(m_controllerSocket,&QLocalSocket::readyRead, this, &CogWheelController::readyRead);
    connect(m_controllerSocket,&QLocalSocket::bytesWritten, this, &CogWheelController::bytesWritten);

}

/**
 * @brief CogWheelController::connected
 *
 * CogWheel Manager connected.
 *
 */
void CogWheelController::connected()
{

    qDebug() << "CogWheel Manager Connected";

}

/**
 * @brief CogWheelController::disconnected
 *
 * CogWheel Manager disonnected.
 *
 */
void CogWheelController::disconnected()
{
    qDebug() << "CogWheel Manager disconnected";

    if (m_controllerSocket) {
        if (m_controllerSocket->isOpen()) {
            m_controllerSocket->close();
        }
        m_controllerSocket->deleteLater();
        m_controllerSocket=nullptr;
    }

}

/**
 * @brief CogWheelController::error
 *
 * CogWheel Manager connection error.
 *
 * @param socketError
 */
void CogWheelController::error(QLocalSocket::LocalSocketError socketError)
{
    qDebug() << "Controller socket error" << socketError;
}

/**
 * @brief CogWheelController::readyRead
 *
 * Read CogWheel Manager commands.
 *
 */
void CogWheelController::readyRead()
{

    // Datastream for commands

    QDataStream in(m_controllerSocket);
    in.setVersion(QDataStream::Qt_4_7);

    // Read blocksize header

    if (m_commandBlockSize == 0) {
        if (m_controllerSocket->bytesAvailable() < (int)sizeof(quint32)) {
            return;
        }
        in >> m_commandBlockSize;
    }

    // Bytes still to read

    if (m_controllerSocket->bytesAvailable() < m_commandBlockSize || in.atEnd()) {
        return;
    }

    // Read command and if valid process. Note data stream passed to command function to
    // enableany command parameters to be processed.

    QString command;
    in >> command;

    if (m_controllerCommandTable.contains(command)) {
        m_controllerCommandTable[command](in);
        m_commandBlockSize=0;
    } else {
        qDebug() << "Command [" << command << "] not valid.";
    }

}
/**
 * @brief CogWheelController::bytesWritten
 * @param bytes
 */
void CogWheelController::bytesWritten(qint64 bytes)
{
    qDebug() << "Controller bytesWritten" << bytes;
}

// ===================
// CONTROLLER COMMANDS
// ===================

/**
 * @brief CogWheelController::startServer
 *
 * Start FTP Server.
 *
 * @param input
 */
void CogWheelController::startServer(QDataStream &input)
{
    Q_UNUSED(input);

    if (m_server==nullptr) {
        m_server = new CogWheelServer(true);
        if (m_server==nullptr) {
            qDebug() << "Unable to allocate server object";
        }
    } else {
        qInfo() << "CogWheel Server already started.";
    }

}

/**
 * @brief CogWheelController::stopServer
 *
 * Stop FTP Server.
 *
 * @param input
 */
void CogWheelController::stopServer(QDataStream &input)
{

     Q_UNUSED(input);

     if (m_server) {
         m_server->stopServer();
         m_server->deleteLater();
         m_server=nullptr;
     } else {
         qInfo() << "CogWheel Server already stopped.";
     }

}

void CogWheelController::killServer(QDataStream &input)
{
    stopServer(input);
    m_cogWheelApplication->quit();
}

// ============================
// CLASS PRIVATE DATA ACCESSORS
// ============================

/**
 * @brief CogWheelController::server
 * @return
 */
CogWheelServer *CogWheelController::server()
{
    return m_server;
}

