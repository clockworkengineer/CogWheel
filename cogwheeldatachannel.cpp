#include "cogwheeldatachannel.h"

CogWheelDataChannel::CogWheelDataChannel(QObject *parent) : QObject(parent)
{
    qDebug() << "CogWheelDataChannel created.";

    m_dataChannelSocket = new QTcpSocket();

    connect(m_dataChannelSocket, &QTcpSocket::connected, this, &CogWheelDataChannel::connected, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QTcpSocket::disconnected, this, &CogWheelDataChannel::disconnected, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QTcpSocket::stateChanged, this, &CogWheelDataChannel::stateChanged, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QTcpSocket::bytesWritten, this, &CogWheelDataChannel::bytesWritten), Qt::DirectConnection;
    connect(m_dataChannelSocket, &QTcpSocket::readyRead, this, &CogWheelDataChannel::readyRead, Qt::DirectConnection);

}

void CogWheelDataChannel::connectToClient()
{
    m_dataChannelSocket->connectToHost(m_clientHostIP, m_clientHostPort);
    m_dataChannelSocket->waitForConnected();

    qDebug() << "-- File Socket State --" << m_dataChannelSocket->state();

    if (m_dataChannelSocket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "-- File Socket Error String --" << m_dataChannelSocket->errorString();
        return;
    }

    m_connected=true;

}

void CogWheelDataChannel::disconnectFromClient()
{
    if (m_dataChannelSocket->state() == QAbstractSocket::ConnectedState) {
        m_dataChannelSocket->disconnectFromHost();
        m_dataChannelSocket->waitForDisconnected(-1);
        m_connected=false;
    } else {
        qDebug() << "CogWheelDataChannel::disconnectFromHost: socket not connected.";
    }
}

void CogWheelDataChannel::setClientHostIP(QString clientIP)
{
    qDebug() << "CogWheelDataChannel::setClientHostIP " << clientIP;
    m_clientHostIP.setAddress(clientIP);
}

void CogWheelDataChannel::setClientHostPort(quint16 clientPort)
{

    qDebug() << "CogWheelDataChannel::setClientHostPort " << clientPort;
    m_clientHostPort = clientPort;
}

void CogWheelDataChannel::connected()
{
    qDebug() << "CogWheelDataChannel::connected()";
}

void CogWheelDataChannel::disconnected()
{
    qDebug() << "CogWheelDataChannel::disconnected()";
}

void CogWheelDataChannel::stateChanged(QAbstractSocket::SocketState socketState)
{
    qDebug() << "CogWheelDataChannel::stateChanged: " << socketState;
}

void CogWheelDataChannel::bytesWritten(qint64 numBytes)
{
    qDebug() << "CogWheelDataChannel::bytesWritten: " << numBytes;
}

void CogWheelDataChannel::readyRead()
{
    qDebug() << "CogWheelDataChannel::readyRead()";
}
