/*
 * File:   cogwheelserversettings.h
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

#ifndef COGWHEELSERVERSETTINGS_H
#define COGWHEELSERVERSETTINGS_H

//
// Class: CogWheelServerSettings
//
// Description: Class to load and save the server settings.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheel.h"

#include <QSettings>
#include <QFile>

// =================
// CLASS DECLARATION
// =================

class CogWheelServerSettings
{

public:

    // Class exception

    struct Exception : public std::runtime_error {

        Exception(const QString & messageStr)
            : std::runtime_error(static_cast<QString>("CogWheelServerSettings Failure: " + messageStr).toStdString()) {
        }

    };

    // Load and save settings

    void load();
    void save();

    // Load server private key and certificate

    bool loadPrivateKeyAndCert();

    // Private data accessors

    QString serverName() const;
    void setServerName(const QString &serverName);
    QString serverVersion() const;
    void setServerVersion(const QString &serverVersion);
    bool serverAllowSMNT() const;
    void setServerAllowSMNT(bool serverAllowSMNT);
    quint64 serverPort() const;
    void setServerPort(const quint64 &serverPort);
    quint64 serverWriteBytesSize() const;
    void setServerWriteBytesSize(const quint64 &serverWriteBytesSize);
    bool serverEnabled() const;
    void setServerEnabled(bool serverEnabled);
    bool serverSslEnabled() const;
    void setServerSslEnabled(bool serverSslEnabled);
    bool serverPlainFTPEnabled() const;
    void setServerPlainFTPEnabled(bool serverPlainFTPEnabled);
    QByteArray serverPrivateKey() const;
    void setServerPrivateKey(const QByteArray &serverPrivateKey);
    QByteArray serverCert() const;
    void setServerCert(const QByteArray &serverCert);
    QString serverKeyFileName() const;
    void setServerKeyFileName(const QString &serverKeyFileName);
    QString serverCertFileName() const;
    void setServerCertFileName(const QString &serverCertFileName);
    bool serverAnonymousEnabled() const;
    void setServerAnonymousEnabled(bool serverAnonymousEnabled);
    quint64 connectionListUpdateTime() const;
    void setConnectionListUpdateTime(const quint64 &connectionListUpdateTime);
    bool serverLoggingEnabled() const;
    void setServerLoggingEnabled(bool serverLoggingEnabled);
    QString serverLoggingFileName() const;
    void setServerLoggingFileName(const QString &serverLoggingFileName);
    QStringList serverLoggingLevels() const;
    void setServerLoggingLevels(const QStringList &serverLoggingLevels);
    QString serverGlobalName() const;
    void setServerGlobalName(const QString &serverGlobalName);
    quint64 serverPassivePortLow() const;
    void setServerPassivePortLow(const quint64 &serverPassivePortLow);
    quint64 serverPassivePortHigh() const;
    void setServerPassivePortHigh(const quint64 &serverPassivePortHigh);

private:

    QString m_serverName;                                    // Server Name
    QString m_serverVersion;                                 // Server Version
    bool m_serverAllowSMNT=false;                            // ==true allow SMNT command
    quint64 m_serverPort;                                    // Server connection port
    quint64 m_serverWriteBytesSize=kCWWriteBytesSize;        // No of bytes per write
    bool m_serverEnabled=false;                              // ==true Server enabled
    bool m_serverSslEnabled=false;                           // ==true TLS/SSL enabled
    bool m_servePlainFTPEnabled=false;                       // ==true Plain insecure FTP enabled
    bool m_serverAnonymousEnabled=false;                     // == true anonymous login enabled
    QString m_serverKeyFileName;                             // Server private key file name
    QString m_serverCertFileName;                            // Server certifucate file
    QString m_serverGlobalName;                              // Address of server outside NAT
    quint64 m_serverPassivePortLow=0;                        // Passive port low range
    quint64 m_serverPassivePortHigh=0;                       // Passice port high range

    quint64 m_connectionListUpdateTime=kCWConnListUpdateTime;// Connection list update timer
    bool m_serverLoggingEnabled=false;                       // == true logging enabled
    QStringList m_serverLoggingLevels;                       // Server logging levels
    QString m_serverLoggingFileName;                         // Name of file to which logging output goes
    QByteArray m_serverPrivateKey;                           // Server private key
    QByteArray m_serverCert;                                 // Server Certificate

};
#endif // COGWHEELSERVERSETTINGS_H
