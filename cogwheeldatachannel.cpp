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

/**
 * @brief CogWheelDataChannel::CogWheelDataChannel
 *
 * Create data channel instance. Create a socket and
 * connect up its signals and slots.
 *
 * @param parent    parent object (not used).
 */
CogWheelDataChannel::CogWheelDataChannel(QObject *parent)
{
    Q_UNUSED(parent);

    emit info("Data channel created.");

    m_dataChannelSocket = new QSslSocket();

    if (m_dataChannelSocket==nullptr) {
        emit error("Error trying to create data channel socket.");
        return;
    }

    connect(m_dataChannelSocket, &QSslSocket::connected, this, &CogWheelDataChannel::connected, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QSslSocket::disconnected, this, &CogWheelDataChannel::disconnected, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QSslSocket::stateChanged, this, &CogWheelDataChannel::stateChanged, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QSslSocket::bytesWritten, this, &CogWheelDataChannel::bytesWritten, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QSslSocket::readyRead, this, &CogWheelDataChannel::readyRead, Qt::DirectConnection);

    connect(m_dataChannelSocket, static_cast<void (QSslSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
            this, &CogWheelDataChannel::socketError, Qt::DirectConnection);

}

/**
 * @brief CogWheelDataChannel::~CogWheelDataChannel
 */
CogWheelDataChannel::~CogWheelDataChannel()
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
 * @brief CogWheelDataChannel::connectToClient
 *
 * Connect up data channel; either from server (active)
 * or from client (passive).
 *
 * @param connection    Pointer to control channel instance.
 *
 * @return  true on sucessful connection
 */
bool CogWheelDataChannel::connectToClient(CogWheelControlChannel *connection)
{

    if (m_connected) {
        emit error("Data channel already connected.");
        return(m_connected);
    }

    if (!connection->isPassive()) {

        emit info("Active Mode. Connecting data channel to client ....");

        m_dataChannelSocket->connectToHost(m_clientHostIP, m_clientHostPort);
        m_dataChannelSocket->waitForConnected(-1);

        connection->sendReplyCode(150);

    } else {

        emit info("Passive Mode. Waiting to connect to data channel ....");

        if (m_dataChannelSocket->state() != QAbstractSocket::ConnectedState) {
            waitForNewConnection(-1);
        }

        connection->sendReplyCode(125);

    }


    m_connected=true;

    // Data channel protection set to private switch on SSL

    if (connection->dataChanelProtection()=='P') {
        enbleDataChannelTLSSupport(connection);
    }

    m_writeBytesSize = connection->writeBytesSize();

    if (m_dataChannelSocket->state() != QAbstractSocket::ConnectedState) {
        emit error("Data channel did not connect. Socket Error: "+m_dataChannelSocket->errorString());
    }

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

    if (m_dataChannelSocket->state() == QAbstractSocket::ConnectedState) {
        m_dataChannelSocket->flush();
        m_dataChannelSocket->disconnectFromHost();
        if (m_dataChannelSocket->state() != QAbstractSocket::UnconnectedState) {
            m_dataChannelSocket->waitForDisconnected(-1);
        }
        connection->sendReplyCode(226);
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
    emit info("Data channel client IP "+clientIP);
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

    emit info("Data channel client Port "+QString::number(clientPort));
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
        if(listen(QHostAddress::Any)) {
            emit info ("Listening for passive connect....");
            setClientHostIP(serverIP);
            setClientHostPort(serverPort());
        }
        emit passiveConnection();
        m_listening=true;
    }catch(QString err) {
        emit error(err);
    }catch(...) {
        emit error("Unknown error in listenForConnection().");
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
            emit error("QFile instance for "+fileName+" could not be created.");
            return;
        }

        // Open the file

        if(!m_fileBeingTransferred->open(QFile::ReadOnly)) {
            fileTransferCleanup();
            emit error("Error: File "+fileName+" could not be opened.");
            return;
        }

        emit info("Downloading file "+fileName+".");

        // Move to the requested position

        if(connection->restoreFilePostion() > 0) {
            emit info("Restoring previous position.");
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

    } catch(QString err) {
        fileTransferCleanup();
        emit error(err);
    } catch(...) {
        fileTransferCleanup();
        emit error("Unknown error in downloadFile().");
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
        emit error("QFile instance for "+fileName+" could not be cretaed.");
        return;
    }

    if(!m_fileBeingTransferred->open(QFile::Append)) {
        emit error("File "+fileName+" could not be opened.");
        fileTransferCleanup();
        return;
    }

    // Truncate the file if needed

    if(connection->restoreFilePostion() > 0) {
        if(!m_fileBeingTransferred->resize(connection->restoreFilePostion()))  {
            emit error("File "+fileName+" could not be truncated.");
            fileTransferCleanup();
            return;
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
 * @param errors
 */
void CogWheelDataChannel::sslError(QList<QSslError> errors)
{

    QString errorStr="";

    foreach (const QSslError &e, errors) {
        errorStr.append(e.errorString());
    }

    emit error(errorStr);

    m_dataChannelSocket->ignoreSslErrors();

}

/**
 * @brief CogWheelDataChannel::controlChannelEncrypted
 */
void CogWheelDataChannel::dataChannelEncrypted()
{

    emit info ("Data Channel now encrypted.");

    m_sslConnection=true;

}

bool CogWheelDataChannel::IsSslConnection() const
{
    return m_sslConnection;
}

void CogWheelDataChannel::setSslConnection(bool sslConnection)
{
    m_sslConnection = sslConnection;
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

    emit info("--- Incoming connection for data channel --- "+QString::number(handle));

    if(!m_dataChannelSocket->setSocketDescriptor(handle)){
        emit error( "Error binding socket: "+m_dataChannelSocket->errorString());
    } else {
        emit info("Data channel socket connected for handle : "+QString::number(handle));
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
    emit info("Data channel connected.");
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

    emit info("Data channel disconnected.");

    m_connected=false;

    if (m_fileBeingTransferred) {
        fileTransferCleanup();
        emit transferFinished();
    }

}

/**
 * @brief CogWheelDataChannel::stateChanged
 *
 * Data channel socket state changed slot function.
 *
 * @param socketState   New socket state.
 */
void CogWheelDataChannel::stateChanged(QAbstractSocket::SocketState socketState)
{
    Q_UNUSED(socketState);
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
 * closing any file and deleting its data.
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
 * ata channel socket error slot function.
 *
 * @param socketError   Socket error.
 */
void CogWheelDataChannel::socketError(QAbstractSocket::SocketError socketError)
{
    if (socketError==QAbstractSocket::RemoteHostClosedError) {
        emit info("Client closed data connection.");
    } else {
        emit error("Data channel socket error: "+QString::number(socketError));
    }
}

// ============================
// CLASS PRIVATE DATA ACCESSORS
// ============================

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
