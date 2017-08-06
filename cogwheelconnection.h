#ifndef COGWHEELCONNECTION_H
#define COGWHEELCONNECTION_H

#include "cogwheeldatachannel.h"

#include <QObject>
#include <QTcpSocket>
#include <QThread>

class CogWheelConnection : public QObject
{
    Q_OBJECT

public:
    explicit CogWheelConnection(QObject *parent = nullptr);

    void sendReplyCode(quint16 replyCode, QString message);
    void sendReplyCode(quint16 replyCode);

    void sendOnDataChannel(QString data);

private:
    void processFTPCommand(QString command);

signals:
    void finished(qint64 socketHandle);
    void aborted(qint64 socketHandle);

public slots:
    void open(qint64 socketHandle);
    void close();

    void connected();
    void disconnected();
    void readyRead();
    void bytesWritten(qint64 numberOfBytes);

public:

    QThread m_connectionThread;

    QTcpSocket *m_controlChannelSocket;
    CogWheelDataChannel m_dataChannel;

    QString m_user;
    QString m_password;
    QString m_currentWorkingDirectory;

    qintptr m_socketHandle;

    bool m_passive;


};

#endif // COGWHEELCONNECTION_H
