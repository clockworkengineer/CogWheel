/*
 * File:   cogwheelcontrolchannel.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

#ifndef COGWHEELCONTROLCHANNEL_H
#define COGWHEELCONTROLCHANNEL_H

#include "cogwheeldatachannel.h"
#include "cogwheelserversettings.h"

#include <QObject>
#include <QTcpSocket>
#include <QThread>

class CogWheelControlChannel : public QObject
{
    Q_OBJECT

public:

    // Constructor

    explicit CogWheelControlChannel(CogWheelServerSettings serverSettings, QObject *parent = nullptr);

    // Data channel creation/destrucion

    void createDataChannel();
    void tearDownDataChannel();

    // Data channel control

    bool connectDataChannel();
    void uploadFileToDataChannel(const QString &file);
    void disconnectDataChannel();
    void setHostPortForDataChannel(const QStringList &ipAddressAndPort);
    void downloadFileFromDataChannel(const QString &file);
    void listenForConnectionOnDataChannel();
    void abortOnDataChannel();

    // Data channel send data

    void sendOnDataChannel(const QByteArray &dataToSend);

    // Control channel send response and data

    void sendReplyCode(quint16 replyCode, const QString &message);
    void sendReplyCode(quint16 replyCode);
    void sendOnControlChannel(const QString &dataToSend);

    // Private data accessors

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
    QChar transferTypeFormat() const;
    void setTransferTypeFormat(const QChar &transferTypeFormat);
    qint16 transTypeByteSize() const;
    void setTransTypeByteSize(const qint16 &transTypeByteSize);
    QString serverName() const;
    void setServerName(const QString &serverName);
    QString serverVersion() const;
    void setServerVersion(const QString &serverVersion);
    qint64 writeBytesSize() const;
    void setWriteBytesSize(const qint64 &writeBytesSize);

private:
    void processFTPCommand(QString commandLine);    // Process FTP command

signals:

    // Control channel

    void finishedConnection(qint64 socketHandle);
    void abortedConnection(qint64 socketHandle);

public slots:

    // Control channel

    void openConnection(qint64 socketHandle);
    void closeConnection();

    // Data channel

    void transferFinished();            // File transfer finished
    void passiveConnection();           // Passive connection

    // Errors & information messages

    void error(const QString &errorNessage);  // Channel error
    void info(const QString &message);        // Information message

    // Control channel socket

    void connected();
    void disconnected();
    void readyRead();
    void bytesWritten(qint64 numberOfBytes);

private:

    QString m_userName;                 // Login user name
    QString m_password;                 // User password
    QString m_currentWorkingDirectory;  // Current working directory
    bool m_connected=false;             // == true then control channel connected
    bool m_passive=false;               // == true then passive data connection
    bool m_authorized=false;            // == true then user has been authorised
    bool m_anonymous=false;             // == true then anonymous login
    QString m_rootDirectory;            // Root directory
    QString m_accountName;              // Account name
    QString m_clientHostIP;             // Client host IP Address
    QString m_serverIP;                 // Server IP Address
    QChar m_transferMode = 'S';         // Transfrer mode
    QChar m_fileStructure = 'F';        // File Structure
    QChar m_transferType = 'A';         // Transfer type
    QChar m_transferTypeFormat = 'N';   // Transfer format
    qint16 m_transTypeByteSize = 8;     // Transfer byte size
    qint64 m_restoreFilePostion=0;      // File restore position in bytes
    QString m_renameFromFileName;       // RNFR/RNTO file name

    QString m_serverName;               // Server name
    QString m_serverVersion;            // Server version
    bool m_allowSMNT=false;             // == true allow SMNT command
    qint64 m_writeBytesSize=0;          // Number of bytes per write

    QThread *m_connectionThread=nullptr;            // Connection thread
    QTcpSocket *m_controlChannelSocket=nullptr;     // Control channel socket
    CogWheelDataChannel *m_dataChannel=nullptr;     // Data channel
    QString m_readBufer;                            // Control channel read buffer
    qintptr m_socketHandle;                         // Control channel socket handle

};

#endif // COGWHEELCONTROLCHANNEL_H
