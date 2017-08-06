#include "cogwheelserver.h"
#include <QtCore>

CogWheelServer::CogWheelServer(QObject *parent) : QTcpServer(parent)
{
    qDebug() << "CogWheelServer FTP Server created.";

    QCoreApplication::setOrganizationName("ClockWorkEngineer");
    QCoreApplication::setApplicationName("CogWheel");

//    m_connections.moveToThread(&m_serverThread);

}

void CogWheelServer::startServer()
{
    qDebug() << "CogWheelServer FTP Server started....";

    if (listen(QHostAddress::Any, m_controlPort)) {
        qDebug() << "CogWheelServer listening....";
    } else {
        qDebug() << "CogWheelServer listen failure.";
    }

    //m_serverThread.start();

}

void CogWheelServer::stopServer()
{
    qDebug() << "CogWheelServer stopped.";
}

void CogWheelServer::incomingConnection(qintptr handle)
{
    qDebug() << "CogWheelServer incomingConnection" << handle;

    connect(this,&CogWheelServer::open, &m_connections, &CogWheelConnections::accept);

    emit open(handle);

}
