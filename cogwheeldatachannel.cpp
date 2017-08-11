#include "cogwheeldatachannel.h"
#include "cogwheelconnection.h"

#include <QtCore>
#include <QAbstractSocket>

CogWheelDataChannel::CogWheelDataChannel(QObject *parent)
{
    qDebug() << "CogWheelDataChannel created.";

    m_dataChannelSocket = new QTcpSocket();

    connect(m_dataChannelSocket, &QTcpSocket::connected, this, &CogWheelDataChannel::connected, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QTcpSocket::disconnected, this, &CogWheelDataChannel::disconnected, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QTcpSocket::stateChanged, this, &CogWheelDataChannel::stateChanged, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QTcpSocket::bytesWritten, this, &CogWheelDataChannel::bytesWritten, Qt::DirectConnection);
    connect(m_dataChannelSocket, &QTcpSocket::readyRead, this, &CogWheelDataChannel::readyRead, Qt::DirectConnection);

    connect(m_dataChannelSocket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
            this, &CogWheelDataChannel::error, Qt::DirectConnection);

}

bool CogWheelDataChannel::connectToClient(CogWheelConnection *connection)
{

    if (m_connected) {
        qDebug() << "CogWheelDataChannel::connectToClient: Already connected.";
        return(m_connected);
    }

    if (!connection->passive()) {

        m_dataChannelSocket->connectToHost(m_clientHostIP, m_clientHostPort);
        m_dataChannelSocket->waitForConnected();

        connection->sendReplyCode(150);

    } else {

        qDebug() << "Waiting";
   //     while(m_dataChannelSocket->state() != QAbstractSocket::ConnectedState) {
            waitForNewConnection(-1);
           // if (m_dataChannelSocket->state() != QAbstractSocket::ConnectedState) {
           //     qDebug() << "Not Connected" << m_dataChannelSocket->state();
          //  }
   //     }
        connection->sendReplyCode(125);

    }

     m_connected=true;

    qDebug() << "-- File Socket State --" << m_dataChannelSocket->state();

    if (m_dataChannelSocket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "-- File Socket Error String --" << m_dataChannelSocket->errorString();
    }

    return(m_connected);

}

void CogWheelDataChannel::disconnectFromClient(CogWheelConnection *connection)
{

    if (m_dataChannelSocket->state() == QAbstractSocket::ConnectedState) {
        m_dataChannelSocket->disconnectFromHost();
        m_dataChannelSocket->waitForDisconnected(-1);
    } else {
        qDebug() << "CogWheelDataChannel::disconnectFromHost: socket not connected.";
    }
    m_connected=false;
}

void CogWheelDataChannel::setClientHostIP(QString clientIP)
{
    qDebug() << "CogWheelDataChannel::setClientHostIP " << clientIP;
    m_clientHostIP.setAddress(clientIP);
}

void CogWheelDataChannel::setClientHostPort(quint16 clientPort)
{

    qDebug() << "CogWheelDataChannel::setClientHostPort " << clientPort;
    m_clientHostPort = clientPort;
}

void CogWheelDataChannel::listenForConnection()
{
    try
    {
        //Pick a random port and start listening
        if(listen(QHostAddress::Any))
        {
            qDebug() << "Listening....";
            //setClientHostIP("192.168.1.71");
            setClientHostIP("127.0.0.1");
            setClientHostPort(serverPort());
            emit passiveConnection();
        }
        else
        {
            //this->resumeAccepting();
            emit passiveConnection();
            qDebug() << "resume Listening....";
        }
    }
    catch(QString err)
    {
        //  emit OnError(err);
    }
    catch(...)
    {
        // emit OnError("Unknown error in ListenFor()");
    }
}

void CogWheelDataChannel::downloadFile(CogWheelConnection *connection, QString fileName)
{

    try
    {
        //        //Set the transfer variables
        //        mUploading = false;
        //        mFilename = Filename;

        //Open the file
        QFile file(fileName);

        if(!file.open(QFile::ReadOnly))
        {
            //            emit OnError("Could not open file!");
            return;
        }

        qDebug() << "*** FileSocket *** " << fileName;

        //        //Move to the requested position
        //        if(Position > 0)
        //        {
        //            file.seek(Position);
        //        }

        //Send the contents of the file
        while (!file.atEnd()) {
            QByteArray buffer = file.read(1024 * 8);
            connection->sendOnDataChannel(QString(buffer));
        }

        //Close the file
        file.close();

        //Tell connected objects we are done
        //        emit OnFinished();

        //Close the socket once we are done
        disconnectFromClient(connection);
    }
    catch(QString err)
    {
        //        emit OnError(err);
    }
    catch(...)
    {
        //        emit OnError("Unknown error in Download()");
    }
}

void CogWheelDataChannel::uploadFile(CogWheelConnection *connection, QString fileName)
{

    m_uploadFileName = fileName;
    m_fileBeingUploaded = true;

}

void CogWheelDataChannel::incomingConnection(qintptr handle)
{

    qDebug() << "--- CogWheelDataChannel incomingConnection ---" << handle;


    if(!m_dataChannelSocket->setSocketDescriptor(handle))
    {
        qDebug() << "-- File Socket --" << handle << " Error binding socket: " << m_dataChannelSocket->errorString();
        // emit OnError("Error binding socket");

    }
    else
    {
        qDebug() << "-- File Socket --" << handle << " session Connected";
      //  m_connected=true;
        //Let connected objects know we are connected
         //emit OnConnected();
    }

    //Stop listing for connections
    //this->pauseAccepting();
    //this->close();

}

void CogWheelDataChannel::connected()
{
    qDebug() << "CogWheelDataChannel::connected()";
}

void CogWheelDataChannel::disconnected()
{
    qDebug() << "CogWheelDataChannel::disconnected()";

    if (m_fileBeingUploaded) {
        m_fileBeingUploaded=false;
        m_uploadFileName="";
        emit uploadFinished();
    }

//    this->resumeAccepting();
//    this->close();
  //  m_connected=false;
}

void CogWheelDataChannel::stateChanged(QAbstractSocket::SocketState socketState)
{
    //  qDebug() << "CogWheelDataChannel::stateChanged: " << socketState;
}

void CogWheelDataChannel::bytesWritten(qint64 numBytes)
{
    //  qDebug() << "CogWheelDataChannel::bytesWritten: " << numBytes;
}

void CogWheelDataChannel::readyRead()
{
    qDebug() << "CogWheelDataChannel::readyRead()";

    if(m_fileBeingUploaded  && m_uploadFileName != "")
    {
        QFile file(m_uploadFileName);

        if(!file.open(QFile::Append))
        {
            //   emit OnError("Could not open file!");
            return;
        }

        QByteArray buffer = m_dataChannelSocket->readAll();
        file.write(buffer);
        file.close();
    }
}

void CogWheelDataChannel::error(QAbstractSocket::SocketError socketError)
{
    qDebug() << "CogWheelDataChannel::error()";

    emit dataChannelSocketError(socketError);

}

QHostAddress CogWheelDataChannel::clientHostIP() const
{
    return m_clientHostIP;
}

quint16 CogWheelDataChannel::clientHostPort() const
{
    return m_clientHostPort;
}

