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
// Description:
//
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelserver.h"
#include <QtCore>

CogWheelServer::CogWheelServer(QObject *parent) : QTcpServer(parent)
{
    qDebug() << "CogWheelServer FTP Server created.";

    QCoreApplication::setOrganizationName("ClockWorkEngineer");
    QCoreApplication::setApplicationName("CogWheel");

    m_serverSettings.load();
    m_connections.setServerSettings (&m_serverSettings);

}

void CogWheelServer::startServer()
{
    qDebug() << "CogWheel FTP Server started on thread " << QThread::currentThreadId();

    if (listen(QHostAddress::Any, m_serverSettings.serverPort())) {
        qDebug() << "CogWheel Server listening on port " << m_serverSettings.serverPort();
        connect(this,&CogWheelServer::acceptConnection, &m_connections, &CogWheelConnections::acceptConnection);
    } else {
        qDebug() << "CogWheel Server listen failure.";
    }

}

void CogWheelServer::stopServer()
{
    qDebug() << "CogWheel Server stopped.";
}

void CogWheelServer::incomingConnection(qintptr handle)
{
    qDebug() << "--- CogWheel Server incoming connection ---" << handle;

    emit acceptConnection(handle);

}
