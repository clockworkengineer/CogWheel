#ifndef COGWHEELDATACHANNEL_H
#define COGWHEELDATACHANNEL_H

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QHostAddress>
#include <QTcpServer>

class CogWheelConnection;

class CogWheelDataChannel : public QTcpServer
{
    Q_OBJECT

public:
    explicit CogWheelDataChannel(QObject *parent = nullptr);

    bool connectToClient(CogWheelConnection *connection);
    void disconnectFromClient(CogWheelConnection *connection);

    void setClientHostIP(QString clientIP);
    void setClientHostPort(quint16 clientPort);
    QHostAddress clientHostIP() const;
    quint16 clientHostPort() const;

    void listenForConnection();

    void downloadFile(CogWheelConnection *connection, QString fileName);
    void uploadFile(CogWheelConnection *connection, QString fileName);

protected:
    void incomingConnection(qintptr handle);
    void OnConnected();

signals:
    void uploadFinished();
    void dataChannelSocketError(QAbstractSocket::SocketError socketError);
    void dataChannelError(QString errorNessage);
    void passiveConnection();

public slots:

    void connected();
    void disconnected();
    void stateChanged(QAbstractSocket::SocketState socketState);
    void bytesWritten(qint64 numBytes);
    void readyRead();
    void error(QAbstractSocket::SocketError socketError);

public:
    QTcpSocket *m_dataChannelSocket;

private:
    QHostAddress m_clientHostIP;
    quint16 m_clientHostPort;
    bool m_connected=false;
    bool m_fileBeingUploaded=false;
    QString m_uploadFileName;

};

#endif // COGWHEELDATACHANNEL_H
