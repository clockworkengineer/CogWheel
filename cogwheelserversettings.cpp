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
// Description:
//
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelserversettings.h"

void CogWheelServerSettings::load()
{

    QSettings server;

    server.beginGroup("Server");
    if (server.childKeys().isEmpty()) {
        server.setValue("name", "CogWheel");
        server.setValue("version", "0.5");
        server.setValue("port", 2221);
        server.setValue("allowSMNT", false);
    }
    server.endGroup();

    server.beginGroup("Server");
    setServerName(server.value("name").toString());
    setServerVersion(server.value("version").toString());
    setServerPort(server.value("port").toInt());
    setAllowSMNT(server.value("allowSMNT").toBool());
    server.endGroup();

}

void CogWheelServerSettings::save()
{

}

QString CogWheelServerSettings::serverName() const
{
    return m_serverName;
}

void CogWheelServerSettings::setServerName(const QString &serverName)
{
    m_serverName = serverName;
}

QString CogWheelServerSettings::serverVersion() const
{
    return m_serverVersion;
}

void CogWheelServerSettings::setServerVersion(const QString &serverVersion)
{
    m_serverVersion = serverVersion;
}

bool CogWheelServerSettings::allowSMNT() const
{
    return m_allowSMNT;
}

void CogWheelServerSettings::setAllowSMNT(bool allowSMNT)
{
    m_allowSMNT = allowSMNT;
}

qint64 CogWheelServerSettings::serverPort() const
{
    return m_serverPort;
}

void CogWheelServerSettings::setServerPort(const qint64 &serverPort)
{
    m_serverPort = serverPort;
}

