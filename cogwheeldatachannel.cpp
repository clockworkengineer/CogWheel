#include "cogwheeldatachannel.h"
#include "cogwheelconnection.h"

#include <QtCore>
#include <QAbstractSocket>

CogWheelDataChannel::CogWheelDataChannel(QObject *parent)
{
    qDebug() << "Data channel created.";

    m_dataChannelSocket = new QTcpSocket();

    connect(m_dataChannelSocket, &QTcpSocket::connected, this, &CogWheelDataChannel::dataChannelConnect, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QTcpSocket::disconnected, this, &CogWheelDataChannel::dataChannelDisconnect, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QTcpSocket::stateChanged, this, &CogWheelDataChannel::dataChannelStateChanged, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QTcpSocket::bytesWritten, this, &CogWheelDataChannel::dataChannelBytesWritten, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QTcpSocket::readyRead, this, &CogWheelDataChannel::dataChannelReadyRead, Qt::DirectConnection);

    connect(m_dataChannelSocket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
            this, &CogWheelDataChannel::dataChannelSocketError, Qt::DirectConnection);

}

bool CogWheelDataChannel::connectToClient(CogWheelConnection *connection)
{

    if (m_connected) {
        qDebug() << "Data channel already connected.";
        return(m_connected);
    }

    if (!connection->passive()) {

        qDebug() << "Active Mode. Connecting data channel to client ....";

        m_dataChannelSocket->connectToHost(m_clientHostIP, m_clientHostPort);
        m_dataChannelSocket->waitForConnected();
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

void CogWheelDataChannel::disconnectFromClient(CogWheelConnection *connection)
{

    if (m_dataChannelSocket->state() == QAbstractSocket::ConnectedState) {
        m_dataChannelSocket->disconnectFromHost();
        m_dataChannelSocket->waitForDisconnected(-1);
    } else {
        qDebug() << "Data channel socket not connected.";
    }
    m_connected=false;
}

void CogWheelDataChannel::setClientHostIP(QString clientIP)
{
    qDebug() << "Data channel client IP " << clientIP;
    m_clientHostIP.setAddress(clientIP);
}

void CogWheelDataChannel::setClientHostPort(quint16 clientPort)
{

    qDebug() << "Data channel client Port " << clientPort;
    m_clientHostPort = clientPort;
}

void CogWheelDataChannel::listenForConnection(QString serverIP)
{
    try
    {
        //Pick a random port and start listening
        if(listen(QHostAddress::Any)) {
            qDebug() << "Listening....";
            setClientHostIP(serverIP);
            setClientHostPort(serverPort());
            emit dataChannelPassiveConnection();
            m_listening=true;
        }else {
            emit dataChannelPassiveConnection();
        }
    }catch(QString err) {
        emit dataChannelError(err);
    }
    catch(...)
    {
        emit dataChannelError("Unknown error in ListenFor()");
    }
}

void CogWheelDataChannel::downloadFile(CogWheelConnection *connection, QString fileName)
{

    try
    {

        m_fileBeingUploaded = false;
        m_transferFileName = fileName;

        //Open the file

        QFile file(fileName);

        if(!file.open(QFile::ReadOnly)) {
            emit dataChannelError("Could not open file!");
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
            connection->sendOnDataChannel(QString(buffer));
        }

        //Close the file
        file.close();

        //Tell connected objects we are done
        emit dataChannelFinished();

        //Close the socket once we are done
        disconnectFromClient(connection);

    } catch(QString err) {
        emit dataChannelError(err);
    } catch(...) {
        emit dataChannelError("Unknown error in file download().");
    }
}

void CogWheelDataChannel::uploadFile(CogWheelConnection *connection, QString fileName)
{

    m_transferFileName = fileName;
    m_fileBeingUploaded = true;

    //Truncate the file if needed
    if(connection->restoreFilePostion() > 0) {
        QFile file(fileName);
        if(!file.resize(connection->restoreFilePostion()))  {
            emit dataChannelError("File could not be truncated!");
            return;
        }
    }

}

void CogWheelDataChannel::incomingConnection(qintptr handle)
{

    qDebug() << "--- CogWheelDataChannel incomingConnection ---" << handle;


    if(!m_dataChannelSocket->setSocketDescriptor(handle)){
        qDebug() << "-- File Socket --" << handle << " Error binding socket: " << m_dataChannelSocket->errorString();
        emit dataChannelError("Error binding socket.");

    } else {
        qDebug() << "-- File Socket --" << handle << " session Connected";
        //  m_connected=true;
        //Let connected objects know we are connected
        //emit OnConnected();
    }

}

void CogWheelDataChannel::dataChannelConnect()
{
    qDebug() << "CogWheelDataChannel::connected()";
}

void CogWheelDataChannel::dataChannelDisconnect()
{
    qDebug() << "CogWheelDataChannel::disconnected()";

    if (m_fileBeingUploaded) {
        m_fileBeingUploaded=false;
        m_transferFileName="";
        emit dataChannelUploadFinished();
    }

}

void CogWheelDataChannel::dataChannelStateChanged(QAbstractSocket::SocketState socketState)
{

}

void CogWheelDataChannel::dataChannelBytesWritten(qint64 numBytes)
{

}

void CogWheelDataChannel::dataChannelReadyRead()
{
    qDebug() << "CogWheelDataChannel::readyRead()";

    if(m_fileBeingUploaded  && m_transferFileName != "") {

        QFile file(m_transferFileName);

        if(!file.open(QFile::Append))
        {
            emit dataChannelError("Could not open file!");
            return;
        }

        QByteArray buffer = m_dataChannelSocket->readAll();
        file.write(buffer);
        file.close();
    }
}

void CogWheelDataChannel::dataChannelSocketError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "CogWheelDataChannel::error()";

    emit dataChannelSocketError(socketError);

}

bool CogWheelDataChannel::connected() const
{
    return m_connected;
}

void CogWheelDataChannel::setConnected(bool connected)
{
    m_connected = connected;
}

bool CogWheelDataChannel::listening() const
{
    return m_listening;
}

void CogWheelDataChannel::setListening(bool listening)
{
    m_listening = listening;
}

QHostAddress CogWheelDataChannel::clientHostIP() const
{
    return m_clientHostIP;
}

quint16 CogWheelDataChannel::clientHostPort() const
{
    return m_clientHostPort;
}

