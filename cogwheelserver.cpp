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

}

void CogWheelServer::startServer()
{
    qDebug() << "CogWheelServer FTP Server started on thread " << QThread::currentThreadId();

    if (listen(QHostAddress::Any, m_controlPort)) {
        qDebug() << "CogWheelServer listening....";        
        connect(this,&CogWheelServer::acceptConnection, &m_connections, &CogWheelConnections::acceptConnection);
    } else {
        qDebug() << "CogWheelServer listen failure.";
    }

}

void CogWheelServer::stopServer()
{
    qDebug() << "CogWheelServer stopped.";
}

void CogWheelServer::incomingConnection(qintptr handle)
{
    qDebug() << "--- CogWheelServer incomingConnection ---" << handle;

    emit acceptConnection(handle);

}
