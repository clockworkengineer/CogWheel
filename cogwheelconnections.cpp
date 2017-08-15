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
// Description:
//
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelconnections.h"

CogWheelConnections::CogWheelConnections(QObject *parent) : QObject(parent)
{

}

void CogWheelConnections::acceptConnection(qint64 handle)
{

    if (m_connections.contains(handle)) {
        qWarning() << "CogWheelConnections::accept() : connection already bsing used";
        return;
    }

    CogWheelControlChannel *connection = new CogWheelControlChannel(m_serverSettings);

    if (connection==nullptr) {
        qWarning() << "CogWheelConnections::accept() : failed to create connection.";
        return;
    }

    connection->setConnectionThread( new QThread());

    if (connection->connectionThread()==nullptr) {
        qWarning() << "CogWheelConnections::accept() : failed to create thread.";
        return;
    }

    m_connections[handle] = connection;

    connection->moveToThread(connection->connectionThread());

    connection->connectionThread()->start();

    disconnect(this, &CogWheelConnections::openConnection,0,0);

    connect(this,&CogWheelConnections::openConnection, connection, &CogWheelControlChannel::openConnection);
    connect(connection,&CogWheelControlChannel::finishedConnection,this, &CogWheelConnections::finishedConnection);
    connect(connection,&CogWheelControlChannel::abortedConnection,this, &CogWheelConnections::finishedConnection);

    connect(connection->connectionThread(),&QThread::finished,connection->connectionThread(), &QThread::deleteLater );

    emit openConnection(handle);

    qDebug() << "NUMBER OF CONNECTIONS: " << m_connections.size();

}

void CogWheelConnections::finishedConnection(qint64 handle)
{
    qDebug() << "CogWheelConnections::finishedConnection: removing connection";

    if (!m_connections.contains(handle)) {
        qWarning() << "CogWheelConnections::close() : connection not present.";
        return;
    }

    CogWheelControlChannel *connection = m_connections[handle];

    m_connections.remove(handle);
    connection->deleteLater();

}


void CogWheelConnections::abortedConnection(qint64 handle)
{
    qDebug() << "CogWheelConnections::abortedConnection: aborting connection";
    finishedConnection(handle);
}

CogWheelServerSettings *CogWheelConnections::serverSettings() const
{
    return m_serverSettings;
}

void CogWheelConnections::setServerSettings(CogWheelServerSettings *serverSettings)
{
    m_serverSettings = serverSettings;
}
