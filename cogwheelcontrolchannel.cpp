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
// Description: Class to provide FTP server control channel functionality
// for the receiving of commands to be processed and the sending of any reponses.
// It passes any commands to the FTP core object to be processed and also
// creates/destroys the data channel as and when it is needed.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelcontrolchannel.h"
#include "cogwheelftpcore.h"

/**
 * @brief CogWheelControlChannel::CogWheelControlChannel
 *
 * Initialise control channel.
 *
 * @param severSettings     Server Settings
 * @param parent            Object parent (should be null).
 */
CogWheelControlChannel::CogWheelControlChannel(CogWheelServerSettings serverSettings, QObject *parent) : QObject(parent)
{

    // Setup any control channel server settings

    setServerName(serverSettings.serverName());
    setServerVersion(serverSettings.serverVersion());
    setAllowSMNT(serverSettings.serverAllowSMNT());
    setWriteBytesSize(serverSettings.serverWriteBytesSize());
    setServerPrivateKey(serverSettings.serverPrivateKey());
    setServerCert(serverSettings.serverCert());

}

/**
 * @brief CogWheelControlChannel::createDataChannel
 *
 * Create data channel.
 *
 */
void CogWheelControlChannel::createDataChannel()
{
    // Channel already exists

    if ( m_dataChannel != nullptr) {
        error("Data channel already exists.");;
        return;
    }

    // Create data channel

    m_dataChannel = new CogWheelDataChannel();

    if (m_dataChannel == nullptr) {
        error("Failure to create data channel.");
        return;
    }

    // Setup signals and slots for channel

    connect(m_dataChannel,&CogWheelDataChannel::transferFinished, this,&CogWheelControlChannel::transferFinished, Qt::DirectConnection);
    connect(m_dataChannel, &CogWheelDataChannel::passiveConnection, this, &CogWheelControlChannel::passiveConnection, Qt::DirectConnection);

    connect(m_dataChannel, &CogWheelDataChannel::error, this, &CogWheelControlChannel::error, Qt::DirectConnection);
    connect(m_dataChannel, &CogWheelDataChannel::info, this, &CogWheelControlChannel::info, Qt::DirectConnection);
    connect(m_dataChannel, &CogWheelDataChannel::warning, this, &CogWheelControlChannel::warning, Qt::DirectConnection);

}

/**
 * @brief CogWheelControlChannel::tearDownDataChannel
 *
 * Destroy data channel and its associated resources.
 *
 */
void CogWheelControlChannel::tearDownDataChannel()
{

    if ( m_dataChannel == nullptr) {
        error("Failure to destroy data channel as it does not exist.");
        return;
    }

    if ( m_dataChannel->dataChannelSocket() == nullptr) {
        error("Data channel socket does not exist.");
        m_dataChannel->deleteLater();
        m_dataChannel = nullptr;
        return;
    }

    m_dataChannel->dataChannelSocket()->close();
    m_dataChannel->dataChannelSocket()->deleteLater();

    m_dataChannel->deleteLater();
    m_dataChannel = nullptr;

}

/**
 * @brief CogWheelControlChannel::connectDataChannel
 *
 * Connect up data channel to client.
 *
 * @return  true if sucessfully connected.
 */
bool CogWheelControlChannel::connectDataChannel()
{

    // Create data channel

    createDataChannel();

    if (m_dataChannel == nullptr) {
        error("Data channel not active.");
        return(false);
    }

    return(m_dataChannel->connectToClient(this));

}

/**
 * @brief CogWheelControlChannel::uploadFileToDataChannel
 *
 * Upload file to server over data channel.
 *
 * @param file      File name to upload.
 */
void CogWheelControlChannel::uploadFileToDataChannel(const QString &file)
{
    if (m_dataChannel == nullptr) {
        error("Data channel not active.");
        return;
    }

    m_dataChannel->uploadFile(this, file);

}

/**
 * @brief CogWheelControlChannel::disconnectDataChannel
 *
 * Disconnect data channel.
 *
 */
void CogWheelControlChannel::disconnectDataChannel()
{

    if (m_dataChannel == nullptr) {
        error("Data channel not active.");
        return;
    }

    m_dataChannel->disconnectFromClient(this);

    // Destroy data channel when not in use.

    tearDownDataChannel();

}

/**
 * @brief CogWheelControlChannel::setHostPortForDataChannel
 *
 * Set Host IP Address and port from PORT command
 *
 * @param ipAddressAndPort  IP Address and port as list of 6 strings.
 */
void CogWheelControlChannel::setHostPortForDataChannel(const QStringList &ipAddressAndPort)
{

    // Create data channel

    createDataChannel();

    if (m_dataChannel == nullptr) {
        error("Data channel not active.");
        return;
    }

    m_dataChannel->setClientHostIP(ipAddressAndPort[0]+"."+ipAddressAndPort[1]+"."+ipAddressAndPort[2]+"."+ipAddressAndPort[3]);
    m_dataChannel->setClientHostPort((ipAddressAndPort[4].toInt()<<8)|ipAddressAndPort[5].toInt());

}

/**
 * @brief CogWheelControlChannel::downloadFileFromDataChannel
 *
 * Download file from server over data channel.
 *
 * @param file  File name to download.
 */
void CogWheelControlChannel::downloadFileFromDataChannel(const QString &file)
{
    if (m_dataChannel == nullptr) {
        error("Data channel not active.");
        return;
    }

    m_dataChannel->downloadFile(this, file);

}

/**
 * @brief CogWheelControlChannel::listenForConnectionOnDataChannel
 *
 * Listen for a connection from client on data channel.
 * i.e. Passive Mode.
 *
 */
void CogWheelControlChannel::listenForConnectionOnDataChannel()
{

    // Create data channel

    createDataChannel();

    if (m_dataChannel == nullptr) {
        error("Data channel not active.");
        return;
    }

    m_dataChannel->listenForConnection(m_serverIP);

}

/**
 * @brief CogWheelControlChannel::abortOnDataChannel
 *
 * Abort any transfer on data channel and disconnect from client.
 *
 */
void CogWheelControlChannel::abortOnDataChannel()
{

    if(m_dataChannel != nullptr) {
        if(m_dataChannel->isConnected() || m_dataChannel->isListening()){
            disconnectDataChannel();
        }
    }

}

/**
 * @brief CogWheelControlChannel::processFTPCommand
 *
 * Send FTP command and arguments to FTP Core.
 *
 * @param commandLine   FTP command line string.
 */
void CogWheelControlChannel::processFTPCommand(QString commandLine)
{
    QString command;
    QString arguments;

    // Remove end of line

    commandLine.chop(2);

    // Separate command and arguments

    if (commandLine.contains(' ')) {
        command = commandLine.mid(0, commandLine.indexOf(' '));
        commandLine.remove(0, commandLine.indexOf(' ')+1);
        arguments = commandLine;
    } else {
        command = commandLine;
    }

    // Perform command (converting to uppercase)

    CogWheelFTPCore::performCommand(this, command.toUpper(), arguments);

}

/**
 * @brief CogWheelControlChannel::openConnection
 *
 * Open up data channel connection.
 *
 * @param socketHandle  Handle of data channel socket.
 */
void CogWheelControlChannel::openConnection(qint64 socketHandle)
{

    m_socketHandle = socketHandle; // Inialise here as used logging

    info("Open control channel for socket "+QString::number(socketHandle));

    // Create control channel socket

    m_controlChannelSocket = new QSslSocket();

    if (m_controlChannelSocket == nullptr) {
        error("Failure to create control channel socket.");
        return;
    }

    // Initialise socket from socket handle

    if (!m_controlChannelSocket->setSocketDescriptor(m_socketHandle)) {
        error("Error setting up socket for control channel.");
        m_controlChannelSocket->deleteLater();
        return;
    }

    // Get client and server IP address.

    QHostAddress host = m_controlChannelSocket->peerAddress();
    m_clientHostIP = host.toString();
    m_clientHostIP.remove(0,QString("::ffff:").length());

    host = m_controlChannelSocket->localAddress();
    m_serverIP = host.toString();
    m_serverIP.remove(0,QString("::ffff:").length());

    info("Opened control channel from "+m_clientHostIP);
    info("Opened control channel to "+m_serverIP);

    // Setup control channel signals/slots.

    connect(m_controlChannelSocket, &QSslSocket::connected, this, &CogWheelControlChannel::connected, Qt::DirectConnection);
    connect(m_controlChannelSocket, &QSslSocket::disconnected, this, &CogWheelControlChannel::disconnected, Qt::DirectConnection);
    connect(m_controlChannelSocket, &QSslSocket::readyRead, this, &CogWheelControlChannel::readyRead, Qt::DirectConnection);
    connect(m_controlChannelSocket, &QSslSocket::bytesWritten, this, &CogWheelControlChannel::bytesWritten, Qt::DirectConnection);

    // Set connected and return success.

    setConnected(true);

    sendReplyCode(200);

}

/**
 * @brief CogWheelControlChannel::closeConnection
 *
 * Close control channel.
 *
 */
void CogWheelControlChannel::closeConnection()
{

    info ("Closing control on socket : "+ QString::number(m_socketHandle));

    if (!isConnected() || m_controlChannelSocket == nullptr) {
        error("Control Channel already disconnected.");
        return;
    }

    // Close control channel socket and flag for deletion

    m_controlChannelSocket->close();
    m_controlChannelSocket->deleteLater();
    m_controlChannelSocket = nullptr;

    // Set disconnected and signal connection manager

    setConnected(false);

    emit finishedConnection(m_socketHandle);

}

/**
 * @brief CogWheelControlChannel::transferFinished
 *
 * File transfer finished so send response to client.
 *
 */
void CogWheelControlChannel::transferFinished()
{
    disconnectDataChannel();
    sendReplyCode(226);
}

/**
 * @brief CogWheelControlChannel::error
 *
 * Produce error trace message.
 *
 * @param message   Message string.
 */
void CogWheelControlChannel::error(const QString &message)
{
    qDebug() << QString("CONT[%1]E: %2").arg(QString::number(m_socketHandle), message).toStdString().c_str();
}

/**
 * @brief CogWheelControlChannel::info
 *
 * Produce informational trace message.
 *
 * @param message   Message string.
 */
void CogWheelControlChannel::info(const QString &message)
{
    qDebug() << QString("CONT[%1]I: %2").arg(QString::number(m_socketHandle), message).toStdString().c_str();
}

/**
 * @brief CogWheelControlChannel::warning
 *
 * Produce warning trace message.
 *
 * @param message   Message string.
 */
void CogWheelControlChannel::warning(const QString &message)
{
    qDebug() << QString("CONT[%1]W: %2").arg(QString::number(m_socketHandle), message).toStdString().c_str();
}

/**
 * @brief CogWheelControlChannel::passiveConnection
 *
 * Send entering passive mode reponse to client.
 *
 */
void CogWheelControlChannel::passiveConnection()
{

    QString passiveChannelAddress = m_dataChannel->clientHostIP().toString().replace(".",",");

    passiveChannelAddress.append(","+QString::number(m_dataChannel->clientHostPort()>>8));
    passiveChannelAddress.append(","+QString::number(m_dataChannel->clientHostPort()&0xFF));
    sendReplyCode(227,"Entering Passive Mode (" + passiveChannelAddress + ").");

    info("Entering Passive Mode (" + passiveChannelAddress + ").");

}

/**
 * @brief CogWheelControlChannel::sendOnControlChannel
 *
 * Send data string on control channel to client.
 *
 * @param dataToSend    Data to send over control channel.
 */
void CogWheelControlChannel::sendOnControlChannel(const QString &dataToSend) {

    // Convert QString to bytes

    m_controlChannelSocket->write(dataToSend.toUtf8().data());
}

/**
 * @brief CogWheelControlChannel::enbleTLSSupport
 *
 * Enable TLS support for on SSL socket as an AUTH TLS command has been sent.
 *
 */
void CogWheelControlChannel::enbleTLSSupport()
{

    // Use ony secure protocols

    m_controlChannelSocket->setProtocol(QSsl::SecureProtocols);

    // Create QSsl private key and cert

    QSslKey sslPrivateKey(m_serverPrivateKey, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);
    QSslCertificate sslCert(m_serverCert);

    m_controlChannelSocket->addCaCertificate(sslCert);
    m_controlChannelSocket->setLocalCertificate(sslCert);
    m_controlChannelSocket->setPrivateKey(sslPrivateKey);

    // Hookup error and channel encypted signals

    connect(m_controlChannelSocket, static_cast<void(QSslSocket::*)(const QList<QSslError> &)>(&QSslSocket::sslErrors), this, &CogWheelControlChannel::sslError);
    connect(m_controlChannelSocket, &QSslSocket::encrypted,this, &CogWheelControlChannel::controlChannelEncrypted);

    // Set keep alive and start TLS negotation

    m_controlChannelSocket->setSocketOption(QAbstractSocket::KeepAliveOption, true );
    m_controlChannelSocket->startServerEncryption();

}

/**
 * @brief CogWheelControlChannel::sslError
 * @param errors
 */
void CogWheelControlChannel::sslError(QList<QSslError> errors)
{

    QString errorStr="";

    foreach (const QSslError &e, errors) {
        errorStr.append(e.errorString()).append("\n");
    }

    emit error(errorStr);

    m_controlChannelSocket->ignoreSslErrors();

}

/**
 * @brief CogWheelControlChannel::controlChannelEncrypted
 */
void CogWheelControlChannel::controlChannelEncrypted()
{

    emit info ("Control Channel now encrypted.");

    m_sslConnection=true;

}

QByteArray CogWheelControlChannel::serverCert() const
{
    return m_serverCert;
}

void CogWheelControlChannel::setServerCert(const QByteArray &serverCert)
{
    m_serverCert = serverCert;
}

QByteArray CogWheelControlChannel::serverPrivateKey() const
{
    return m_serverPrivateKey;
}

void CogWheelControlChannel::setServerPrivateKey(const QByteArray &serverPrivateKey)
{
    m_serverPrivateKey = serverPrivateKey;
}

QChar CogWheelControlChannel::dataChanelProtection() const
{
    return m_dataChanelProtection;
}

void CogWheelControlChannel::setDataChanelProtection(const QChar &dataChanelProtection)
{
    m_dataChanelProtection = dataChanelProtection;
}

bool CogWheelControlChannel::IsSslConnection() const
{
    return m_sslConnection;
}

void CogWheelControlChannel::setSslConnection(bool sslConnection)
{
    m_sslConnection = sslConnection;
}

/**
 * @brief CogWheelControlChannel::sendReplyCode
 *
 * Send reply code and message to client over control channel.
 *
 * @param replyCode     Numeric reply code.
 * @param message       Message.
 */
void CogWheelControlChannel::sendReplyCode(quint16 replyCode,const QString &message)
{

    // Convert reply code to string, append message and send.

    QString reply { QString::number(replyCode) + " " + message + "\r\n"};

    m_controlChannelSocket->write(reply.toUtf8().data());

}

/**
 * @brief CogWheelControlChannel::sendReplyCode
 *
 * Send reply code and its message to client over control channel.
 *
 * @param replyCode     Numeric reply code.
 */
void CogWheelControlChannel::sendReplyCode(quint16 replyCode)
{

    sendReplyCode(replyCode, CogWheelFTPCore::getResponseText(replyCode));
}

/**
 * @brief CogWheelControlChannel::sendOnDataChannel
 *
 * Send data over data channel.
 *
 * @param dataToSend    Data to send (bytes).
 */
void CogWheelControlChannel::sendOnDataChannel(const QByteArray &dataToSend)
{

    m_dataChannel->dataChannelSocket()->write(dataToSend);

}

/**
 * @brief CogWheelControlChannel::connected
 *
 * Control channel connected slot function.
 *
 */
void CogWheelControlChannel::connected()
{
    info("Control channel connected...");
}

/**
 * @brief CogWheelControlChannel::disconnected
 *
 * Control channel disconnected slot function.
 */
void CogWheelControlChannel::disconnected()
{
    info("Control channel disconnected.");

    // Close control channel.

    closeConnection();

}

/**
 * @brief CogWheelControlChannel::readyRead
 *
 * Control channel read slot function.
 *
 */
void CogWheelControlChannel::readyRead()
{

    // Read all available data and append to read buffer
    // When end of line found execute command and clear buffer.

    m_readBufer.append(m_controlChannelSocket->readAll());

    if (m_readBufer.endsWith('\n')) {
        processFTPCommand(m_readBufer);
        m_readBufer.clear();
    }

}

/**
 * @brief CogWheelControlChannel::bytesWritten
 * @param numberOfBytes
 */
void CogWheelControlChannel::bytesWritten(qint64 numberOfBytes)
{

    Q_UNUSED(numberOfBytes);

}

// ============================
// CLASS PRIVATE DATA ACCESSORS
// ============================

/**
 * @brief CogWheelControlChannel::writeAccess
 * @return
 */
bool CogWheelControlChannel::writeAccess() const
{
    return m_writeAccess;
}

/**
 * @brief CogWheelControlChannel::setWriteAccess
 * @param writeAccess
 */
void CogWheelControlChannel::setWriteAccess(bool writeAccess)
{
    m_writeAccess = writeAccess;
}

/**
 * @brief CogWheelControlChannel::writeBytesSize
 * @return
 */
qint64 CogWheelControlChannel::writeBytesSize() const
{
    return m_writeBytesSize;
}

/**
 * @brief CogWheelControlChannel::setWriteBytesSize
 * @param writeBytesSize
 */
void CogWheelControlChannel::setWriteBytesSize(const qint64 &writeBytesSize)
{
    m_writeBytesSize = writeBytesSize;
}

/**
 * @brief CogWheelControlChannel::serverName
 * @return
 */
QString CogWheelControlChannel::serverName() const
{
    return m_serverName;
}

/**
 * @brief CogWheelControlChannel::setServerName
 * @param serverName
 */
void CogWheelControlChannel::setServerName(const QString &serverName)
{
    m_serverName = serverName;
}

/**
 * @brief CogWheelControlChannel::serverVersion
 * @return
 */
QString CogWheelControlChannel::serverVersion() const
{
    return m_serverVersion;
}

/**
 * @brief CogWheelControlChannel::setServerVersion
 * @param serverVersion
 */
void CogWheelControlChannel::setServerVersion(const QString &serverVersion)
{
    m_serverVersion = serverVersion;
}

/**
 * @brief CogWheelControlChannel::transTypeByteSize
 * @return
 */
qint16 CogWheelControlChannel::transTypeByteSize() const
{
    return m_transTypeByteSize;
}

/**
 * @brief CogWheelControlChannel::setTransTypeByteSize
 * @param transTypeByteSize
 */
void CogWheelControlChannel::setTransTypeByteSize(const qint16 &transTypeByteSize)
{
    m_transTypeByteSize = transTypeByteSize;
}

/**
 * @brief CogWheelControlChannel::transferTypeFormat
 * @return
 */
QChar CogWheelControlChannel::transferTypeFormat() const
{
    return m_transferTypeFormat;
}

/**
 * @brief CogWheelControlChannel::setTransferTypeFormat
 * @param transferTypeFormat
 */
void CogWheelControlChannel::setTransferTypeFormat(const QChar &transferTypeFormat)
{
    m_transferTypeFormat = transferTypeFormat;
}

/**
 * @brief CogWheelControlChannel::dataChannel
 * @return
 */
CogWheelDataChannel *CogWheelControlChannel::dataChannel() const
{
    return m_dataChannel;
}

/**
 * @brief CogWheelControlChannel::setDataChannel
 * @param dataChannel
 */
void CogWheelControlChannel::setDataChannel(CogWheelDataChannel *dataChannel)
{
    m_dataChannel = dataChannel;
}

/**
 * @brief CogWheelControlChannel::transferType
 * @return
 */
QChar CogWheelControlChannel::transferType() const
{
    return m_transferType;
}

/**
 * @brief CogWheelControlChannel::setTransferType
 * @param transferType
 */
void CogWheelControlChannel::setTransferType(const QChar &transferType)
{
    m_transferType = transferType;
}

/**
 * @brief CogWheelControlChannel::fileStructure
 * @return
 */
QChar CogWheelControlChannel::fileStructure() const
{
    return m_fileStructure;
}

/**
 * @brief CogWheelControlChannel::setFileStructure
 * @param fileStructure
 */
void CogWheelControlChannel::setFileStructure(const QChar &fileStructure)
{
    m_fileStructure = fileStructure;
}

/**
 * @brief CogWheelControlChannel::transferMode
 * @return
 */
QChar CogWheelControlChannel::transferMode() const
{
    return m_transferMode;
}

/**
 * @brief CogWheelControlChannel::setTransferMode
 * @param tranfersMode
 */
void CogWheelControlChannel::setTransferMode(const QChar &tranfersMode)
{
    m_transferMode = tranfersMode;
}

/**
 * @brief CogWheelControlChannel::isConnected
 * @return
 */
bool CogWheelControlChannel::isConnected() const
{
    return m_connected;
}

/**
 * @brief CogWheelControlChannel::setConnected
 * @param connected
 */
void CogWheelControlChannel::setConnected(bool connected)
{
    m_connected = connected;
}

/**
 * @brief CogWheelControlChannel::serverIP
 * @return
 */
QString CogWheelControlChannel::serverIP() const
{
    return m_serverIP;
}

/**
 * @brief CogWheelControlChannel::setServerIP
 * @param serverIP
 */
void CogWheelControlChannel::setServerIP(const QString &serverIP)
{
    m_serverIP = serverIP;
}

/**
 * @brief CogWheelControlChannel::clientHostIP
 * @return
 */
QString CogWheelControlChannel::clientHostIP() const
{
    return m_clientHostIP;
}

/**
 * @brief CogWheelControlChannel::setClientHostIP
 * @param clientHostIP
 */
void CogWheelControlChannel::setClientHostIP(const QString &clientHostIP)
{
    m_clientHostIP = clientHostIP;
}

/**
 * @brief CogWheelControlChannel::restoreFilePostion
 * @return
 */
qint64 CogWheelControlChannel::restoreFilePostion() const
{
    return m_restoreFilePostion;
}

/**
 * @brief CogWheelControlChannel::setRestoreFilePostion
 * @param restoreFilePostion
 */
void CogWheelControlChannel::setRestoreFilePostion(const qint64 &restoreFilePostion)
{
    m_restoreFilePostion = restoreFilePostion;
}

/**
 * @brief CogWheelControlChannel::renameFromFileName
 * @return
 */
QString CogWheelControlChannel::renameFromFileName() const
{
    return m_renameFromFileName;
}

/**
 * @brief CogWheelControlChannel::setRenameFromFileName
 * @param renameFromFileName
 */
void CogWheelControlChannel::setRenameFromFileName(const QString &renameFromFileName)
{
    m_renameFromFileName = renameFromFileName;
}

/**
 * @brief CogWheelControlChannel::isAllowSMNT
 * @return
 */
bool CogWheelControlChannel::isAllowSMNT() const
{
    return m_allowSMNT;
}

/**
 * @brief CogWheelControlChannel::setAllowSMNT
 * @param allowSMT
 */
void CogWheelControlChannel::setAllowSMNT(bool allowSMT)
{
    m_allowSMNT = allowSMT;
}

/**
 * @brief CogWheelControlChannel::accountName
 * @return
 */
QString CogWheelControlChannel::accountName() const
{
    return m_accountName;
}

/**
 * @brief CogWheelControlChannel::setAccountName
 * @param accountName
 */
void CogWheelControlChannel::setAccountName(const QString &accountName)
{
    m_accountName = accountName;
}

/**
 * @brief CogWheelControlChannel::rootDirectory
 * @return
 */
QString CogWheelControlChannel::rootDirectory() const
{
    return m_rootDirectory;
}

/**
 * @brief CogWheelControlChannel::setRootDirectory
 * @param rootDirectory
 */
void CogWheelControlChannel::setRootDirectory(const QString &rootDirectory)
{
    m_rootDirectory = rootDirectory;
    if (m_rootDirectory.endsWith('\'')) {
        m_rootDirectory.chop(1);
    }
}

/**
 * @brief CogWheelControlChannel::socketHandle
 * @return
 */
qintptr CogWheelControlChannel::socketHandle() const
{
    return m_socketHandle;
}

/**
 * @brief CogWheelControlChannel::setSocketHandle
 * @param socketHandle
 */
void CogWheelControlChannel::setSocketHandle(const qintptr &socketHandle)
{
    m_socketHandle = socketHandle;
}

/**
 * @brief CogWheelControlChannel::controlChannelSocket
 * @return
 */
QSslSocket *CogWheelControlChannel::controlChannelSocket() const
{
    return m_controlChannelSocket;
}

/**
 * @brief CogWheelControlChannel::setControlChannelSocket
 * @param controlChannelSocket
 */
void CogWheelControlChannel::setControlChannelSocket(QSslSocket *controlChannelSocket)
{
    m_controlChannelSocket = controlChannelSocket;
}

/**
 * @brief CogWheelControlChannel::connectionThread
 * @return
 */
QThread *CogWheelControlChannel::connectionThread() const
{
    return m_connectionThread;
}

/**
 * @brief CogWheelControlChannel::setConnectionThread
 * @param connectionThread
 */
void CogWheelControlChannel::setConnectionThread(QThread *connectionThread)
{
    m_connectionThread = connectionThread;
}

/**
 * @brief CogWheelControlChannel::isAnonymous
 * @return
 */
bool CogWheelControlChannel::isAnonymous() const
{
    return m_anonymous;
}

/**
 * @brief CogWheelControlChannel::setAnonymous
 * @param anonymous
 */
void CogWheelControlChannel::setAnonymous(bool anonymous)
{
    m_anonymous = anonymous;
}

/**
 * @brief CogWheelControlChannel::isAuthorized
 * @return
 */
bool CogWheelControlChannel::isAuthorized() const
{
    return m_authorized;
}

/**
 * @brief CogWheelControlChannel::setAuthorized
 * @param authorized
 */
void CogWheelControlChannel::setAuthorized(bool authorized)
{
    m_authorized = authorized;
}

/**
 * @brief CogWheelControlChannel::isPassive
 * @return
 */
bool CogWheelControlChannel::isPassive() const
{
    return m_passive;
}

/**
 * @brief CogWheelControlChannel::setPassive
 * @param passive
 */
void CogWheelControlChannel::setPassive(bool passive)
{
    m_passive = passive;
}

/**
 * @brief CogWheelControlChannel::currentWorkingDirectory
 * @return
 */
QString CogWheelControlChannel::currentWorkingDirectory() const
{
    return m_currentWorkingDirectory;
}

/**
 * @brief CogWheelControlChannel::setCurrentWorkingDirectory
 * @param currentWorkingDirectory
 */
void CogWheelControlChannel::setCurrentWorkingDirectory(const QString &currentWorkingDirectory)
{
    m_currentWorkingDirectory = currentWorkingDirectory;

    if(m_currentWorkingDirectory.isEmpty()) {
        m_currentWorkingDirectory.append('/');
    }

}

/**
 * @brief CogWheelControlChannel::userName
 * @return
 */
QString CogWheelControlChannel::userName() const
{
    return m_userName;
}

/**
 * @brief CogWheelControlChannel::setUserName
 * @param user
 */
void CogWheelControlChannel::setUserName(const QString &user)
{
    m_userName = user;
}

/**
 * @brief CogWheelControlChannel::password
 * @return
 */
QString CogWheelControlChannel::password() const
{
    return m_password;
}

/**
 * @brief CogWheelControlChannel::setPassword
 * @param password
 */
void CogWheelControlChannel::setPassword(const QString &password)
{
    m_password = password;
}
