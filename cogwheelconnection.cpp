#include "cogwheelconnection.h"
#include"cogwheelftpcore.h"

#include <QCoreApplication>

CogWheelConnection::CogWheelConnection(qintptr socketHandle,  QObject *parent) : QObject(parent), m_socketHandle(socketHandle)
{

    qDebug() << "CogWheelConnection creation";

    m_currentWorkingDirectory =  QCoreApplication::applicationDirPath();

}

void CogWheelConnection::start()
{
    m_controlChannelSocket = new QTcpSocket();

    if (!m_controlChannelSocket->setSocketDescriptor(m_socketHandle)) {
        qWarning () << "CogWheelConnection error setting up socket";
        return;
    }

    connect(m_controlChannelSocket, &QTcpSocket::connected, this, &CogWheelConnection::connected);
    connect(m_controlChannelSocket, &QTcpSocket::disconnected, this, &CogWheelConnection::disconnected);
    connect(m_controlChannelSocket, &QTcpSocket::readyRead, this, &CogWheelConnection::readyRead);
    connect(m_controlChannelSocket, &QTcpSocket::bytesWritten, this, &CogWheelConnection::bytesWritten);

    sendReply(200);

}

void CogWheelConnection::processFTPCommand(QString command)
{
    QStringList commandAndArguments;

    command.chop(2);

    commandAndArguments = command.split(' ');

    qDebug() << "Command = [" << commandAndArguments[0] << "]";

    CogWheelFTPCore::performCommand(this, commandAndArguments);

}

void CogWheelConnection::sendReply(quint16 replyCode, QString message)
{

    QString replyStr { QString::number(replyCode) + " " + message + "\r\n"};
    QByteArray reply { replyStr.toUtf8() };
    m_controlChannelSocket->write(reply.data());
    qDebug() << "SendReply [" << replyStr << "]";

}

void CogWheelConnection::sendReply(quint16 replyCode)
{
    sendReply(replyCode, CogWheelFTPCore::getResponseText(replyCode));
}

void CogWheelConnection::sendData(QString reponse)
{

    if (m_dataChannel->m_dataChannelSocket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "NOT CONNECTED BUT " << m_dataChannel->m_dataChannelSocket->state();
    }

    QByteArray reply { reponse.toUtf8() };
    m_dataChannel->m_dataChannelSocket->write(reply.data());
    qDebug() << "Data [" << reply.data() << "]";
    if (m_dataChannel->m_dataChannelSocket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "CONNECTED.";
    }

}

void CogWheelConnection::connected()
{
    qDebug() << "CogWheelConnection connected...";
}

void CogWheelConnection::disconnected()
{
    qDebug() << "CogWheelConnection disconnected.";
}

void CogWheelConnection::readyRead()
{
    qDebug() << "CogWheelConnection readReady";

    processFTPCommand(m_controlChannelSocket->readAll());

}

void CogWheelConnection::bytesWritten(qint64 numberOfBytes)
{
    qDebug() << "CogWheelConnection bytesWritten" << numberOfBytes;
}

