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

// ====================
// CLASS IMPLEMENTATION
// ====================

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
        throw CogWheelController::Exception("Unable to allocate server object.");
    }

    // Save QtApplication object instance

    m_cogWheelApplication = cogWheelApp;

    // Controller response signal/slots

    connect(m_server->connections(), &CogWheelConnections::updateConnectionList, this, &CogWheelController::updateConnectionList);

    // Logging flush timer

    m_logFlushTimer = new QTimer();
    connect(m_logFlushTimer, &QTimer::timeout, this, &CogWheelController::flushLoggingBufferToManager);
    m_logFlushTimer->start(kCWLoggingFlushTimer);


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
 * @brief CogWheelController::connectUpControllerSocket
 *
 * Connect up controller siganls/slots and setup read/write data streams.
 *
 */
void CogWheelController::connectUpControllerSocket()
{

    connect(m_controllerSocket,&QLocalSocket::connected, this, &CogWheelController::connected);
    connect(m_controllerSocket,&QLocalSocket::disconnected, this, &CogWheelController::disconnected);
    connect(m_controllerSocket,static_cast<void(QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error), this, &CogWheelController::error);
    connect(m_controllerSocket,&QLocalSocket::readyRead, this, &CogWheelController::readyRead);

    m_controllerReadStream.setVersion(QDataStream::Qt_4_7);
    m_controllerReadStream.setDevice(m_controllerSocket);

    m_writeQBuffer.setBuffer(&m_writeRawDataBuffer);
    m_writeQBuffer.open(QIODevice::WriteOnly);
    m_controllerWriteStream.setVersion(QDataStream::Qt_4_7);
    m_controllerWriteStream.setDevice(&m_writeQBuffer);

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
        throw CogWheelController::Exception("Controller unable listen on socket name: " + m_serverName);
    }

    m_controllerSocket = new QLocalSocket();
    if (m_controllerSocket==nullptr) {
        throw CogWheelController::Exception("Error in creating controller socket.");
    }

    connectUpControllerSocket();

    m_controllerSocket->connectToServer(m_serverName+kCWManagerPostfix);
    m_controllerSocket->waitForConnected(-1);

    if (m_controllerSocket->state() != QLocalSocket::ConnectedState) {
        resetControllerSocket();  // Error reset socket and return
        cogWheelInfo("Error could not connect server controller.");
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

    resetLoggingFlushTimer();

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

    if (m_controllerSocket && (m_controllerSocket->state() == QLocalSocket::ConnectedState)) {
        m_writeRawDataBuffer.clear();
        m_controllerWriteStream.device()->seek(0);
        m_controllerWriteStream << (quint32)0;
        m_controllerWriteStream << command;
        m_controllerWriteStream << param1;
        m_controllerWriteStream.device()->seek(0);
        m_controllerWriteStream << (quint32)(m_writeRawDataBuffer.size() - sizeof(quint32));
        m_controllerSocket->write(m_writeRawDataBuffer);
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

    if (m_controllerSocket && (m_controllerSocket->state() == QLocalSocket::ConnectedState)) {
        m_writeRawDataBuffer.clear();
        m_controllerWriteStream.device()->seek(0);
        m_controllerWriteStream << (quint32)0;
        m_controllerWriteStream << command;
        m_controllerWriteStream << param1;
        m_controllerWriteStream.device()->seek(0);
        m_controllerWriteStream << (quint32)(m_writeRawDataBuffer.size() - sizeof(quint32));
        m_controllerSocket->write(m_writeRawDataBuffer);
        m_controllerSocket->flush();
    }

}

/**
 * @brief CogWheelController::resetControllerSocket
 *
 * Reset controller socket and write QBuffer.
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
    if (m_writeQBuffer.isOpen()) {
        m_writeQBuffer.close();
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
        flushLoggingBufferToManager();
        m_logFlushTimer->stop();
        m_logFlushTimer->deleteLater();
        m_logFlushTimer=nullptr;
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

    m_controllerSocket = new QLocalSocket();
    if (m_controllerSocket==nullptr) {
        throw CogWheelController::Exception("Could not create controller socket.");
    }

    if (!m_controllerSocket->setSocketDescriptor(handle)) {
        m_controllerSocket->deleteLater();
        throw CogWheelController::Exception("Setting up socket for control controller.");
    }

    // Connect up signals/slots, send status to manager and clear local conenction list

    connectUpControllerSocket();

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
    cogWheelError( static_cast<QLocalSocket*>(sender())->errorString());
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

        // Read blocksize header

        if (m_commandBlockSize == 0) {
            if (m_controllerSocket->bytesAvailable() < (int)sizeof(quint32)) {
                return;
            }
            m_controllerReadStream >> m_commandBlockSize;
        }

        // Bytes still to read

        if (m_controllerSocket->bytesAvailable() < m_commandBlockSize || m_controllerReadStream.atEnd()) {
            return;
        }

        // Read command and if valid process. Note data stream passed to command function to
        // enable any command parameters to be processed.

        QString command;
        m_controllerReadStream >> command;

        if (m_managerCommandTable.contains(command)) {
            (this->*m_managerCommandTable[command])(m_controllerReadStream);
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
 * @brief CogWheelController::flushLoggingBufferToManager
 *
 * Flush logging buffer to server and then clear it.
 * It also flushs data to any log file of it exists.
 *
 */
void CogWheelController::flushLoggingBufferToManager()
{

    if (!CogWheelLogger::getInstance().getLoggingBuffer().isEmpty()) {
        writeCommandToManager(kCWCommandLOGOUTPUT, CogWheelLogger::getInstance().getLoggingBuffer());
        CogWheelLogger::getInstance().clearLoggingBuffer();
    }

    flushLoggingFile();

}

// ===================
// CONTROLLER COMMANDS
// ===================

/**
 * @brief CogWheelController::startServer
 *
 * Start FTP Server.
 *
 * @param controllerInputStream
 */
void CogWheelController::startServer(QDataStream &controllerInputStream)
{
    Q_UNUSED(controllerInputStream);

    if (m_server==nullptr) {
        m_server = new CogWheelServer(true);
        if (m_server==nullptr) {
            throw CogWheelController::Exception("Unable to allocate server object");
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
 * @param controllerInputStream
 */
void CogWheelController::stopServer(QDataStream &controllerInputStream)
{

    Q_UNUSED(controllerInputStream);

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
 * @param controllerInputStream
 */
void CogWheelController::killServer(QDataStream &controllerInputStream)
{
    stopServer(controllerInputStream);
    writeCommandToManager(kCWCommandSTATUS, kCWStatusTERMINATED);
    cogWheelInfo("CogWheel Server terminated.");
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



