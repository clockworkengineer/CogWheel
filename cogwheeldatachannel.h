#ifndef COGWHEELDATACHANNEL_H
#define COGWHEELDATACHANNEL_H

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QHostAddress>

class CogWheelDataChannel : public QObject
{
    Q_OBJECT

public:
    explicit CogWheelDataChannel(QObject *parent = nullptr);

    void connectToClient();
    void disconnectFromClient();
    void setClientHostIP(QString clientIP);
    void setClientHostPort(quint16 clientPort);

signals:

public slots:

    void connected();
    void disconnected();
    void stateChanged(QAbstractSocket::SocketState socketState);
    void bytesWritten(qint64 numBytes);
    void readyRead();

public:
    QTcpSocket *m_dataChannelSocket;

private:
    QHostAddress m_clientHostIP;
    quint16 m_clientHostPort;
    bool m_connected=false;

};

#endif // COGWHEELDATACHANNEL_H
