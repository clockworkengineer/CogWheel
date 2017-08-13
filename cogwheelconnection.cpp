#include "cogwheelconnection.h"
#include"cogwheelftpcore.h"

#include <QCoreApplication>

CogWheelConnection::CogWheelConnection(QObject *parent) : QObject(parent)
{

}

void CogWheelConnection::processFTPCommand(QString commandLine)
{
    QString command;
    QString arguments;

    commandLine.chop(2);

    if (commandLine.contains(' ')) {
        command = commandLine.mid(0, commandLine.indexOf(' '));
        commandLine.remove(0, commandLine.indexOf(' ')+1);
        arguments = commandLine;
    } else {
        command = commandLine;
    }

    CogWheelFTPCore::performCommand(this, command.toUpper(), arguments);

}

void CogWheelConnection::openConnection(qint64 socketHandle)
{

    qDebug() << "CogWheelConnection::openConnection: on thread " << QThread::currentThreadId();

    m_dataChannel = new CogWheelDataChannel();

    m_controlChannelSocket = new QTcpSocket();

    m_socketHandle = socketHandle;
    if (!m_controlChannelSocket->setSocketDescriptor(m_socketHandle)) {
        qWarning () << "Error setting up socket for control channel.";
        return;
    }

    QHostAddress host = m_controlChannelSocket->peerAddress();
    m_clientHostIP = host.toString();
    m_clientHostIP.remove(0,QString("::ffff:").length());

    host = m_controlChannelSocket->localAddress();
    m_serverIP = host.toString();
    m_serverIP.remove(0,QString("::ffff:").length());

    qDebug() << "Opened control channel from " << m_clientHostIP;
    qDebug() << "Opened control channel to " << m_serverIP;

    connect(m_controlChannelSocket, &QTcpSocket::connected, this, &CogWheelConnection::controlChannelConnected, Qt::DirectConnection);
    connect(m_controlChannelSocket, &QTcpSocket::disconnected, this, &CogWheelConnection::controlChannelDisconnected, Qt::DirectConnection);
    connect(m_controlChannelSocket, &QTcpSocket::readyRead, this, &CogWheelConnection::controlChannelReadyRead, Qt::DirectConnection);
    connect(m_controlChannelSocket, &QTcpSocket::bytesWritten, this, &CogWheelConnection::controlChannelBytesWritten, Qt::DirectConnection);

    connect(m_dataChannel,&CogWheelDataChannel::dataChannelUploadFinished, this,&CogWheelConnection::uploadFinished, Qt::DirectConnection);
    connect(m_dataChannel, &CogWheelDataChannel::dataChannelError, this, &CogWheelConnection::dataChannelError, Qt::DirectConnection);
    connect(m_dataChannel, &CogWheelDataChannel::dataChannelPassiveConnection, this, &CogWheelConnection::passiveConnection, Qt::DirectConnection);

    setConnected(true);

    sendReplyCode(200);

}

void CogWheelConnection::closeConnection()
{
    qDebug() << "CogWheelConnection::close(): " << m_socketHandle;

    if (!connected()) {
        qDebug() << "Control Channel already disconnected.";
        return;
    }

    if (m_controlChannelSocket == nullptr) {
        qWarning() << "m_controlChannelSocket == nullptr";
        return;
    }

    m_controlChannelSocket->close();
    m_controlChannelSocket->deleteLater();
    m_controlChannelSocket = nullptr;

    if ( m_dataChannel->m_dataChannelSocket == nullptr) {
        qWarning() << " m_dataChannel->m_dataChannelSocket == nullptr";
        return;
    }

    m_dataChannel->m_dataChannelSocket->close();
    m_dataChannel->m_dataChannelSocket->deleteLater();
    m_dataChannel->deleteLater();
    m_dataChannel = nullptr;

    setConnected(false);

    emit finishedConnection(m_socketHandle);

}

void CogWheelConnection::uploadFinished()
{
    sendReplyCode(226);
}

void CogWheelConnection::dataChannelError(QString errorNessage)
{
    qDebug() << "CogWheelConnection::dataChannelError: " << errorNessage;
}

void CogWheelConnection::passiveConnection()
{
    QString passiveChannelAddress = m_dataChannel->clientHostIP().toString().replace(".",",");

    passiveChannelAddress.append(","+QString::number(m_dataChannel->clientHostPort()>>8));
    passiveChannelAddress.append(","+QString::number(m_dataChannel->clientHostPort()&0xFF));
    sendReplyCode(227,"Entering Passive Mode (" + passiveChannelAddress + ").");
    qDebug() << "Entering Passive Mode (" + passiveChannelAddress + ").";

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

void CogWheelConnection::controlChannelConnected()
{
    qDebug() << "CogWheelConnection connected...";
}

void CogWheelConnection::controlChannelDisconnected()
{
    qDebug() << "CogWheelConnection disconnected.";

    closeConnection();

}

void CogWheelConnection::controlChannelReadyRead()
{

    m_readBufer.append(m_controlChannelSocket->readAll());

    if (m_readBufer.endsWith('\n')) {
        processFTPCommand(m_readBufer);
        m_readBufer.clear();
    }

}

void CogWheelConnection::controlChannelBytesWritten(qint64 numberOfBytes)
{

}

QChar CogWheelConnection::transferType() const
{
    return m_transferType;
}

void CogWheelConnection::setTransferType(const QChar &transferType)
{
    m_transferType = transferType;
}

QChar CogWheelConnection::fileStructure() const
{
    return m_fileStructure;
}

void CogWheelConnection::setFileStructure(const QChar &fileStructure)
{
    m_fileStructure = fileStructure;
}

QChar CogWheelConnection::transferMode() const
{
    return m_transferMode;
}

void CogWheelConnection::setTransferMode(const QChar &tranfersMode)
{
    m_transferMode = tranfersMode;
}

bool CogWheelConnection::connected() const
{
    return m_connected;
}

void CogWheelConnection::setConnected(bool connected)
{
    m_connected = connected;
}

QString CogWheelConnection::serverIP() const
{
    return m_serverIP;
}

void CogWheelConnection::setServerIP(const QString &serverIP)
{
    m_serverIP = serverIP;
}

QString CogWheelConnection::clientHostIP() const
{
    return m_clientHostIP;
}

void CogWheelConnection::setClientHostIP(const QString &clientHostIP)
{
    m_clientHostIP = clientHostIP;
}

qint64 CogWheelConnection::restoreFilePostion() const
{
    return m_restoreFilePostion;
}

void CogWheelConnection::setRestoreFilePostion(const qint64 &restoreFilePostion)
{
    m_restoreFilePostion = restoreFilePostion;
}

QString CogWheelConnection::renameFromFileName() const
{
    return m_renameFromFileName;
}

void CogWheelConnection::setRenameFromFileName(const QString &renameFromFileName)
{
    m_renameFromFileName = renameFromFileName;
}

bool CogWheelConnection::allowSMNT() const
{
    return m_allowSMNT;
}

void CogWheelConnection::setAllowSMNT(bool allowSMT)
{
    m_allowSMNT = allowSMT;
}

QString CogWheelConnection::accountName() const
{
    return m_accountName;
}

void CogWheelConnection::setAccountName(const QString &accountName)
{
    m_accountName = accountName;
}

QString CogWheelConnection::rootDirectory() const
{
    return m_rootDirectory;
}

void CogWheelConnection::setRootDirectory(const QString &rootDirectory)
{
    m_rootDirectory = rootDirectory;
    if (m_rootDirectory.endsWith('\'')) {
        m_rootDirectory.chop(1);
    }
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

    if(m_currentWorkingDirectory.isEmpty()) {
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

