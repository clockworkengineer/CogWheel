#include "cogwheelconnection.h"
#include"cogwheelftpcore.h"

#include <QCoreApplication>

CogWheelConnection::CogWheelConnection(QObject *parent) : QObject(parent)
{

    qDebug() << "CogWheelConnection creation";

    m_currentWorkingDirectory =  QCoreApplication::applicationDirPath();

}

void CogWheelConnection::processFTPCommand(QString command)
{
    QStringList commandAndArguments;

    command.chop(2);

    commandAndArguments = command.split(' ');

    qDebug() << "Command = [" << commandAndArguments[0] << "]";

    CogWheelFTPCore::performCommand(this, commandAndArguments);

}

void CogWheelConnection::open(qint64 socketHandle)
{
        m_controlChannelSocket = new QTcpSocket();

        m_socketHandle = socketHandle;
        if (!m_controlChannelSocket->setSocketDescriptor(m_socketHandle)) {
            qWarning () << "CogWheelConnection error setting up socket";
            return;
        }

        connect(m_controlChannelSocket, &QTcpSocket::connected, this, &CogWheelConnection::connected, Qt::DirectConnection);
        connect(m_controlChannelSocket, &QTcpSocket::disconnected, this, &CogWheelConnection::disconnected, Qt::DirectConnection);
        connect(m_controlChannelSocket, &QTcpSocket::readyRead, this, &CogWheelConnection::readyRead, Qt::DirectConnection);
        connect(m_controlChannelSocket, &QTcpSocket::bytesWritten, this, &CogWheelConnection::bytesWritten, Qt::DirectConnection);

        sendReplyCode(200);

}

void CogWheelConnection::close()
{
    qDebug() << "CogWheelConnection::close(): " << m_socketHandle;

    if (m_controlChannelSocket == nullptr) {
        qWarning() << "m_controlChannelSocket == nullptr";
        return;
    }

    m_controlChannelSocket->close();
    m_controlChannelSocket->deleteLater();

    if ( m_dataChannel.m_dataChannelSocket == nullptr) {
        qWarning() << " m_dataChannel.m_dataChannelSocket == nullptr";
        return;
    }

    m_dataChannel.m_dataChannelSocket->close();
    m_dataChannel.m_dataChannelSocket->deleteLater();

}

void CogWheelConnection::sendReplyCode(quint16 replyCode, QString message)
{

    QString replyStr { QString::number(replyCode) + " " + message + "\r\n"};
    QByteArray reply { replyStr.toUtf8() };
    m_controlChannelSocket->write(reply.data());
    qDebug() << "SendReply [" << replyStr << "]";

}

void CogWheelConnection::sendReplyCode(quint16 replyCode)
{
    sendReplyCode(replyCode, CogWheelFTPCore::getResponseText(replyCode));
}

void CogWheelConnection::sendOnDataChannel(QString data)
{
    m_dataChannel.m_dataChannelSocket->write(data.toUtf8().data());
    qDebug() << "Data [" << data.toUtf8().data() << "]";
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

