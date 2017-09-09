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
#include "cogwheellogger.h"

/**
 * @brief CogWheelServer::CogWheelServer
 *
 * Load server settings and start server if specified.
 *
 * @param autoStart     If true then start the server
 * @param parent        Parent object which should be null.
 */
CogWheelServer::CogWheelServer(bool autoStart, QObject *parent) : QTcpServer(parent)
{

    cogWheelInfo("Loading CogWheel FTP Server Settings...");

    m_serverSettings.load();

    if (m_serverSettings.serverSslEnabled()) {
        if (m_serverSettings.loadPrivateKeyAndCert()) {
            cogWheelInfo("Server Private Key & Certicate Loaded.");
        } else {
            m_serverSettings.setServerSslEnabled(false);
            cogWheelInfo("Error Loading Server Private & Certicate SSL Disabled.");
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
    cogWheelInfo("CogWheel FTP Server started.");

    if (listen(QHostAddress::Any, m_serverSettings.serverPort())) {
        cogWheelInfo("CogWheel Server listening on port "+QString::number(m_serverSettings.serverPort()));
        connect(this,&CogWheelServer::accept, &m_connections, &CogWheelConnections::acceptConnection);
        setRunning(true);
    } else {
        cogWheelError("CogWheel Server listen failure.");
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
    cogWheelInfo("CogWheel Server stopped.");

    setRunning(false);

    m_connections.closeAll();

}

/**
 * @brief CogWheelServer::incomingConnection
 *
 * QTcpServer override for incoming connections. Here we just signal
 * the connections handler class with the socket handle.
 *
 * @param handle    Client connecting socket handle.
 */
void CogWheelServer::incomingConnection(qintptr handle)
{
    cogWheelInfo("--- CogWheel Server incoming connection --- "+QString::number(handle));

    emit accept(handle);

}

// ============================
// CLASS PRIVATE DATA ACCESSORS
// ============================

/**
 * @brief CogWheelServer::connections
 * @return
 */
CogWheelConnections *CogWheelServer::connections() const
{
    return const_cast<CogWheelConnections*>(&m_connections);
}

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
