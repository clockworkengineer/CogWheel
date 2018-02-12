/*
 * File:   cogwheelconnections.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

//
// Class: CogWheelConnections
//
// Description: Class to accept new FTP connections from the
// server, create an instance of  control channel, a new thread
// to run the channel on and open the channel. The connection is
// removed on the reciept of a signal to the finshedConnection slot
// function which removes the connection from the list of current
// connections.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelconnections.h"
#include "cogwheellogger.h"

// ====================
// CLASS IMPLEMENTATION
// ====================

/**
 * @brief CogWheelConnections::CogWheelConnections
 *
 * Empty constructor.
 *
 * @param parent
 */
CogWheelConnections::CogWheelConnections(QObject *parent) : QObject(parent)
{

}

/**
 * @brief CogWheelConnections::~CogWheelConnections
 *
 * Destructor. Close all open connections.
 *
 */
CogWheelConnections::~CogWheelConnections()
{
    closeAll();
}

/**
 * @brief CogWheelConnections::
 *
 * Close all open/active conenctions.
 *
 */
void CogWheelConnections::closeAll()
{
    emit closeAllConnections();
    resetConnectionListUpdateTimer();
}

/**
 * @brief CogWheelConnections::acceptConnection
 *
 * Accept incoming FTP client connection. Create a control channel
 * object, setup the signals/slots for communicating with it, create
 * a  thread and move the channel to it, store channel in the current
 * connections array  and open the control channel for receiving commands.
 *
 * @param handle    Control socket handle.
 */
void CogWheelConnections::acceptConnection(qint64 handle)
{

    if (m_connections.contains(handle)) {
        cogWheelError("Connection already being used");
        return;
    }

    // Use scope pointers to enable easy tidyup on error

    QScopedPointer<CogWheelControlChannel> connection { new CogWheelControlChannel(m_serverSettings) };
    QScopedPointer<QThread> connectionThread { new QThread() };

    if (connection==nullptr) {
        throw CogWheelConnections::Exception("Could not create connection object.");
    }

    if (connectionThread==nullptr) {
        throw CogWheelConnections::Exception("Could not create thread for conenction.");
    }

    // Create thread and run control channel on it.

    connection->setConnectionThread( connectionThread.take());
    connection->moveToThread(connection->connectionThread());
    connection->connectionThread()->start();

    // Store connection

    m_connections[handle] = connection.take();

    // Disconnect any old open handlers

    disconnect(this, &CogWheelConnections::openConnection,0,0);

    // Setup slots

    connect(this,&CogWheelConnections::openConnection, m_connections[handle], &CogWheelControlChannel::openConnection);
    connect(m_connections[handle],&CogWheelControlChannel::finishedConnection,this, &CogWheelConnections::finishedConnection);
    connect(m_connections[handle],&CogWheelControlChannel::abortedConnection,this, &CogWheelConnections::finishedConnection);
    connect(this,&CogWheelConnections::closeAllConnections, m_connections[handle], &CogWheelControlChannel::closeConnection);

    // Delete thread when it is finished

    connect(m_connections[handle]->connectionThread(),&QThread::finished,m_connections[handle]->connectionThread(), &QThread::deleteLater );

    // Open control channel

    emit openConnection(handle);

    cogWheelInfo("Number of active connections now: "+QString::number(m_connections.size()));

    // Set timer running for connection list update to manager

    if (!m_connections.empty() && !m_connectionListUpdateTimer) {
        m_connectionListUpdateTimer = new QTimer();
        connect(m_connectionListUpdateTimer, &QTimer::timeout, this, &CogWheelConnections::connectionListToManager);
        m_connectionListUpdateTimer->start(m_serverSettings.connectionListUpdateTime());
    }

}

/**
 * @brief CogWheelConnections::finishedConnection
 *
 * Control channel closed down so remove from connections array
 * and flag its memory to be removed.
 *
 * @param handle    Socket handle connection stored under.
 *
 */
void CogWheelConnections::finishedConnection(qint64 handle)
{
    cogWheelInfo("Removing connection for handle : "+QString::number(handle));

    if (!m_connections.contains(handle)) {
        throw CogWheelConnections::Exception("Connection not present for handle: "+QString::number(handle));;
    }

    CogWheelControlChannel *connection = m_connections[handle];

    m_connections.remove(handle);
    connection->deleteLater();

    if (!m_connections.isEmpty()) {
        cogWheelInfo("Number of active connections: "+QString::number(m_connections.size()));
    } else {
        // Send empty connection list and reset/stop timer.
        connectionListToManager();
        resetConnectionListUpdateTimer();
        cogWheelInfo("No active connections on server.");
    }

}

/**
 * @brief CogWheelConnections::connectionListToManager
 *
 * Send current connection list to manager.
 *
 */
void CogWheelConnections::connectionListToManager()
{
    QStringList connectionList;

    for( auto &connection : m_connections) {
        if (connection->isAuthorized()) {
            connectionList.append(connection->userName());
        } else {
            connectionList.append(QString::number(connection->socketHandle()));
        }
    }

    emit updateConnectionList(connectionList);

}

/**
 * @brief CogWheelConnections::abortedConnection
 *
 * Control channel aborted. Just do a finish.
 *
 * @param handle    Socket handle connection stored under.
 */
void CogWheelConnections::abortedConnection(qint64 handle)
{
    cogWheelError("Aborting connection for handle: "+QString::number(handle));
    finishedConnection(handle);
}

/**
 * @brief CogWheelConnections::resetConnectionListUpdateTimer
 *
 * Stop and delete connection list update timer.
 *
 */
void CogWheelConnections::resetConnectionListUpdateTimer()
{
    if (m_connectionListUpdateTimer) {
        m_connectionListUpdateTimer->stop();
        m_connectionListUpdateTimer->deleteLater();
        m_connectionListUpdateTimer=nullptr;
    }
}


// ============================
// CLASS PRIVATE DATA ACCESSORS
// ============================

/**
 * @brief CogWheelConnections::serverSettings
 *
 * Get server settings.
 *
 * @return  Server Settings
 */
CogWheelServerSettings CogWheelConnections::serverSettings() const
{
    return m_serverSettings;
}

/**
 * @brief CogWheelConnections::setServerSettings
 *
 * Store server settings.
 *
 * @param serverSettings
 */
void CogWheelConnections::setServerSettings(const CogWheelServerSettings &serverSettings)
{
    m_serverSettings = serverSettings;
}

