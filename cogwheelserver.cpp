/*
 * File:   cogwheelserver.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

//
// Class: CogWheelServer
//
// Description: Base server class that loads any settings, listens for
// connections and passes them to the connections handler class.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelserver.h"
#include <QtCore>

/**
 * @brief CogWheelServer::CogWheelServer
 *
 * Initialise Organisation and Application names, load server
 * settings and start server if specified.
 *
 * @param autoStart     If true then start the server
 * @param parent        Parent object which should be null.
 */
CogWheelServer::CogWheelServer(bool autoStart, QObject *parent) : QTcpServer(parent)
{

    QCoreApplication::setOrganizationName("ClockWorkEngineer");
    QCoreApplication::setApplicationName("CogWheel");

    connect(&m_connections, &CogWheelConnections::error, this, &CogWheelServer::error);
    connect(&m_connections, &CogWheelConnections::info, this, &CogWheelServer::info);
    connect(&m_connections, &CogWheelConnections::warning, this, &CogWheelServer::warning);

    emit info("Loading CogWheel FTP Server Settings...");

    m_serverSettings.load();

    if (m_serverSettings.serverSslEnabled()) {
        if (m_serverSettings.loadPrivateKeyAndCert()) {
            emit info("Server Private Key & Certicate Loaded.");
        } else {
            m_serverSettings.setServerSslEnabled(false);
            emit info("Error Loading Server Private & Certicate SSL Disabled.");
        }
    }

    // Setup server settings

    m_connections.setServerSettings (m_serverSettings);
    m_ftpServer.setupServer(m_serverSettings);

    if (autoStart) {
        startServer();
    }

}

/**
 * @brief CogWheelServer::~CogWheelServer
 */
CogWheelServer::~CogWheelServer()
{

}

/**
 * @brief CogWheelServer::startServer
 *
 * Start listening for client connections on the specified port and also
 * setup the accept signal to the connections class.
 *
 */
void CogWheelServer::startServer()
{
    info("CogWheel FTP Server started.");

    if (listen(QHostAddress::Any, m_serverSettings.serverPort())) {
        info("CogWheel Server listening on port "+QString::number(m_serverSettings.serverPort()));
        connect(this,&CogWheelServer::accept, &m_connections, &CogWheelConnections::acceptConnection);
        setRunning(true);
    } else {
        error("CogWheel Server listen failure.");
    }

}

/**
 * @brief CogWheelServer::stopServer
 *
 * Stop server running. Closing all active connections.
 *
 *
 */
void CogWheelServer::stopServer()
{
    info("CogWheel Server stopped.");

    setRunning(false);

    m_connections.closeAll();

}

/**
 * @brief CogWheelServer::incomingConnection
 *
 * QTcpServer override for incoming connections. Here we just signal
 * the connections handler class with the socket handle.
 *
 * @param handle    Client connectin socket handle.
 */
void CogWheelServer::incomingConnection(qintptr handle)
{
    info("--- CogWheel Server incoming connection --- "+QString::number(handle));

    emit accept(handle);

}

/**
 * @brief CogWheelServer::error
 *
 * Produce error trace message.
 *
 * @param message  Message string.
 */
void CogWheelServer::error(const QString &message)
{
    qDebug() << message.toStdString().c_str();
}

/**
 * @brief CogWheelServer::info
 *
 * Produce informational trace message.
 *
 * @param message  Message string.
 */
void CogWheelServer::info(const QString &message)
{
    qInfo() << message.toStdString().c_str();
}

/**
 * @brief CogWheelServer::warning
 *
 * Produce warning trace message.
 *
 * @param message  Message string.
 */
void CogWheelServer::warning(const QString &message)
{
    qWarning() << message.toStdString().c_str();
}

// ============================
// CLASS PRIVATE DATA ACCESSORS
// ============================

/**
 * @brief CogWheelServer::isRunning
 * @return
 */
bool CogWheelServer::isRunning() const
{
    return m_running;
}

/**
 * @brief CogWheelServer::setRunning
 * @param running
 */
void CogWheelServer::setRunning(bool running)
{
    m_running = running;
}
