#include "cogwheelconnections.h"

CogWheelConnections::CogWheelConnections(QObject *parent) : QObject(parent)
{

}

void CogWheelConnections::accept(qint64 handle)
{
    CogWheelConnection *connection = new CogWheelConnection();

    m_connections.append(connection);

   // connection->moveToThread(&connection->m_connectionThread);

   // connection->m_connectionThread.start();

    connect(this,&CogWheelConnections::open, connection, &CogWheelConnection::open);

    emit open(handle);

}

void CogWheelConnections::close(qint64 handle)
{

}

//void CogWheelConnections::finished(qint64 handle)
//{

//}

void CogWheelConnections::aborted(qint64 handle)
{

}
