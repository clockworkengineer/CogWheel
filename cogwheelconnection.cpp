#include "cogwheelconnection.h"
#include"cogwheelftpcore.h"

#include <QCoreApplication>

CogWheelConnection::CogWheelConnection(QObject *parent) : QObject(parent)
{

    //  qDebug() << "CogWheelConnection creation";

    m_currentWorkingDirectory =  QCoreApplication::applicationDirPath();

}

void CogWheelConnection::processFTPCommand(QString command)
{
    QStringList commandAndArguments;

    command.chop(2);

    if (command.contains(' ')) {
        commandAndArguments.append(command.mid(0, command.indexOf(' ')));
        command.remove(0, command.indexOf(' ')+1);
    }
    commandAndArguments.append(command);

    qDebug() << "Command = [" << commandAndArguments[0] << "]";
    if (commandAndArguments.size()>1) {
         qDebug() << "Arguments = [" << commandAndArguments[1] << "]";
    }

    CogWheelFTPCore::performCommand(this, commandAndArguments);

}

void CogWheelConnection::openConnection(qint64 socketHandle)
{

    qDebug() << "CogWheelConnection::openConnection: on thread " << QThread::currentThreadId();

    m_dataChannel = new CogWheelDataChannel();

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

    connect(m_dataChannel,&CogWheelDataChannel::uploadFinished, this,&CogWheelConnection::uploadFinished, Qt::DirectConnection);
    connect(m_dataChannel, &CogWheelDataChannel::dataChannelSocketError, this, &CogWheelConnection::dataChannelSocketError, Qt::DirectConnection);
    connect(m_dataChannel, &CogWheelDataChannel::passiveConnection, this, &CogWheelConnection::passiveConnection, Qt::DirectConnection);

    sendReplyCode(200);

}

void CogWheelConnection::closeConnection()
{
    qDebug() << "CogWheelConnection::close(): " << m_socketHandle;

    if (m_controlChannelSocket == nullptr) {
        qWarning() << "m_controlChannelSocket == nullptr";
        return;
    }

    m_controlChannelSocket->close();
    m_controlChannelSocket->deleteLater();

    if ( m_dataChannel->m_dataChannelSocket == nullptr) {
        qWarning() << " m_dataChannel->m_dataChannelSocket == nullptr";
        return;
    }

    m_dataChannel->m_dataChannelSocket->close();
    m_dataChannel->m_dataChannelSocket->deleteLater();
    m_dataChannel->deleteLater();

    emit finishedConnection(m_socketHandle);

}

void CogWheelConnection::uploadFinished()
{
    sendReplyCode(226);
}

void CogWheelConnection::dataChannelSocketError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "CogWheelConnection::dataChannelSocketError: " << socketError;
}

void CogWheelConnection::passiveConnection()
{
    QString passiveChannelAddress = m_dataChannel->clientHostIP().toString().replace(".",",");

    passiveChannelAddress.append(","+QString::number(m_dataChannel->clientHostPort()>>8));
    passiveChannelAddress.append(","+QString::number(m_dataChannel->clientHostPort()&0xFF));
    sendReplyCode(227,"Entering Passive Mode (" + passiveChannelAddress + ").");

}

void CogWheelConnection::sendOnControlChannel(const QString &data) {
    QByteArray reply { data.toUtf8() };
    m_controlChannelSocket->write(reply.data());
}

void CogWheelConnection::sendReplyCode(quint16 replyCode,const QString &message)
{

    QString replyStr { QString::number(replyCode) + " " + message + "\r\n"};
    QByteArray reply { replyStr.toUtf8() };
    m_controlChannelSocket->write(reply.data());

}

void CogWheelConnection::sendReplyCode(quint16 replyCode)
{
    sendReplyCode(replyCode, CogWheelFTPCore::getResponseText(replyCode));
}

void CogWheelConnection::sendOnDataChannel(const QString &data)
{

  QByteArray reply { data.toUtf8() };
  m_dataChannel->m_dataChannelSocket->write(reply.data());

}

void CogWheelConnection::connected()
{
    qDebug() << "CogWheelConnection connected...";
}

void CogWheelConnection::disconnected()
{
    qDebug() << "CogWheelConnection disconnected.";

    closeConnection();

}

void CogWheelConnection::readyRead()
{

    m_readBufer.append(m_controlChannelSocket->readAll());

    if (m_readBufer.endsWith('\n')) {
        processFTPCommand(m_readBufer);
        m_readBufer.clear();
    }

}

void CogWheelConnection::bytesWritten(qint64 numberOfBytes)
{

}

qintptr CogWheelConnection::socketHandle() const
{
    return m_socketHandle;
}

void CogWheelConnection::setSocketHandle(const qintptr &socketHandle)
{
    m_socketHandle = socketHandle;
}

QTcpSocket *CogWheelConnection::controlChannelSocket() const
{
    return m_controlChannelSocket;
}

void CogWheelConnection::setControlChannelSocket(QTcpSocket *controlChannelSocket)
{
    m_controlChannelSocket = controlChannelSocket;
}

QThread *CogWheelConnection::connectionThread() const
{
    return m_connectionThread;
}

void CogWheelConnection::setConnectionThread(QThread *connectionThread)
{
    m_connectionThread = connectionThread;
}

CogWheelDataChannel *CogWheelConnection::dataChannel() const
{
    return m_dataChannel;
}

void CogWheelConnection::setDataChannel(CogWheelDataChannel *dataChannel)
{
    m_dataChannel = dataChannel;
}

bool CogWheelConnection::anonymous() const
{
    return m_anonymous;
}

void CogWheelConnection::setAnonymous(bool anonymous)
{
    m_anonymous = anonymous;
}

bool CogWheelConnection::authorized() const
{
    return m_authorized;
}

void CogWheelConnection::setAuthorized(bool authorized)
{
    m_authorized = authorized;
}

bool CogWheelConnection::passive() const
{
    return m_passive;
}

void CogWheelConnection::setPassive(bool passive)
{
    m_passive = passive;
}

QString CogWheelConnection::currentWorkingDirectory() const
{
    return m_currentWorkingDirectory;
}

void CogWheelConnection::setCurrentWorkingDirectory(const QString &currentWorkingDirectory)
{
    m_currentWorkingDirectory = currentWorkingDirectory;
    if(!m_currentWorkingDirectory.endsWith('/')); {
        m_currentWorkingDirectory.append('/');
    }

}

QString CogWheelConnection::userName() const
{
    return m_userName;
}

void CogWheelConnection::setUserName(const QString &user)
{
    m_userName = user;
}

QString CogWheelConnection::password() const
{
    return m_password;
}

void CogWheelConnection::setPassword(const QString &password)
{
    m_password = password;
}

