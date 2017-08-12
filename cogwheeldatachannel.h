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

    void listenForConnection(QString serverIP);
    void downloadFile(CogWheelConnection *connection, QString fileName);
    void uploadFile(CogWheelConnection *connection, QString fileName);

    void setClientHostIP(QString clientIP);
    void setClientHostPort(quint16 clientPort);
    bool listening() const;
    void setListening(bool listening);
    bool connected() const;
    void setConnected(bool connected);


    QHostAddress clientHostIP() const;
    quint16 clientHostPort() const;

protected:
    void incomingConnection(qintptr handle);
    void OnConnected();

signals:
    void dataChannelUploadFinished();
    void dataChannelError(QString errorNessage);
    void dataChannelPassiveConnection();
    void dataChannelFinished();

public slots:

    void dataChannelConnect();
    void dataChannelDisconnect();
    void dataChannelStateChanged(QAbstractSocket::SocketState socketState);
    void dataChannelBytesWritten(qint64 numBytes);
    void dataChannelReadyRead();
    void dataChannelSocketError(QAbstractSocket::SocketError socketError);

public:
    QTcpSocket *m_dataChannelSocket;


private:
    QHostAddress m_clientHostIP;
    quint16 m_clientHostPort;
    bool m_connected=false;
    bool m_listening=false;
    bool m_fileBeingUploaded=false;
    QString m_transferFileName;

};

#endif // COGWHEELDATACHANNEL_H
