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
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelserversettings.h"

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
    if (!server.childKeys().contains("active")) {
        server.setValue("active", true);
    }
    server.endGroup();

    server.beginGroup("Server");
    setServerName(server.value("name").toString());
    setServerVersion(server.value("version").toString());
    setServerPort(server.value("port").toInt());
    setAllowSMNT(server.value("allowSMNT").toBool());
    setWriteBytesSize(server.value("writesize").toInt());
    setActive(server.value("active").toBool());
    server.endGroup();

}

/**
 * @brief CogWheelServerSettings::save
 *
 * Save server settings (TO BE IMLEMENTED).
 *
 */
void CogWheelServerSettings::save()
{

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
bool CogWheelServerSettings::allowSMNT() const
{
    return m_allowSMNT;
}

/**
 * @brief CogWheelServerSettings::setAllowSMNT
 * @param allowSMNT
 */
void CogWheelServerSettings::setAllowSMNT(bool allowSMNT)
{
    m_allowSMNT = allowSMNT;
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

qint64 CogWheelServerSettings::writeBytesSize() const
{
    return m_writeBytesSize;
}

void CogWheelServerSettings::setWriteBytesSize(const qint64 &writeBytesSize)
{
    m_writeBytesSize = writeBytesSize;
}

bool CogWheelServerSettings::active() const
{
    return m_active;
}

void CogWheelServerSettings::setActive(bool active)
{
    m_active = active;
}
