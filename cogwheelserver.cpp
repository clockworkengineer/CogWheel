#include "cogwheelserver.h"

CogWheelServer::CogWheelServer(QObject *parent) : QTcpServer(parent)
{
    qDebug() << "CogWheelServer created.";
}

void CogWheelServer::startServer()
{
    qDebug() << "CogWheelServer started....";

    m_serverThread.start();

    if (listen(QHostAddress::Any, m_controlPort)) {
        qDebug() << "CogWheelServer listening....";
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
    qDebug() << "CogWheelServer incomingConnection" << handle;

    CogWheelConnection *connection = new CogWheelConnection(handle);

    m_connections.append(connection);

    // connection->moveToThread(&m_serverThread);

    connection->start();

}
