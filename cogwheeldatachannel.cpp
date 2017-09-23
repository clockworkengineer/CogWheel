/*
 * File:   cogwheeldatachannel.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

//
// Class: CogWheelDataChannel
//
// Description: Class to provide FTP server data channel functionality.
// The channel to created and destroyed on an as needed basis and can operate
// in the default active mode where the server creates it or in passive mode
// where the server waits for a connection from the client on a specified port.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheeldatachannel.h"
#include "cogwheelcontrolchannel.h"
#include "cogwheelftpserverreply.h"
#include "cogwheellogger.h"

// ====================
// CLASS IMPLEMENTATION
// ====================

/**
 * @brief CogWheelDataChannel::CogWheelDataChannel
 *
 * Create data channel instance. Create a socket and
 * connect up its signals and slots.
 *
 * @param parent    parent object (not used).
 */
CogWheelDataChannel::CogWheelDataChannel(qintptr controlSocketHandle, QObject *parent) : m_controlSocketHandle(controlSocketHandle)
{
    Q_UNUSED(parent);

    cogWheelInfo(m_controlSocketHandle,"Data channel created.");

    m_dataChannelSocket = new QSslSocket();

    if (m_dataChannelSocket==nullptr) {
        throw CogWheelFtpServerReply(425, "Error trying to create data channel socket.");
    }

    connect(m_dataChannelSocket, &QSslSocket::connected, this, &CogWheelDataChannel::connected, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QSslSocket::disconnected, this, &CogWheelDataChannel::disconnected, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QSslSocket::bytesWritten, this, &CogWheelDataChannel::bytesWritten, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QSslSocket::readyRead, this, &CogWheelDataChannel::readyRead, Qt::DirectConnection);
    connect(m_dataChannelSocket, static_cast<void (QSslSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
            this, &CogWheelDataChannel::socketError, Qt::DirectConnection);

}

/**
 * @brief CogWheelDataChannel::~CogWheelDataChannel
 *
 * Data channel destructor. Release all data accessed though pointers.
 *
 */
CogWheelDataChannel::~CogWheelDataChannel()
{
    fileTransferCleanup();
    dataChannelSocketCleanup();
}

/**
 * @brief CogWheelDataChannel::connectToClient
 *
 * Connect up data channel; either from server (active)
 * or from client (passive).
 *
 * @param connection    Pointer to control channel instance.
 *
 * @return  == true on sucessful connection
 */
bool CogWheelDataChannel::connectToClient(CogWheelControlChannel *connection)
{

    if (m_connected) {
        cogWheelError(m_controlSocketHandle,"Data channel already connected.");
        return(m_connected);
    }

    if (!connection->isPassive()) {

        cogWheelInfo(m_controlSocketHandle,"Active Mode. Connecting data channel to client ....");

        m_dataChannelSocket->connectToHost(m_clientHostIP, m_clientHostPort);
        m_dataChannelSocket->waitForConnected(-1);

        connection->sendReplyCode(150);

    } else {

        cogWheelInfo(m_controlSocketHandle,"Passive Mode. Waiting to connect to data channel ....");

        if (m_dataChannelSocket->state() != QAbstractSocket::ConnectedState) {
            waitForNewConnection(-1);
        }

        connection->sendReplyCode(125);

    }


    // Data channel protection set to private so switch on SSL

    if (connection->dataChanelProtection()=='P') {
        enbleDataChannelTLSSupport(connection);
    }

    // Set write size

    m_writeBytesSize = connection->serverWriteBytesSize();

    // Re-check connected status and return error if not

    if (m_dataChannelSocket->state() != QAbstractSocket::ConnectedState) {
        throw CogWheelFtpServerReply(425, "Data channel did not connect. Socket Error: "+m_dataChannelSocket->errorString());
    }

    m_connected=true;

    return(m_connected);

}

/**
 * @brief CogWheelDataChannel::disconnectFromClient
 *
 * Disconnect data channel.
 *
 * @param connection    Pointer to control channel instance.
 */
void CogWheelDataChannel::disconnectFromClient(CogWheelControlChannel *connection)
{

    if (m_dataChannelSocket) {
        if (m_dataChannelSocket->state() == QAbstractSocket::ConnectedState) {
            m_dataChannelSocket->flush();   // Flush any buffered data
            m_dataChannelSocket->disconnectFromHost();
            if (m_dataChannelSocket->state() != QAbstractSocket::UnconnectedState) {
                m_dataChannelSocket->waitForDisconnected(-1);
            }
            connection->sendReplyCode(226); // Data channel closed reply
        }
    }
    m_connected=false;

}

/**
 * @brief CogWheelDataChannel::setClientHostIP
 *
 * Set client IP address for data channel connection.
 *
 * @param clientIP  Client IP Address.
 */
void CogWheelDataChannel::setClientHostIP(QString clientIP)
{
    cogWheelInfo(m_controlSocketHandle,"Data channel client IP "+clientIP);
    m_clientHostIP.setAddress(clientIP);
}

/**
 * @brief CogWheelDataChannel::setClientHostPort
 *
 * Set client port number for data channel connection.
 *
 * @param clientPort    Client port number.
 */
void CogWheelDataChannel::setClientHostPort(quint16 clientPort)
{

    cogWheelInfo(m_controlSocketHandle,"Data channel client Port "+QString::number(clientPort));
    m_clientHostPort = clientPort;
}

/**
 * @brief CogWheelDataChannel::listenForConnection
 *
 * Listen for connection from client (passive mode).
 *
 * @param serverIP  Server IP address.
 */
void CogWheelDataChannel::listenForConnection(const QString &serverIP)
{
    try
    {
        // Pick a random port and start listening
        if(listen(QHostAddress::Any, m_clientHostPort)) {
            cogWheelInfo(m_controlSocketHandle,"Listening for passive connect....");
            setClientHostIP(serverIP);
            setClientHostPort(serverPort());
        } else {
            cogWheelError("Possible Data port conflict on port: "+QString::number(m_clientHostPort));
            throw CogWheelDataChannel::Exception("Possible data channel conflict on port: "+QString::number(m_clientHostPort));
        }
        emit passiveConnection();
        m_listening=true;
    }catch(std::exception &err) {
        throw CogWheelFtpServerReply(425, err.what());
    }catch(...) {
        throw CogWheelFtpServerReply(425,"Unknown error in listenForConnection().");
    }
}

/**
 * @brief CogWheelDataChannel::downloadFile
 *
 * Download a given local file over data channel to client.
 *
 * @param connection    Pointer to control channel instance.
 * @param fileName      Local file name.
 */
void CogWheelDataChannel::downloadFile(CogWheelControlChannel *connection, const QString &fileName)
{

    try {

        m_fileBeingTransferred = new QFile(fileName);

        if (m_fileBeingTransferred==nullptr) {
            throw CogWheelFtpServerReply(451, "QFile instance for "+fileName+" could not be created.");
        }

        // Open the file

        if(!m_fileBeingTransferred->open(QFile::ReadOnly)) {
            fileTransferCleanup();
            throw CogWheelFtpServerReply(451, "Error: File "+fileName+" could not be opened.");
        }

        cogWheelInfo(m_controlSocketHandle,"Downloading file "+fileName+".");

        // Move to the requested position

        if(connection->restoreFilePostion() > 0) {
            cogWheelInfo(m_controlSocketHandle,"Restoring previous position.");
            m_fileBeingTransferred->seek(connection->restoreFilePostion());
        }

        m_downloadFileSize = m_fileBeingTransferred->size()-connection->restoreFilePostion();

        // Send initial block of file

        if (m_fileBeingTransferred->size()) {
            QByteArray buffer = m_fileBeingTransferred->read(m_writeBytesSize);
            m_dataChannelSocket->write(buffer);
        } else {
            bytesWritten(0);   // File is zero length (close connection/signal success)
        }

    } catch(std::exception &err) {
        fileTransferCleanup();
        throw CogWheelFtpServerReply(451, err.what());
    } catch(...) {
        fileTransferCleanup();
        throw CogWheelFtpServerReply(451,"Uknown error in downloadFile().");
    }

}

/**
 * @brief CogWheelDataChannel::uploadFile
 *
 * Start upload of a given remote file to server over data channel.
 * The actual upload takes place using the sockets readyRead() slot
 * function.
 *
 * @param connection    Pointer to control channel instance.
 * @param fileName      Local destination file name.
 */
void CogWheelDataChannel::uploadFile(CogWheelControlChannel *connection, const QString &fileName)
{

    m_fileBeingTransferred = new QFile(fileName);

    if (m_fileBeingTransferred==nullptr) {
        throw CogWheelFtpServerReply(451, "QFile instance for "+fileName+" could not be cretaed.");
    }

    if(!m_fileBeingTransferred->open(QFile::Append)) {
        fileTransferCleanup();
        throw CogWheelFtpServerReply(451, "File "+fileName+" could not be opened.");
    }

    // Truncate the file if needed

    if(connection->restoreFilePostion() > 0) {
        if(!m_fileBeingTransferred->resize(connection->restoreFilePostion()))  {
            fileTransferCleanup();
            throw CogWheelFtpServerReply(451, "File "+fileName+" could not be truncated.");
        }
    }

}

/**
 * @brief CogWheelDataChannel::enbleDataChannelTLSSupport
 *
 * Enable SSL on data channel.
 *
 */
void CogWheelDataChannel::enbleDataChannelTLSSupport(CogWheelControlChannel *connection)
{

    // Use ony secure protocols

    m_dataChannelSocket->setProtocol(QSsl::SecureProtocols);

    // Create private key and cert

    QSslKey sslPrivateKey(connection->serverPrivateKey(), QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);
    QSslCertificate sslCert(connection->serverCert());

    m_dataChannelSocket->addCaCertificate(sslCert);
    m_dataChannelSocket->setLocalCertificate(sslCert);
    m_dataChannelSocket->setPrivateKey(sslPrivateKey);

    // Hookup error and channel encypted signals

    connect(m_dataChannelSocket, static_cast<void(QSslSocket::*)(const QList<QSslError> &)>(&QSslSocket::sslErrors), this, &CogWheelDataChannel::sslError);
    connect(m_dataChannelSocket, &QSslSocket::encrypted,this, &CogWheelDataChannel::dataChannelEncrypted);

    // Set keep alive and start TLS negotation

    m_dataChannelSocket->setSocketOption(QAbstractSocket::KeepAliveOption, true );

    m_dataChannelSocket->startServerEncryption();
    m_dataChannelSocket->waitForEncrypted(-1);

}

/**
 * @brief CogWheelDataChannel::sslError
 *
 * Report any ssl related errors.
 *
 * @param errors
 */
void CogWheelDataChannel::sslError(QList<QSslError> errors)
{

    QString errorStr="";

    foreach (const QSslError &e, errors) {
        errorStr.append(e.errorString());
    }

    cogWheelError(m_controlSocketHandle,errorStr);

    m_dataChannelSocket->ignoreSslErrors();

}

/**
 * @brief CogWheelDataChannel::controlChannelEncrypted
 */
void CogWheelDataChannel::dataChannelEncrypted()
{

    cogWheelInfo(m_controlSocketHandle,"Data Channel now encrypted.");

    m_sslConnection=true;

}

/**
 * @brief CogWheelDataChannel::incomingConnection
 *
 * Handle connection from client to server for data channel
 * for passive mode.
 *
 * @param handle    Handle to socket.
 */
void CogWheelDataChannel::incomingConnection(qintptr handle)
{

    cogWheelInfo(m_controlSocketHandle,"--- Incoming connection for data channel --- "+QString::number(handle));

    if(!m_dataChannelSocket->setSocketDescriptor(handle)){
        throw CogWheelFtpServerReply(425, "Error binding socket: "+m_dataChannelSocket->errorString());
    } else {
        cogWheelInfo(m_controlSocketHandle,"Data channel socket connected for handle : "+QString::number(handle));
    }

}

/**
 * @brief CogWheelDataChannel::connected
 *
 * Data channel socket connected slot function.
 *
 */
void CogWheelDataChannel::connected()
{
    cogWheelInfo(m_controlSocketHandle,"Data channel connected.");
}

/**
 * @brief CogWheelDataChannel::disconnected
 *
 * Data channel socket disconnect slot function. If a
 * file is being uploaded/downloaded then reset any
 * related variables
 *
 */
void CogWheelDataChannel::disconnected()
{

    cogWheelInfo(m_controlSocketHandle,"Data channel disconnected.");

    m_connected=false;

    if (m_fileBeingTransferred) {
        fileTransferCleanup();
        emit transferFinished();
    }

}

/**
 * @brief CogWheelDataChannel::bytesWritten
 *
 * Data channel socket bytes written slot function. If a file
 * is being downloaded subtract bytes from file size and
 * when reaches zero disconnect.
 *
 * @param numBytes  Number of bytes written (unused).
 */
void CogWheelDataChannel::bytesWritten(qint64 numBytes)
{

    if (m_fileBeingTransferred) {
        m_downloadFileSize -= numBytes;
        if (m_downloadFileSize==0) {
            m_dataChannelSocket->disconnectFromHost();
            return;
        }
        if (!m_fileBeingTransferred->atEnd()) {
            QByteArray buffer = m_fileBeingTransferred->read(m_writeBytesSize);
            m_dataChannelSocket->write(buffer);
        }
    }
}

/**
 * @brief CogWheelDataChannel::fileTransferCleanup
 *
 * File upload/download cleanup code. This includes
 * closing any file and deleting its object instance.
 */
void CogWheelDataChannel::fileTransferCleanup()
{
    if (m_fileBeingTransferred) {
        if (m_fileBeingTransferred->isOpen()) {
            m_fileBeingTransferred->close();
        }
        m_fileBeingTransferred->deleteLater();
        m_fileBeingTransferred=nullptr;
        m_downloadFileSize=0;
    }
}

/**
 * @brief CogWheelDataChannel::dataChannelSocketCleanup
 *
 * Data channel socket cleanup. This includes closing if open
 * and deleting its object instance.
 *
 */
void CogWheelDataChannel::dataChannelSocketCleanup()
{

    if (m_dataChannelSocket) {
        if (m_dataChannelSocket->isOpen()) {
            m_dataChannelSocket->close();
        }
        m_dataChannelSocket->deleteLater();
        m_dataChannelSocket=nullptr;
    }
}

/**
 * @brief CogWheelDataChannel::readyRead
 *
 * Data channel socket readyRead slot function. Used to
 * perform the uploading of a file to server once it has
 * been kicked off.
 *
 */
void CogWheelDataChannel::readyRead()
{

    if(m_fileBeingTransferred) {
        m_fileBeingTransferred->write(m_dataChannelSocket->readAll());
    }

}

/**
 * @brief CogWheelDataChannel::socketError
 *
 * data channel socket error slot function.
 *
 * @param socketError   Socket error.
 */
void CogWheelDataChannel::socketError(QAbstractSocket::SocketError socketError)
{
    if (socketError!=QAbstractSocket::RemoteHostClosedError) {
        cogWheelError(m_controlSocketHandle,"Data channel socket error: "+QString::number(socketError));
    }
}

// ============================
// CLASS PRIVATE DATA ACCESSORS
// ============================

bool CogWheelDataChannel::IsSslConnection() const
{
    return m_sslConnection;
}

void CogWheelDataChannel::setSslConnection(bool sslConnection)
{
    m_sslConnection = sslConnection;
}

/**
 * @brief CogWheelDataChannel::dataChannelSocket
 * @return
 */
QSslSocket *CogWheelDataChannel::dataChannelSocket() const
{
    return m_dataChannelSocket;
}

/**
 * @brief CogWheelDataChannel::setDataChannelSocket
 * @param dataChannelSocket
 */
void CogWheelDataChannel::setDataChannelSocket(QSslSocket *dataChannelSocket)
{
    m_dataChannelSocket = dataChannelSocket;
}

/**
 * @brief CogWheelDataChannel::isConnected
 * @return
 */
bool CogWheelDataChannel::isConnected() const
{
    return m_connected;
}

/**
 * @brief CogWheelDataChannel::setConnected
 * @param connected
 */
void CogWheelDataChannel::setConnected(bool connected)
{
    m_connected = connected;
}

/**
 * @brief CogWheelDataChannel::isListening
 * @return
 */
bool CogWheelDataChannel::isListening() const
{
    return m_listening;
}

/**
 * @brief CogWheelDataChannel::setListening
 * @param listening
 */
void CogWheelDataChannel::setListening(bool listening)
{
    m_listening = listening;
}

/**
 * @brief CogWheelDataChannel::clientHostIP
 * @return
 */
QHostAddress CogWheelDataChannel::clientHostIP() const
{
    return m_clientHostIP;
}

/**
 * @brief CogWheelDataChannel::clientHostPort
 * @return
 */
quint16 CogWheelDataChannel::clientHostPort() const
{
    return m_clientHostPort;
}
