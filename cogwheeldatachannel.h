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

#ifndef COGWHEELDATACHANNEL_H
#define COGWHEELDATACHANNEL_H

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QHostAddress>
#include <QTcpServer>

class CogWheelControlChannel;

class CogWheelDataChannel : public QTcpServer
{
    Q_OBJECT

public:
    explicit CogWheelDataChannel(QObject *parent = nullptr);

    bool connectToClient(CogWheelControlChannel *connection);
    void disconnectFromClient(CogWheelControlChannel *connection);

    void listenForConnection(QString serverIP);
    void downloadFile(CogWheelControlChannel *connection, QString fileName);
    void uploadFile(CogWheelControlChannel *connection, QString fileName);

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
    QString transferFileName() const;
    void setTransferFileName(const QString &transferFileName);

protected:
    void incomingConnection(qintptr handle);
    void OnConnected();

signals:
    void uploadFinished();
    void error(QString errorNessage);
    void passiveConnection();
    void finished();

public slots:
    void connected();
    void disconnected();
    void stateChanged(QAbstractSocket::SocketState socketState);
    void bytesWritten(qint64 numBytes);
    void readyRead();
    void socketError(QAbstractSocket::SocketError socketError);

public:
    QTcpSocket *m_dataChannelSocket;

private:
    QHostAddress m_clientHostIP;    // Address of client
    quint16 m_clientHostPort;       // Port used on client
    bool m_connected=false;         // == true data channel connected
    bool m_listening=false;         // == true listening on data channel
    bool m_fileBeingUploaded=false; // == true file being uploaed
    QString m_transferFileName;     // == upload/download file name

};

#endif // COGWHEELDATACHANNEL_H
