/*
 * File:   cogwheeldatachannel.h
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

#ifndef COGWHEELDATACHANNEL_H
#define COGWHEELDATACHANNEL_H

#include "cogwheel.h"

#include <QObject>
#include <QString>
#include <QHostAddress>
#include <QTcpServer>
#include <QSslSocket>
#include <QSslCertificate>
#include <QSslKey>
#include <QFile>

class CogWheelControlChannel;

class CogWheelDataChannel : public QTcpServer
{
    Q_OBJECT

public:

  // Constructor / Destructor

    explicit CogWheelDataChannel(qintptr controlSocketHandle, QObject *parent = nullptr);
    ~CogWheelDataChannel();

    // Channel control

    bool connectToClient(CogWheelControlChannel *connection);
    void disconnectFromClient(CogWheelControlChannel *connection);

    // Channel action

    void listenForConnection(const QString &serverIP);
    void downloadFile(CogWheelControlChannel *connection, const QString &fileName);
    void uploadFile(CogWheelControlChannel *connection, const QString &fileName);

    // TLS

    void enbleDataChannelTLSSupport(CogWheelControlChannel *connection);

    // Private data accessors

    void setClientHostIP(QString clientIP);
    void setClientHostPort(quint16 clientPort);
    QHostAddress clientHostIP() const;
    quint16 clientHostPort() const;
    bool isListening() const;
    void setListening(bool isListening);
    bool isConnected() const;
    void setConnected(bool isConnected);
    bool isFileBeingUploaded() const;
    void setFileBeingUploaded(bool isFileBeingUploaded);
    QSslSocket *dataChannelSocket() const;
    void setDataChannelSocket(QSslSocket *dataChannelSocket);
    bool IsSslConnection() const;
    void setSslConnection(bool IsSslConnection);

private:

    // Cleanup after file transfer

    void fileTransferCleanup();

protected:

    // QTcpServer overrides

    void incomingConnection(qintptr handle);

signals:

    // Channel notification

    void transferFinished();                   // File transfer finished
    void passiveConnection();                  // Passive connection

public slots:

    // Data channel socket

    void connected();
    void disconnected();
    void stateChanged(QAbstractSocket::SocketState socketState);
    void bytesWritten(qint64 numBytes);
    void readyRead();
    void socketError(QAbstractSocket::SocketError socketError);

    // TLS/SSL specific

    void sslError(QList<QSslError> errors);
    void dataChannelEncrypted();

private:
    qintptr m_controlSocketHandle;        // Control channel socket handle
    QSslSocket *m_dataChannelSocket;      // Data channel socket
    QHostAddress m_clientHostIP;          // Address of client
    quint16 m_clientHostPort;             // Port used on client
    bool m_connected=false;               // == true data channel connected
    bool m_listening=false;               // == true listening on data channel
    QFile *m_fileBeingTransferred=nullptr;// Upload/download file
    quint64 m_downloadFileSize=0;         // Downloading file size
    qint64 m_writeBytesSize=0;            // No of bytes per write
    bool m_sslConnection=false;           // == true connection is SSL

};

#endif // COGWHEELDATACHANNEL_H
