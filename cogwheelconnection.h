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

    void sendReplyCode(quint16 replyCode, const QString &message);
    void sendReplyCode(quint16 replyCode);

    void sendOnControlChannel(const QString &data);
    void sendOnDataChannel(const QString &data);

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
    QString rootDirectory() const;
    void setRootDirectory(const QString &rootDirectory);
    QString accountName() const;
    void setAccountName(const QString &accountName);
    bool allowSMNT() const;
    void setAllowSMNT(bool allowSMNT);
    QString renameFromFileName() const;
    void setRenameFromFileName(const QString &value);
    qint64 restoreFilePostion() const;
    void setRestoreFilePostion(const qint64 &restoreFilePostion);
    QString clientHostIP() const;
    void setClientHostIP(const QString &clientHostIP);
    QString serverIP() const;
    void setServerIP(const QString &serverIP);

private:
    void processFTPCommand(QString command);

signals:
    void finishedConnection(qint64 socketHandle);
    void abortedConnection(qint64 socketHandle);

public slots:
    void openConnection(qint64 socketHandle);
    void closeConnection();
    void uploadFinished();
    void dataChannelError(QString errorNessage);
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
    bool m_allowSMNT=false;
    QString m_rootDirectory;
    QString m_accountName;
    QString m_clientHostIP;
    QString m_serverIP;

    qint64 m_restoreFilePostion=0;
    QString m_renameFromFileName;

    QThread *m_connectionThread;
    QTcpSocket *m_controlChannelSocket;
    CogWheelDataChannel *m_dataChannel;
    QString m_readBufer;
    qintptr m_socketHandle;

};

#endif // COGWHEELCONNECTION_H
