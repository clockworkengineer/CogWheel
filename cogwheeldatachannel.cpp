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
// The channel to created and destroyed on an as needed basis and can operative
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
 * @param parent
 */
CogWheelDataChannel::CogWheelDataChannel(QObject *parent)
{
    Q_UNUSED(parent);

    qDebug() << "Data channel created.";

    m_dataChannelSocket = new QTcpSocket();

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
 * @param connection
 * @return
 */
bool CogWheelDataChannel::connectToClient(CogWheelControlChannel *connection)
{

    if (m_connected) {
        qDebug() << "Data channel already connected.";
        return(m_connected);
    }

    if (!connection->isPassive()) {

        qDebug() << "Active Mode. Connecting data channel to client ....";

        m_dataChannelSocket->connectToHost(m_clientHostIP, m_clientHostPort);
        m_dataChannelSocket->waitForConnected(-1);
        connection->sendReplyCode(150);

    } else {

        qDebug() << "Passive Mode. Waiting to connect to data channel ....";

        if (m_dataChannelSocket->state() != QAbstractSocket::ConnectedState) {
            waitForNewConnection(-1);
        }

        connection->sendReplyCode(125);

    }

    qDebug() << "connected.";

    m_connected=true;

    qDebug() << "-- Datat channel State --" << m_dataChannelSocket->state();

    if (m_dataChannelSocket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "-- Data channel socket Error String --" << m_dataChannelSocket->errorString();
    }

    return(m_connected);

}

/**
 * @brief CogWheelDataChannel::disconnectFromClient
 * @param connection
 */
void CogWheelDataChannel::disconnectFromClient(CogWheelControlChannel *connection)
{

    if (m_dataChannelSocket->state() == QAbstractSocket::ConnectedState) {
        m_dataChannelSocket->disconnectFromHost();
        m_dataChannelSocket->waitForDisconnected(-1);
        connection->sendReplyCode(226);
    } else {
        qDebug() << "Data channel socket not connected.";
    }
    m_connected=false;
}

/**
 * @brief CogWheelDataChannel::setClientHostIP
 * @param clientIP
 */
void CogWheelDataChannel::setClientHostIP(QString clientIP)
{
    qDebug() << "Data channel client IP " << clientIP;
    m_clientHostIP.setAddress(clientIP);
}

/**
 * @brief CogWheelDataChannel::setClientHostPort
 * @param clientPort
 */
void CogWheelDataChannel::setClientHostPort(quint16 clientPort)
{

    qDebug() << "Data channel client Port " << clientPort;
    m_clientHostPort = clientPort;
}

/**
 * @brief CogWheelDataChannel::listenForConnection
 * @param serverIP
 */
void CogWheelDataChannel::listenForConnection(QString serverIP)
{
    try
    {
        //Pick a random port and start listening
        if(listen(QHostAddress::Any)) {
            qDebug() << "Listening....";
            setClientHostIP(serverIP);
            setClientHostPort(serverPort());
            emit passiveConnection();
            m_listening=true;
        }//else {
        //    emit passiveConnection();
       // }
    }catch(QString err) {
        emit error(err);
    }
    catch(...)
    {
        emit error("Unknown error in listenForConnection");
    }
}

/**
 * @brief CogWheelDataChannel::downloadFile
 * @param connection
 * @param fileName
 */
void CogWheelDataChannel::downloadFile(CogWheelControlChannel *connection, QString fileName)
{

    try
    {

        m_fileBeingUploaded = false;
        m_transferFileName = fileName;

        //Open the file

        QFile file(fileName);

        if(!file.open(QFile::ReadOnly)) {
            emit error("Could not open file!");
            return;
        }

        qDebug() << "*** FileSocket *** " << fileName;

        //Move to the requested position
        if(connection->restoreFilePostion() > 0) {
            file.seek(connection->restoreFilePostion());
        }

        //Send the contents of the file
        while (!file.atEnd()) {
            QByteArray buffer = file.read(1024 * 8);
            connection->sendOnDataChannel(buffer);
        }

        //Close the file
        file.close();

        //Tell connected objects we are done
        emit downloadFinished();

        //Close the socket once we are done
        disconnectFromClient(connection);

    } catch(QString err) {
        emit error(err);
    } catch(...) {
        emit error("Unknown error in file downloadFile().");
    }
}

/**
 * @brief CogWheelDataChannel::uploadFile
 * @param connection
 * @param fileName
 */
void CogWheelDataChannel::uploadFile(CogWheelControlChannel *connection, QString fileName)
{

    m_transferFileName = fileName;
    m_fileBeingUploaded = true;

    //Truncate the file if needed
    if(connection->restoreFilePostion() > 0) {
        QFile file(fileName);
        if(!file.resize(connection->restoreFilePostion()))  {
            emit error("File could not be truncated!");
            return;
        }
    }

}

/**
 * @brief CogWheelDataChannel::incomingConnection
 * @param handle
 */
void CogWheelDataChannel::incomingConnection(qintptr handle)
{

    qDebug() << "--- CogWheelDataChannel incomingConnection ---" << handle;


    if(!m_dataChannelSocket->setSocketDescriptor(handle)){
        qDebug() << "-- File Socket --" << handle << " Error binding socket: " << m_dataChannelSocket->errorString();
        emit error("Error binding socket.");

    } else {
        qDebug() << "-- File Socket --" << handle << " session Connected";
    }

}

/**
 * @brief CogWheelDataChannel::connected
 */
void CogWheelDataChannel::connected()
{
    qDebug() << "CogWheelDataChannel::connected()";
}

/**
 * @brief CogWheelDataChannel::disconnected
 */
void CogWheelDataChannel::disconnected()
{
    qDebug() << "CogWheelDataChannel::disconnected()";

    if (m_fileBeingUploaded) {
        m_fileBeingUploaded=false;
        m_transferFileName="";
        m_connected=false;
        emit uploadFinished();
    }

}

/**
 * @brief CogWheelDataChannel::stateChanged
 * @param socketState
 */
void CogWheelDataChannel::stateChanged(QAbstractSocket::SocketState socketState)
{

    Q_UNUSED(socketState);

}

/**
 * @brief CogWheelDataChannel::bytesWritten
 * @param numBytes
 */
void CogWheelDataChannel::bytesWritten(qint64 numBytes)
{

    Q_UNUSED(numBytes);

}

/**
 * @brief CogWheelDataChannel::readyRead
 */
void CogWheelDataChannel::readyRead()
{
    qDebug() << "CogWheelDataChannel::readyRead()";

    if(m_fileBeingUploaded  && m_transferFileName != "") {

        QFile file(m_transferFileName);

        if(!file.open(QFile::Append)) {
            emit error("Could not open file!");
            return;
        }

        qDebug() << "STATE =" << m_dataChannelSocket->state();

        QByteArray buffer = m_dataChannelSocket->readAll();
        file.write(buffer);
        file.close();
    }
}

/**
 * @brief CogWheelDataChannel::socketError
 * @param socketError
 */
void CogWheelDataChannel::socketError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "dataChannelSocketError" << socketError;

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
