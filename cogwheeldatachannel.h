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
    QHostAddress m_clientHostIP;
    quint16 m_clientHostPort;
    bool m_connected=false;
    bool m_listening=false;
    bool m_fileBeingUploaded=false;
    QString m_transferFileName;

};

#endif // COGWHEELDATACHANNEL_H
