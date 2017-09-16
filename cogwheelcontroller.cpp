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
#include "cogwheellogger.h"

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
CogWheelController::CogWheelController(QCoreApplication *cogWheelApp, QObject *parent) : QLocalServer(parent)
{

    Q_UNUSED(parent);

    // Load manager/controller server name from config

    QSettings manager;

    manager.beginGroup("Manager");
    if (!manager.childKeys().contains("servername")) {
        manager.setValue("servername", kCWApplicationName);
    }
    manager.endGroup();

    manager.beginGroup("Manager");
    m_serverName = manager.value("servername").toString();
    manager.endGroup();

    // Remove any previous instance of cotroller server name

    QLocalServer::removeServer(m_serverName);

    // Load command table

    m_managerCommandTable.insert(kCWCommandSTART, &CogWheelController::startServer);
    m_managerCommandTable.insert(kCWCommandSTOP, &CogWheelController::stopServer);
    m_managerCommandTable.insert(kCWCommandKILL, &CogWheelController::killServer);

    // Create server instance

    m_server = new CogWheelServer(true);
    if (m_server==nullptr) {
        cogWheelError("Unable to allocate server object.");
    }

    // Save QtApplication object instance

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

    // Remove logging timer if still around

    resetLoggingFlushTimer();

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
 * @brief CogWheelController::connectUpControllerSocket
 *
 * Connect up controller siganls/slots.
 *
 */
void CogWheelController::connectUpControllerSocket()
{

    connect(m_controllerSocket,&QLocalSocket::connected, this, &CogWheelController::connected);
    connect(m_controllerSocket,&QLocalSocket::disconnected, this, &CogWheelController::disconnected);
    connect(m_controllerSocket,static_cast<void(QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error), this, &CogWheelController::error);
    connect(m_controllerSocket,&QLocalSocket::readyRead, this, &CogWheelController::readyRead);

}

/**
 * @brief CogWheelController::
 *
 * Start controller listening for incoming commands.
 *
 */
void CogWheelController::startController()
{

    cogWheelInfo("Start Controller....");

    if (!listen(m_serverName)) {
        cogWheelError("Controller unable listen on socket name: " + m_serverName);
        return;
    }

    m_controllerSocket = new QLocalSocket();
    if (m_controllerSocket==nullptr) {
        cogWheelError("Error in creating controller socket.");
    }

    connectUpControllerSocket();

    m_controllerSocket->connectToServer(m_serverName+kCWManagerPostfix);
    m_controllerSocket->waitForConnected(-1);

    if (m_controllerSocket->state() != QLocalSocket::ConnectedState) {
        resetControllerSocket();  // Error reset socket and return
        return;
    }

    writeCommandToManager("STATUS", (m_server) ? "RUNNING" : "STOPPED");

    cogWheelInfo("CogWheel Controller Started on ["+m_serverName+"]");

}

/**
 * @brief CogWheelController::stopController
 *
 * Stop controller.
 *
 */
void CogWheelController::stopController()
{
    cogWheelInfo("Stopping Controller");

    if (m_controllerSocket) {
        if (m_controllerSocket->state() == QLocalSocket::ConnectingState) {
            m_controllerSocket->disconnectFromServer();
            m_controllerSocket->waitForDisconnected(-1);
        }
    }

}

/**
 * @brief CogWheelController::writeRespnseToManager
 *
 * Write command to manager (one QString parameter).
 *
 * @param command
 * @param param1
 */
void CogWheelController::writeCommandToManager(const QString &command, const QString param1)
{

    if (m_controllerSocket) {
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
 * Write command to manager. (one QStringList parameter).
 *
 * @param command
 * @param param1
 */
void CogWheelController::writeCommandToManager(const QString &command, const QStringList param1)
{

    if (m_controllerSocket) {
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
 * @brief CogWheelController::resetLoggingFlushTimer
 *
 * Stop and remove any logging flush timer.
 *
 */
void CogWheelController::resetLoggingFlushTimer()
{

    if (m_logFlushTimer) {
        m_logFlushTimer->stop();
        m_logFlushTimer->deleteLater();
        m_logFlushTimer=nullptr;
    }
}

/**
 * @brief CogWheelController::enableLoggingToManager
 *
 * Enable/Disable serve logging to connected manager.
 *
 * @param enable    == true enable
 */
void CogWheelController::enableLoggingToManager(bool enable)
{

    if (CogWheelLogger::getInstance().getLoggingEnabled()) {
        if (enable) {
            m_logFlushTimer = new QTimer();
            connect(m_logFlushTimer, &QTimer::timeout, this, &CogWheelController::flushLogToManager);
            m_logFlushTimer->start(kCWLoggingFlushTimer);
        } else {
            resetLoggingFlushTimer();
        }
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

    cogWheelInfo( "Incoming CogWheel Manager connection");

    // Reset  though should be uncessary

    //resetControllerSocket();

    m_controllerSocket = new QLocalSocket();
    if (m_controllerSocket==nullptr) {
        cogWheelError("Error: Could not create controller socket.");
        return;
    }

    if (!m_controllerSocket->setSocketDescriptor(handle)) {
        cogWheelError("Error setting up socket for control controller.");
        m_controllerSocket->deleteLater();
        return;
    }

    // Connect up signals/slots, enable logging, send status to manager and clear local conenction list

    connectUpControllerSocket();
    enableLoggingToManager(true);
    writeCommandToManager(kCWCommandSTATUS, (m_server) ? kCWStatusRUNNING : kCWStatusSTOPPED);
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

    cogWheelInfo("CogWheel manager connected to local controller.");

    enableLoggingToManager(true);

}

/**
 * @brief CogWheelController::disconnected
 *
 * CogWheel Manager disonnected.
 *
 */
void CogWheelController::disconnected()
{
    cogWheelInfo("CogWheel manager disconnected from local controller.");

    enableLoggingToManager(false);

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
        cogWheelInfo("Manager disconnected listen for a new connection....");
        return;
    }
    if (socketError==QLocalSocket::ServerNotFoundError) {
        cogWheelInfo("Manager not running at present....");
        return;
    }
    cogWheelError( static_cast<QLocalSocket*>(sender())->errorString());;
}

/**
 * @brief CogWheelController::readyRead
 *
 * Read CogWheel Manager commands sent to controller. Even if a command
 * has successfully been processed there still may be buffered commands
 * so loop.
 *
 */
void CogWheelController::readyRead()
{

    // Loop while bytes avaialable

    while (m_controllerSocket->bytesAvailable()) {

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
            cogWheelWarning("Manager command [" + command + "] not valid.");
        }

    }

}

/**
 * @brief CogWheelController::updateConnectionList
 * @param connections
 */
void CogWheelController::updateConnectionList(const QStringList &connectionList)
{

    if (m_lastConnectionList != connectionList) {
        writeCommandToManager(kCWCommandCONNECTIONS, connectionList);
        m_lastConnectionList = connectionList;
    }

}

/**
 * @brief CogWheelController::flushLogToManager
 *
 * Flush loggin buffer to server and then clear it.
 *
 */
void CogWheelController::flushLogToManager()
{

    if (!CogWheelLogger::getInstance().getLoggingBuffer().isEmpty()) {
        writeCommandToManager(kCWCommandLOGOUTPUT, CogWheelLogger::getInstance().getLoggingBuffer());
        CogWheelLogger::getInstance().clearLoggingBuffer();
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
            cogWheelError("Unable to allocate server object");
        }
        connect(m_server->connections(), &CogWheelConnections::updateConnectionList, this, &CogWheelController::updateConnectionList);
    } else {
        cogWheelWarning("CogWheel Server already started.");
    }

    writeCommandToManager(kCWCommandSTATUS, kCWStatusRUNNING);

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
        cogWheelWarning("CogWheel Server already stopped.");
    }

    writeCommandToManager(kCWCommandSTATUS, kCWStatusSTOPPED);

}

/**
 * @brief CogWheelController::killServer
 *
 * Kill server process.
 *
 * @param input
 */
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



