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

#include "cogwheel.h"

#include <QSettings>
#include <QFile>

class CogWheelServerSettings
{

public:

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

private:

    QString m_serverName;                                    //  Server Name
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
    quint64 m_connectionListUpdateTime=kCWConnListUpdateTime;// Connection list update timer
    QByteArray m_serverPrivateKey;                           // Server private key
    QByteArray m_serverCert;                                 // Server Certificate

};
#endif // COGWHEELSERVERSETTINGS_H
