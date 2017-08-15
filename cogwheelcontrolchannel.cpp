/*
 * File:   cogwheelcontrolchannel.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

//
// Class: CogWheelControlChannel
//
// Description:
//
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelcontrolchannel.h"
#include "cogwheelftpcore.h"

#include <QCoreApplication>

CogWheelControlChannel::CogWheelControlChannel(QObject *parent) : QObject(parent)
{

}

void CogWheelControlChannel::createDataChannel()
{

    if ( m_dataChannel != nullptr) {
        qWarning() << " m_dataChannel != nullptr";
        return;
    }

    m_dataChannel = new CogWheelDataChannel();

    connect(m_dataChannel,&CogWheelDataChannel::uploadFinished, this,&CogWheelControlChannel::uploadFinished, Qt::DirectConnection);
    connect(m_dataChannel, &CogWheelDataChannel::error, this, &CogWheelControlChannel::error, Qt::DirectConnection);
    connect(m_dataChannel, &CogWheelDataChannel::passiveConnection, this, &CogWheelControlChannel::passiveConnection, Qt::DirectConnection);

}

void CogWheelControlChannel::tearDownDataChannel()
{

    if ( m_dataChannel == nullptr) {
        qWarning() << " m_dataChannel == nullptr";
        return;
    }

    if ( m_dataChannel->m_dataChannelSocket == nullptr) {
        qWarning() << " m_dataChannel->m_dataChannelSocket == nullptr";
        return;
    }

    m_dataChannel->m_dataChannelSocket->close();
    m_dataChannel->m_dataChannelSocket->deleteLater();

    m_dataChannel->deleteLater();
    m_dataChannel = nullptr;

}

bool CogWheelControlChannel::connectDataChannel()
{

    createDataChannel();

    if (m_dataChannel == nullptr) {
        qDebug() << "Error: Data channel not active.";
        return(false);
    }

    return(m_dataChannel->connectToClient(this));

}

void CogWheelControlChannel::uploadFileToDataChannel(const QString &file)
{
    if (m_dataChannel == nullptr) {
        qDebug() << "Error: Data channel not active.";
        return;
    }

    m_dataChannel->uploadFile(this, file);
}

void CogWheelControlChannel::disconnectDataChannel()
{
    if (m_dataChannel == nullptr) {
        qDebug() << "Error: Data channel not active.";
        return;
    }

    m_dataChannel->disconnectFromClient(this);

    tearDownDataChannel();

}

void CogWheelControlChannel::setHostPortForDataChannel(QStringList ipAddressAndPort)
{
     createDataChannel();

    if (m_dataChannel == nullptr) {
        qDebug() << "Error: Data channel not active.";
        return;
    }

    m_dataChannel->setClientHostIP(ipAddressAndPort[0]+"."+ipAddressAndPort[1]+"."+ipAddressAndPort[2]+"."+ipAddressAndPort[3]);
    m_dataChannel->setClientHostPort((ipAddressAndPort[4].toInt()<<8)|ipAddressAndPort[5].toInt());

}

void CogWheelControlChannel::downloadFileFromDataChannel(const QString &file)
{
    if (m_dataChannel == nullptr) {
        qDebug() << "Error: Data channel not active.";
        return;
    }

    m_dataChannel->downloadFile(this, file);
}

void CogWheelControlChannel::listenForConnectionOnDataChannel()
{

    createDataChannel();

    if (m_dataChannel == nullptr) {
        qDebug() << "Error: Data channel not active.";
        return;
    }

    m_dataChannel->listenForConnection(m_serverIP);
}

void CogWheelControlChannel::abortOnDataChannel()
{

    if(m_dataChannel != nullptr) {
        if(m_dataChannel->isConnected() || m_dataChannel->isListening()){
            m_dataChannel->disconnectFromClient(this);
        }
    }

}

void CogWheelControlChannel::processFTPCommand(QString commandLine)
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

void CogWheelControlChannel::openConnection(qint64 socketHandle)
{

    qDebug() << "CogWheelConnection::openConnection: on thread " << QThread::currentThreadId();

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

    connect(m_controlChannelSocket, &QTcpSocket::connected, this, &CogWheelControlChannel::connected, Qt::DirectConnection);
    connect(m_controlChannelSocket, &QTcpSocket::disconnected, this, &CogWheelControlChannel::disconnected, Qt::DirectConnection);
    connect(m_controlChannelSocket, &QTcpSocket::readyRead, this, &CogWheelControlChannel::readyRead, Qt::DirectConnection);
    connect(m_controlChannelSocket, &QTcpSocket::bytesWritten, this, &CogWheelControlChannel::bytesWritten, Qt::DirectConnection);

   // createDataChannel();

    setConnected(true);

    sendReplyCode(200);

}

void CogWheelControlChannel::closeConnection()
{
    qDebug() << "CogWheelConnection::close(): " << m_socketHandle;

    if (!isConnected()) {
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

  //  tearDownDataChannel();

    setConnected(false);

    emit finishedConnection(m_socketHandle);

}

void CogWheelControlChannel::uploadFinished()
{
    sendReplyCode(226);
}

void CogWheelControlChannel::error(QString errorNessage)
{
    qDebug() << "CogWheelConnection::dataChannelError: " << errorNessage;
}

void CogWheelControlChannel::passiveConnection()
{

    QString passiveChannelAddress = m_dataChannel->clientHostIP().toString().replace(".",",");

    passiveChannelAddress.append(","+QString::number(m_dataChannel->clientHostPort()>>8));
    passiveChannelAddress.append(","+QString::number(m_dataChannel->clientHostPort()&0xFF));
    sendReplyCode(227,"Entering Passive Mode (" + passiveChannelAddress + ").");
    qDebug() << "Entering Passive Mode (" + passiveChannelAddress + ").";

}

void CogWheelControlChannel::sendOnControlChannel(const QString &data) {
    QByteArray reply { data.toUtf8() };
    m_controlChannelSocket->write(reply.data());
}

void CogWheelControlChannel::sendReplyCode(quint16 replyCode,const QString &message)
{

    QString replyStr { QString::number(replyCode) + " " + message + "\r\n"};
    QByteArray reply { replyStr.toUtf8() };
    m_controlChannelSocket->write(reply.data());

}

void CogWheelControlChannel::sendReplyCode(quint16 replyCode)
{
    sendReplyCode(replyCode, CogWheelFTPCore::getResponseText(replyCode));
}

void CogWheelControlChannel::sendOnDataChannel(const QString &data)
{

    QByteArray reply { data.toUtf8() };
    m_dataChannel->m_dataChannelSocket->write(reply.data());

}

void CogWheelControlChannel::connected()
{
    qDebug() << "CogWheelConnection connected...";
}

void CogWheelControlChannel::disconnected()
{
    qDebug() << "CogWheelConnection disconnected.";

    closeConnection();

}

void CogWheelControlChannel::readyRead()
{

    m_readBufer.append(m_controlChannelSocket->readAll());

    if (m_readBufer.endsWith('\n')) {
        processFTPCommand(m_readBufer);
        m_readBufer.clear();
    }

}

void CogWheelControlChannel::bytesWritten(qint64 numberOfBytes)
{

}

qint16 CogWheelControlChannel::transTypeByteSize() const
{
    return m_transTypeByteSize;
}

void CogWheelControlChannel::setTransTypeByteSize(const qint16 &transTypeByteSize)
{
    m_transTypeByteSize = transTypeByteSize;
}

QChar CogWheelControlChannel::transferTypeFormat() const
{
    return m_transferTypeFormat;
}

void CogWheelControlChannel::setTransferTypeFormat(const QChar &transferTypeFormat)
{
    m_transferTypeFormat = transferTypeFormat;
}

CogWheelDataChannel *CogWheelControlChannel::dataChannel() const
{
    return m_dataChannel;
}

void CogWheelControlChannel::setDataChannel(CogWheelDataChannel *dataChannel)
{
    m_dataChannel = dataChannel;
}

QChar CogWheelControlChannel::transferType() const
{
    return m_transferType;
}

void CogWheelControlChannel::setTransferType(const QChar &transferType)
{
    m_transferType = transferType;
}

QChar CogWheelControlChannel::fileStructure() const
{
    return m_fileStructure;
}

void CogWheelControlChannel::setFileStructure(const QChar &fileStructure)
{
    m_fileStructure = fileStructure;
}

QChar CogWheelControlChannel::transferMode() const
{
    return m_transferMode;
}

void CogWheelControlChannel::setTransferMode(const QChar &tranfersMode)
{
    m_transferMode = tranfersMode;
}

bool CogWheelControlChannel::isConnected() const
{
    return m_connected;
}

void CogWheelControlChannel::setConnected(bool connected)
{
    m_connected = connected;
}

QString CogWheelControlChannel::serverIP() const
{
    return m_serverIP;
}

void CogWheelControlChannel::setServerIP(const QString &serverIP)
{
    m_serverIP = serverIP;
}

QString CogWheelControlChannel::clientHostIP() const
{
    return m_clientHostIP;
}

void CogWheelControlChannel::setClientHostIP(const QString &clientHostIP)
{
    m_clientHostIP = clientHostIP;
}

qint64 CogWheelControlChannel::restoreFilePostion() const
{
    return m_restoreFilePostion;
}

void CogWheelControlChannel::setRestoreFilePostion(const qint64 &restoreFilePostion)
{
    m_restoreFilePostion = restoreFilePostion;
}

QString CogWheelControlChannel::renameFromFileName() const
{
    return m_renameFromFileName;
}

void CogWheelControlChannel::setRenameFromFileName(const QString &renameFromFileName)
{
    m_renameFromFileName = renameFromFileName;
}

bool CogWheelControlChannel::isAllowSMNT() const
{
    return m_allowSMNT;
}

void CogWheelControlChannel::setAllowSMNT(bool allowSMT)
{
    m_allowSMNT = allowSMT;
}

QString CogWheelControlChannel::accountName() const
{
    return m_accountName;
}

void CogWheelControlChannel::setAccountName(const QString &accountName)
{
    m_accountName = accountName;
}

QString CogWheelControlChannel::rootDirectory() const
{
    return m_rootDirectory;
}

void CogWheelControlChannel::setRootDirectory(const QString &rootDirectory)
{
    m_rootDirectory = rootDirectory;
    if (m_rootDirectory.endsWith('\'')) {
        m_rootDirectory.chop(1);
    }
}

qintptr CogWheelControlChannel::socketHandle() const
{
    return m_socketHandle;
}

void CogWheelControlChannel::setSocketHandle(const qintptr &socketHandle)
{
    m_socketHandle = socketHandle;
}

QTcpSocket *CogWheelControlChannel::controlChannelSocket() const
{
    return m_controlChannelSocket;
}

void CogWheelControlChannel::setControlChannelSocket(QTcpSocket *controlChannelSocket)
{
    m_controlChannelSocket = controlChannelSocket;
}

QThread *CogWheelControlChannel::connectionThread() const
{
    return m_connectionThread;
}

void CogWheelControlChannel::setConnectionThread(QThread *connectionThread)
{
    m_connectionThread = connectionThread;
}

bool CogWheelControlChannel::isAnonymous() const
{
    return m_anonymous;
}

void CogWheelControlChannel::setAnonymous(bool anonymous)
{
    m_anonymous = anonymous;
}

bool CogWheelControlChannel::isAuthorized() const
{
    return m_authorized;
}

void CogWheelControlChannel::setAuthorized(bool authorized)
{
    m_authorized = authorized;
}

bool CogWheelControlChannel::isPassive() const
{
    return m_passive;
}

void CogWheelControlChannel::setPassive(bool passive)
{
    m_passive = passive;
}

QString CogWheelControlChannel::currentWorkingDirectory() const
{
    return m_currentWorkingDirectory;
}

void CogWheelControlChannel::setCurrentWorkingDirectory(const QString &currentWorkingDirectory)
{
    m_currentWorkingDirectory = currentWorkingDirectory;

    if(m_currentWorkingDirectory.isEmpty()) {
        m_currentWorkingDirectory.append('/');
    }

}

QString CogWheelControlChannel::userName() const
{
    return m_userName;
}

void CogWheelControlChannel::setUserName(const QString &user)
{
    m_userName = user;
}

QString CogWheelControlChannel::password() const
{
    return m_password;
}

void CogWheelControlChannel::setPassword(const QString &password)
{
    m_password = password;
}

