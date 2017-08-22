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
        emit error("Connection already being used");
        return;
    }

    // Use scope pointers to enable easy tidyup on error

    QScopedPointer<CogWheelControlChannel> connection { new CogWheelControlChannel(m_serverSettings) };
    QScopedPointer<QThread> connectionThread { new QThread() };

    if (connection==nullptr) {
        emit error("Failed to create connection.");
        return;
    }

    if (connectionThread==nullptr) {
        emit error("Failed to create connection thread.");
        return;
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

    // Delete thread when it is finished

    connect(m_connections[handle]->connectionThread(),&QThread::finished,m_connections[handle]->connectionThread(), &QThread::deleteLater );

    // Open control channel

    emit openConnection(handle);

    emit info("Number of active connections: "+QString::number(m_connections.size()));

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
    emit info("Removing connection for handle : "+QString::number(handle));

    if (!m_connections.contains(handle)) {
        emit error("Connection not present for handle: "+QString::number(handle));
        return;
    }

    CogWheelControlChannel *connection = m_connections[handle];

    m_connections.remove(handle);
    connection->deleteLater();

    emit info("Number of active connections: "+QString::number(m_connections.size()));

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
    emit error("Aborting connection for handle: "+QString::number(handle));
    finishedConnection(handle);
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
