#ifndef COGWHEELCONNECTION_H
#define COGWHEELCONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include "cogwheeldatachannel.h"

class CogWheelConnection : public QObject
{
    Q_OBJECT

public:
    explicit CogWheelConnection(qintptr socketHandle, QObject *parent = nullptr);
    void start();
    void stop();
    void sendReply(quint16 replyCode, QString message);
    void sendReply(quint16 replyCode);
    void sendData(QString reponse);

private:
    void processFTPCommand(QString command);

signals:
    void close();

public slots:
    void connected();
    void disconnected();
    void readyRead();
    void bytesWritten(qint64 numberOfBytes);

public:

    QTcpSocket *m_controlChannelSocket;
    CogWheelDataChannel m_dataChannel;

    QString m_user;
    QString m_password;
    QString m_currentWorkingDirectory;

    qintptr m_socketHandle;

    bool m_passive;


};

#endif // COGWHEELCONNECTION_H
