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
//

// =============
// INCLUDE FILES
// =============

#include "cogwheeldatachannel.h"
#include "cogwheelcontrolchannel.h"

#include <QtCore>
#include <QAbstractSocket>

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

    m_dataChannelSocket = new QTcpSocket();

    if (m_dataChannelSocket==nullptr) {
        emit error("Error trying to create data channel socket.");
        return;
    }

    connect(m_dataChannelSocket, &QTcpSocket::connected, this, &CogWheelDataChannel::connected, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QTcpSocket::disconnected, this, &CogWheelDataChannel::disconnected, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QTcpSocket::stateChanged, this, &CogWheelDataChannel::stateChanged, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QTcpSocket::bytesWritten, this, &CogWheelDataChannel::bytesWritten, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QTcpSocket::readyRead, this, &CogWheelDataChannel::readyRead, Qt::DirectConnection);

    connect(m_dataChannelSocket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
            this, &CogWheelDataChannel::socketError, Qt::DirectConnection);


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

        //connection->sendReplyCode(150);

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

    emit info("connected.");

    m_connected=true;

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
        m_dataChannelSocket->disconnectFromHost();
        m_dataChannelSocket->waitForDisconnected(-1);
        connection->sendReplyCode(226);
    } else {
        emit error("Data channel socket not connected.");
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

    try
    {

        m_fileBeingDownloaded =true;
        m_fileBeingUploaded = false;
        m_transferFileName = fileName;

        // Open the file

        QFile file(fileName);

        if(!file.open(QFile::ReadOnly)) {
            m_fileBeingDownloaded = false;
            m_transferFileName = "";
            emit error("Error: File "+fileName+" could not be opened.");
            return;
        }

        emit info("Downloading file "+fileName+".");

        // Move to the requested position
        if(connection->restoreFilePostion() > 0) {
            emit info("Restoring previous position.");
            file.seek(connection->restoreFilePostion());
        }

        m_downloadFileSize = file.size();

        // Send the contents of the file

        while (!file.atEnd()) {
            QByteArray buffer = file.read(1024 * 8);
            connection->sendOnDataChannel(buffer); // This should be a parameter somewhere.
        }

        // Close the file

        file.close();

    } catch(QString err) {
        emit error(err);
    } catch(...) {
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

    m_transferFileName = fileName;
    m_fileBeingUploaded = true;
    m_fileBeingDownloaded =false;

    // Truncate the file if needed

    if(connection->restoreFilePostion() > 0) {
        QFile file(fileName);
        if(!file.resize(connection->restoreFilePostion()))  {
            emit error("File "+fileName+" could not be truncated.");
            m_transferFileName = "";
            m_fileBeingUploaded = false;
            return;
        }
    }

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
 * file is being uploaded then reset any related variables
 *
 */
void CogWheelDataChannel::disconnected()
{

    emit info("Data channel disconnected.");

    if (m_fileBeingUploaded) {
        m_fileBeingUploaded=false;
        m_transferFileName="";
        m_connected=false;
        emit uploadFinished();
    } else if (m_fileBeingDownloaded) {
        m_fileBeingDownloaded=false;
        m_transferFileName="";
        m_connected=false;
        m_downloadFileSize=0;
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
 * Data channel socket bytes written slot function. If file
 * is being downloaded subtract bytes from file size and
 * when reaches zero disconnect and signal complete.
 *
 * @param numBytes  Number of bytes written (unused).
 */
void CogWheelDataChannel::bytesWritten(qint64 numBytes)
{

    if (m_fileBeingDownloaded && m_transferFileName != "") {
        m_downloadFileSize -= numBytes;
        if (m_downloadFileSize==0) {
            m_dataChannelSocket->disconnectFromHost();
            emit downloadFinished();
        }
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

    if(m_fileBeingUploaded  && m_transferFileName != "") {

        QFile file(m_transferFileName);

        if(!file.open(QFile::Append)) {
            emit error("File "+m_transferFileName+" could not be opened.");
            return;
        }

        emit info("Uploading file "+m_transferFileName+"...");

        file.write(m_dataChannelSocket->readAll());

        file.close();

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
    emit error("Data channel socket error: "+QString::number(socketError));
}

// ============================
// CLASS PRIVATE DATA ACCESSORS
// ============================

/**
 * @brief CogWheelDataChannel::transferFileName
 * @return
 */
QString CogWheelDataChannel::transferFileName() const
{
    return m_transferFileName;
}

/**
 * @brief CogWheelDataChannel::setTransferFileName
 * @param transferFileName
 */
void CogWheelDataChannel::setTransferFileName(const QString &transferFileName)
{
    m_transferFileName = transferFileName;
}

/**
 * @brief CogWheelDataChannel::isFileBeingUploaded
 * @return
 */
bool CogWheelDataChannel::isFileBeingUploaded() const
{
    return m_fileBeingUploaded;
}

/**
 * @brief CogWheelDataChannel::setFileBeingUploaded
 * @param fileBeingUploaded
 */
void CogWheelDataChannel::setFileBeingUploaded(bool fileBeingUploaded)
{
    m_fileBeingUploaded = fileBeingUploaded;
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
