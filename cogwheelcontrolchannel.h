#ifndef COGWHEELCONNECTION_H
#define COGWHEELCONNECTION_H

#include "cogwheeldatachannel.h"

#include <QObject>
#include <QTcpSocket>
#include <QThread>

class CogWheelControlChannel : public QObject
{
    Q_OBJECT

public:

    explicit CogWheelControlChannel(QObject *parent = nullptr);

    void createDataChannel();
    void tearDownDataChannel();

    bool connectDataChannel();
    void uploadFileToDataChannel(const QString &file);
    void disconnectDataChannel();
    void setHostPortForDataChannel(QStringList ipAddressAndPort);
    void downloadFileFromDataChannel(const QString &file);
    void listenForConnectionOnDataChannel();
    void abortOnDataChannel();

    void sendOnDataChannel(const QString &data);

    void sendReplyCode(quint16 replyCode, const QString &message);
    void sendReplyCode(quint16 replyCode);

    void sendOnControlChannel(const QString &data);

    QString password() const;
    void setPassword(const QString &password);
    QString userName() const;
    void setUserName(const QString &userName);
    QString currentWorkingDirectory() const;
    void setCurrentWorkingDirectory(const QString &currentWorkingDirectory);
    bool isPassive() const;
    void setPassive(bool isPassive);
    bool isAuthorized() const;
    void setAuthorized(bool isAuthorized);
    bool isAnonymous() const;
    void setAnonymous(bool isAnonymous);
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
    bool isAllowSMNT() const;
    void setAllowSMNT(bool isAllowSMNT);
    QString renameFromFileName() const;
    void setRenameFromFileName(const QString &value);
    qint64 restoreFilePostion() const;
    void setRestoreFilePostion(const qint64 &restoreFilePostion);
    QString clientHostIP() const;
    void setClientHostIP(const QString &clientHostIP);
    QString serverIP() const;
    void setServerIP(const QString &serverIP);
    bool isConnected() const;
    void setConnected(bool isConnected);
    QChar transferMode() const;
    void setTransferMode(const QChar &transferMode);
    QChar fileStructure() const;
    void setFileStructure(const QChar &fileStructure);
    QChar transferType() const;
    void setTransferType(const QChar &transferType);

    CogWheelDataChannel *dataChannel() const;
    void setDataChannel(CogWheelDataChannel *dataChannel);

private:
    void processFTPCommand(QString commandLine);

signals:
    void finishedConnection(qint64 socketHandle);
    void abortedConnection(qint64 socketHandle);

public slots:
    void openConnection(qint64 socketHandle);
    void closeConnection();

    void uploadFinished();
    void error(QString errorNessage);
    void passiveConnection();

    void connected();
    void disconnected();
    void readyRead();
    void bytesWritten(qint64 numberOfBytes);

private:

    QString m_userName;
    QString m_password;
    QString m_currentWorkingDirectory;
    bool m_connected=false;
    bool m_passive=false;
    bool m_authorized=false;
    bool m_anonymous=false;
    bool m_allowSMNT=false;
    QString m_rootDirectory;
    QString m_accountName;
    QString m_clientHostIP;
    QString m_serverIP;
    QChar m_transferMode = 'S';
    QChar m_fileStructure = 'F';
    QChar m_transferType = 'I';
    qint64 m_restoreFilePostion=0;
    QString m_renameFromFileName;

    QThread *m_connectionThread=nullptr;
    QTcpSocket *m_controlChannelSocket=nullptr;
    CogWheelDataChannel *m_dataChannel=nullptr;
    QString m_readBufer;
    qintptr m_socketHandle;

};

#endif // COGWHEELCONNECTION_H
