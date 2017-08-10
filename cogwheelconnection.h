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

    QString password() const;
    void setPassword(const QString &password);
    QString userName() const;
    void setUserName(const QString &userName);
    QString currentWorkingDirectory() const;
    void setCurrentWorkingDirectory(const QString &currentWorkingDirectory);
    bool passive() const;
    void setPassive(bool passive);
    bool authorized() const;
    void setAuthorized(bool authorized);
    bool anonymous() const;
    void setAnonymous(bool anonymous);
    CogWheelDataChannel *dataChannel() const;
    void setDataChannel(CogWheelDataChannel *dataChannel);
    QThread *connectionThread() const;
    void setConnectionThread(QThread *connectionThread);
    QTcpSocket *controlChannelSocket() const;
    void setControlChannelSocket(QTcpSocket *controlChannelSocket);
    qintptr socketHandle() const;
    void setSocketHandle(const qintptr &socketHandle);

private:
    void processFTPCommand(QString command);

signals:
    void finishedConnection(qint64 socketHandle);
    void abortedConnection(qint64 socketHandle);

public slots:
    void openConnection(qint64 socketHandle);
    void closeConnection();
    void uploadFinished();
    void dataChannelSocketError(QAbstractSocket::SocketError socketError);
    void passiveConnection();

    void connected();
    void disconnected();
    void readyRead();
    void bytesWritten(qint64 numberOfBytes);


private:

    QString m_userName;
    QString m_password;
    QString m_currentWorkingDirectory;
    bool m_passive=false;
    bool m_authorized=false;
    bool m_anonymous=false;

    QThread *m_connectionThread;
    QTcpSocket *m_controlChannelSocket;
    CogWheelDataChannel *m_dataChannel;
    QString m_readBufer;
    qintptr m_socketHandle;

};

#endif // COGWHEELCONNECTION_H
