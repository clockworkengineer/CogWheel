/*
 * File:   cogwheelcontrolchannel.h
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

//
// Class: CogWheelControlChannel
//
// Description: Class to provide FTP server control channel functionality
// for the receiving of commands to be processed and the sending of any reponses.
// It passes any commands to the FTP core object to be processed and also
// creates/destroys the data channel as and when it is needed.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheel.h"
#include "cogwheeldatachannel.h"
#include "cogwheelserversettings.h"

#include <QObject>
#include <QSslSocket>
#include <QSslCertificate>
#include <QSslKey>
#include <QThread>
#include <QHostInfo>

// =================
// CLASS DECLARATION
// =================

class CogWheelControlChannel : public QObject
{
    Q_OBJECT

public:

    // Class exception

    struct Exception : public std::runtime_error {

        Exception(const QString & messageStr)
            : std::runtime_error(static_cast<QString>("CogWheelControlChannel Failure: " + messageStr).toStdString()) {
        }

    };

    // Constructor / Destructor

    explicit CogWheelControlChannel(CogWheelServerSettings serverSettings, QObject *parent = nullptr);
    ~CogWheelControlChannel();

    // Data channel creation / destrucion

    void createDataChannel();
    void tearDownDataChannel();

    // Data channel functions

    bool connectDataChannel();
    void uploadFileToDataChannel(const QString &file);
    void disconnectDataChannel();
    void setHostPortForDataChannel(const QStringList &ipAddressAndPort);
    void downloadFileFromDataChannel(const QString &file);
    void listenForConnectionOnDataChannel();
    void abortOnDataChannel();
    void sendOnDataChannel(const QByteArray &dataToSend);

    // Control channel send response code+message and data functions

    void sendReplyCode(quint16 replyCode, const QString &message);
    void sendReplyCode(quint16 replyCode);
    void sendOnControlChannel(const QString &dataToSend);

    // TLS support

    void enbleTLSSupport();

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
    QSslSocket *controlChannelSocket() const;
    void setControlChannelSocket(QSslSocket *controlChannelSocket);
    qintptr socketHandle() const;
    void setSocketHandle(const qintptr &socketHandle);
    QString rootDirectory() const;
    void setRootDirectory(const QString &rootDirectory);
    QString accountName() const;
    void setAccountName(const QString &accountName);
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
    qint64 serverWriteBytesSize() const;
    void setServerWriteBytesSize(const qint64 &serverWriteBytesSize);
    bool writeAccess() const;
    void setWriteAccess(bool writeAccess);
    bool IsSslConnection() const;
    void setSslConnection(bool sslConnection);
    QChar dataChanelProtection() const;
    void setDataChanelProtection(const QChar &dataChanelProtection);
    QByteArray serverPrivateKey() const;
    void setServerPrivateKey(const QByteArray &serverPrivateKey);
    QByteArray serverCert() const;
    void setServerCert(const QByteArray &serverCert);
    bool serverEnabled() const;
    void setServerEnabled(bool serverEnabled);
    QString serverGlobalIP() const;
    void setServerGlobalIP(const QString &serverGlobalIP);
    quint64 serverPassivePortLow() const;
    void setServerPassivePortLow(const quint64 &serverPassivePortLow);
    quint64 serverPassivePortHigh() const;
    void setServerPassivePortHigh(const quint64 &serverPassivePortHigh);

private:

    // Process FTP command

    void processFTPCommand(QString commandLine);

    // Passive port allocation/deallocation.

    quint64 getPassivePort();
    void removePassivePort(quint64 passivePort);

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

    // Control channel socket

    void connected();
    void disconnected();
    void readyRead();
    void bytesWritten(qint64 numberOfBytes);

    // TLS/SSL specific

    void sslError(QList<QSslError> errors);
    void controlChannelEncrypted();

private:

    QString m_userName;                 // Login user name
    QString m_password;                 // User password
    QString m_currentWorkingDirectory;  // Current working directory
    bool m_connected=false;             // == true then control channel connected
    bool m_passive=false;               // == true then passive data connection
    bool m_authorized=false;            // == true then user has been authorised
    bool m_anonymous=false;             // == true then anonymous login
    bool m_writeAccess=false;           // == true then user has write access
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
    QChar m_dataChanelProtection='C';   // Data channel protecion level

    qint64 m_serverWriteBytesSize=0;    // Number of bytes per write
    QByteArray m_serverPrivateKey;      // Server private key
    QByteArray m_serverCert;            // Server Certificate
    bool m_serverEnabled=false;         // == true Server enabled
    QString m_serverGlobalIP;           // Server IP Address outside of NAT
    quint64 m_serverPassivePortLow=0;   // Passive port low range
    quint64 m_serverPassivePortHigh=0;  // Passive port High range

    QThread *m_connectionThread=nullptr;            // Connection thread
    QSslSocket *m_controlChannelSocket=nullptr;     // Control channel socket
    CogWheelDataChannel *m_dataChannel=nullptr;     // Data channel
    QString m_readBuffer;                           // Control channel read buffer
    qintptr m_socketHandle;                         // Control channel socket handle
    bool m_sslConnection=false;                     // == true connection is SSL

    static QSet<quint64> passivePortMap;      // Currently active passive ports

};

#endif // COGWHEELCONTROLCHANNEL_H
