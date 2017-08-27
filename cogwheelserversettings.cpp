/*
 * File:   cogwheelserversettings.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

//
// Class: CogWheelServerSettings
//
// Description: Class to load and save the server settings.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelserversettings.h"
#include <QDebug>

/**
 * @brief CogWheelServerSettings::load
 *
 * Load server settings. If none are found then create some defaults
 * before loading them.
 *
 */
void CogWheelServerSettings::load()
{

    QSettings server;

    server.beginGroup("Server");
    if (!server.childKeys().contains("name")) {
        server.setValue("name", "CogWheel");
    }
    if (!server.childKeys().contains("version")) {
        server.setValue("version", "0.5");
    }
    if (!server.childKeys().contains("port")) {
        server.setValue("port", 2221);
    }
    if (!server.childKeys().contains("allowSMNT")) {
        server.setValue("allowSMNT", false);
    }
    if (!server.childKeys().contains("writesize")) {
        server.setValue("writesize", 32*1024);
    }
    if (!server.childKeys().contains("enabled")) {
        server.setValue("enabled", true);
    }
    if (!server.childKeys().contains("sslenabled")) {
        server.setValue("sslenabled", true);
    }
    if (!server.childKeys().contains("plain")) {
        server.setValue("plains", true);
    }
    server.endGroup();

    server.beginGroup("Server");
    setServerName(server.value("name").toString());
    setServerVersion(server.value("version").toString());
    setServerPort(server.value("port").toInt());
    setServerAllowSMNT(server.value("allowSMNT").toBool());
    setServerWriteBytesSize(server.value("writesize").toInt());
    setServerEnabled(server.value("enabled").toBool());
    setServerSslEnabled(server.value("sslenabled").toBool());
    setServerPlainFTPEnabled(server.value("plain").toBool());
    server.endGroup();

    setServerKeyFileName("./server.key");   // Hard encoded for the moment.
    setServerCertFileName("./server.crt");

}

/**
 * @brief CogWheelServerSettings::save
 *
 * Save server settings (TO BE IMLEMENTED).
 *
 */
void CogWheelServerSettings::save()
{

    QSettings server;

    server.beginGroup("Server");
    server.setValue("name", serverName());
    server.setValue("version",serverVersion());
    server.setValue("port", serverPort());
    server.setValue("allowSMNT", serverAllowSMNT());
    server.setValue("writesize", serverWriteBytesSize());
    server.setValue("enabled", serverEnabled());
    server.setValue("sslenabled",serverSslEnabled());
    server.setValue("plains", serverPlainFTPEnabled());
    server.endGroup();

}

bool CogWheelServerSettings::loadPrivateKeyAndCert()
{

    // Read server key

    QFile serverKeyFile(serverKeyFileName());
    if(serverKeyFile.open(QIODevice::ReadOnly)){
        m_serverPrivateKey = serverKeyFile.readAll();
        serverKeyFile.close();
    } else {
       qDebug() << "Error opening file " << serverKeyFileName() << ": " << serverKeyFile.errorString();
       return(false);
    }

    // Read server cert

    QFile serveCertFile(serverCertFileName());
    if(serveCertFile.open(QIODevice::ReadOnly)){
        m_serverCert = serveCertFile.readAll();
        serveCertFile.close();
    } else {
       qDebug() << "Error opening file " << serverCertFileName() << ": " << serveCertFile.errorString();
       return(false);
    }

    return(true);

}

// ============================
// CLASS PRIVATE DATA ACCESSORS
// ============================

/**
 * @brief CogWheelServerSettings::serverName
 * @return
 */
QString CogWheelServerSettings::serverName() const
{
    return m_serverName;
}

/**
 * @brief CogWheelServerSettings::setServerName
 * @param serverName
 */
void CogWheelServerSettings::setServerName(const QString &serverName)
{
    m_serverName = serverName;
}

/**
 * @brief CogWheelServerSettings::serverVersion
 * @return
 */
QString CogWheelServerSettings::serverVersion() const
{
    return m_serverVersion;
}

/**
 * @brief CogWheelServerSettings::setServerVersion
 * @param serverVersion
 */
void CogWheelServerSettings::setServerVersion(const QString &serverVersion)
{
    m_serverVersion = serverVersion;
}

/**
 * @brief CogWheelServerSettings::allowSMNT
 * @return
 */
bool CogWheelServerSettings::serverAllowSMNT() const
{
    return m_serverAllowSMNT;
}

/**
 * @brief CogWheelServerSettings::setAllowSMNT
 * @param allowSMNT
 */
void CogWheelServerSettings::setServerAllowSMNT(bool allowSMNT)
{
    m_serverAllowSMNT = allowSMNT;
}

/**
 * @brief CogWheelServerSettings::serverPort
 * @return
 */
qint64 CogWheelServerSettings::serverPort() const
{
    return m_serverPort;
}

/**
 * @brief CogWheelServerSettings::setServerPort
 * @param serverPort
 */
void CogWheelServerSettings::setServerPort(const qint64 &serverPort)
{
    m_serverPort = serverPort;
}

/**
 * @brief CogWheelServerSettings::writeBytesSize
 * @return
 */
qint64 CogWheelServerSettings::serverWriteBytesSize() const
{
    return m_serverWriteBytesSize;
}

/**
 * @brief CogWheelServerSettings::setWriteBytesSize
 * @param writeBytesSize
 */
void CogWheelServerSettings::setServerWriteBytesSize(const qint64 &writeBytesSize)
{
    m_serverWriteBytesSize = writeBytesSize;
}

/**
 * @brief CogWheelServerSettings::active
 * @return
 */
bool CogWheelServerSettings::serverEnabled() const
{
    return m_serverEnabled;
}

/**
 * @brief CogWheelServerSettings::setActive
 * @param active
 */
void CogWheelServerSettings::setServerEnabled(bool enabled)
{
    m_serverEnabled = enabled;
}

bool CogWheelServerSettings::serverSslEnabled() const
{
    return m_serverSslEnabled;
}

void CogWheelServerSettings::setServerSslEnabled(bool sslEnabled)
{
    m_serverSslEnabled = sslEnabled;
}

bool CogWheelServerSettings::serverPlainFTPEnabled() const
{
    return m_servePlainFTPEnabled;
}

void CogWheelServerSettings::setServerPlainFTPEnabled(bool plainFTPEnabled)
{
    m_servePlainFTPEnabled = plainFTPEnabled;
}

QByteArray CogWheelServerSettings::serverPrivateKey() const
{
    return m_serverPrivateKey;
}

void CogWheelServerSettings::setServerPrivateKey(const QByteArray &serverPrivateKey)
{
    m_serverPrivateKey = serverPrivateKey;
}

QByteArray CogWheelServerSettings::serverCert() const
{
    return m_serverCert;
}

void CogWheelServerSettings::setServerCert(const QByteArray &serverCert)
{
    m_serverCert = serverCert;
}

QString CogWheelServerSettings::serverKeyFileName() const
{
    return m_serverKeyFileName;
}

void CogWheelServerSettings::setServerKeyFileName(const QString &serverKeyFileName)
{
    m_serverKeyFileName = serverKeyFileName;
}

QString CogWheelServerSettings::serverCertFileName() const
{
    return m_serverCertFileName;
}

void CogWheelServerSettings::setServerCertFileName(const QString &serverCertFileName)
{
    m_serverCertFileName = serverCertFileName;
}
