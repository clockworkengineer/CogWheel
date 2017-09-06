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

QHash<QString, CogWheelController::CommandFunction> CogWheelController::m_managerCommandTable;

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

    m_serverName = socketName;

    // Load command table

    m_managerCommandTable.insert("START", &CogWheelController::startServer);
    m_managerCommandTable.insert("STOP", &CogWheelController::stopServer);
    m_managerCommandTable.insert("KILL", &CogWheelController::killServer);

    // Create server instance

    m_server = new CogWheelServer(true);
    if (m_server==nullptr) {
        qDebug() << "Unable to allocate server object.";
    }

    // Save QtApplication data

    m_cogWheelApplication = cogWheelApp;

    // Controller response signal/slots

    connect(m_server->connections(), &CogWheelConnections::updateConnectionList, this, &CogWheelController::updateConnectionList);

}

/**
 * @brief CogWheelController::~CogWheelController
 *
 * Destructor.
 *
 */
CogWheelController::~CogWheelController()
{

    // Stop controller

    stopController();

    // Delete server instance

    if (m_server) {
        m_server->stopServer();
        m_server->deleteLater();
        m_server=nullptr;
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

    qDebug() << "Start Controller....";

    if (!m_listening && !listen(m_serverName)) {
        qDebug() << "Controller unable listen on socket name: " << m_serverName;
        return;
    }
    m_listening=true;

    m_controllerSocket = new QLocalSocket();
    if (m_controllerSocket==nullptr) {
        qDebug() << "Error in creating controller socket.";
    }

    connect(m_controllerSocket,&QLocalSocket::connected, this, &CogWheelController::connected);
    connect(m_controllerSocket,&QLocalSocket::disconnected, this, &CogWheelController::disconnected);
    connect(m_controllerSocket,static_cast<void(QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error), this, &CogWheelController::error);
    connect(m_controllerSocket,&QLocalSocket::readyRead, this, &CogWheelController::readyRead);
    connect(m_controllerSocket,&QLocalSocket::bytesWritten, this, &CogWheelController::bytesWritten);

    m_controllerSocket->connectToServer(m_serverName+"Manager");
    m_controllerSocket->waitForConnected(-1);

    if (m_controllerSocket->state() == QLocalSocket::ConnectedState) {
        m_active=true;
        writeCommandToManager("STATUS", (m_server) ? "RUNNING" : "STOPPED");
        return;
    }

    resetControllerSocket();  // Reset socket

    qInfo() << "CogWheel Controller Started on [" << m_serverName << "]";

}

/**
 * @brief CogWheelController::stopController
 *
 * Stop controller.
 *
 */
void CogWheelController::stopController()
{
    qInfo() << "Stopping Controller";

    if (m_controllerSocket) {
        if (m_controllerSocket->state() == QLocalSocket::ConnectingState) {
            m_controllerSocket->disconnectFromServer();
            m_controllerSocket->waitForDisconnected(-1);
        }
    }

    m_active=false;

}

/**
 * @brief CogWheelController::writeRespnseToManager
 *
 * Write command to manager.
 *
 * @param command
 * @param param1
 */
void CogWheelController::writeCommandToManager(const QString &command, const QString param1)
{

    if (m_active && m_controllerSocket) {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << (quint32)0;
        out << command;
        out << param1;
        out.device()->seek(0);
        out << (quint32)(block.size() - sizeof(quint32));
        m_controllerSocket->write(block);
        m_controllerSocket->flush();
    }

}
/**
 * @brief CogWheelController::writeRespnseToManager
 *
 * Write command to manager.
 *
 * @param command
 * @param param1
 */
void CogWheelController::writeCommandToManager(const QString &command, const QStringList param1)
{

    if (m_active && m_controllerSocket) {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_7);
        out << (quint32)0;
        out << command;
        out << param1;
        out.device()->seek(0);
        out << (quint32)(block.size() - sizeof(quint32));
        m_controllerSocket->write(block);
        m_controllerSocket->flush();
    }

}

/**
 * @brief CogWheelController::resetControllerSocket
 *
 * Reset controller socket.
 *
 */
void CogWheelController::resetControllerSocket()
{
    if (m_controllerSocket) {
        if (m_controllerSocket->isOpen()) {
            m_controllerSocket->close();
        }
        m_controllerSocket->deleteLater();
        m_controllerSocket=nullptr;
    }
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
        resetControllerSocket();
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

    writeCommandToManager("STATUS", (m_server) ? "RUNNING" : "STOPPED");

    m_lastConnectionList.clear();

}

/**
 * @brief CogWheelController::connected
 *
 * CogWheel Manager connected.
 *
 */
void CogWheelController::connected()
{

    qDebug() << "CogWheel manager connected to local controller.";

}

/**
 * @brief CogWheelController::disconnected
 *
 * CogWheel Manager disonnected.
 *
 */
void CogWheelController::disconnected()
{
    qDebug() << "CogWheel manager disconnected from local controller.";

    resetControllerSocket();

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
    if (socketError==QLocalSocket::PeerClosedError) {
//        qInfo() << "Manager disconnected listen for a new connection....";
//        if (!listen(m_serverName)) {
//            qDebug() << "Controller unable listen on socket name: " << m_serverName;
//        }
        return;
    }
    qDebug() << "Controller socket error" << socketError;
}

/**
 * @brief CogWheelController::readyRead
 *
 * Read CogWheel Manager commands sent to controller.
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
    // enable any command parameters to be processed.

    QString command;
    in >> command;

    if (m_managerCommandTable.contains(command)) {
        (this->*m_managerCommandTable[command])(in);
        m_commandBlockSize=0;
    } else {
        qDebug() << "Manager command [" << command << "] not valid.";
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

/**
 * @brief CogWheelController::updateConnectionList
 * @param connections
 */
void CogWheelController::updateConnectionList(const QStringList &connectionList)
{
    qDebug() << "New connction : [" << connectionList << "]";

    if (m_lastConnectionList != connectionList) {
        writeCommandToManager("CONNECTIONS", connectionList);
        m_lastConnectionList = connectionList;
    }

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
        connect(m_server->connections(), &CogWheelConnections::updateConnectionList, this, &CogWheelController::updateConnectionList);
    } else {
        qInfo() << "CogWheel Server already started.";
    }

    writeCommandToManager("STATUS", "RUNNING");

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

    writeCommandToManager("STATUS", "STOPPED");

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

