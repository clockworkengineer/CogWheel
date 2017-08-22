/*
 * File:   cogwheelusersettings.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

//
// Class: CogWheelUserSettings
//
// Description: Class to load and save the user settings. It also
// validates a user exists and authenticates their password.
//
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelusersettings.h"

#include <QCryptographicHash>

/**
 * @brief CogWheelUserSettings::checkUserName
 * @param userName
 * @return
 */
bool CogWheelUserSettings::checkUserName(const QString& userName)
{
      QSettings  userList;
      QStringList userNames;

      userList.beginGroup("UserList");
      userNames = userList.value("users").toStringList();
      userList.endGroup();

      return(!userNames.empty() && userNames.contains(userName));

}

/**
 * @brief CogWheelUserSettings::checkUserPassword
 * @param hashedPasswords
 * @param password
 * @return
 */
bool CogWheelUserSettings::checkUserPassword(const QString& hashedPassword, const QString& password)
{

    QByteArray paswordHash;
    paswordHash = QCryptographicHash::hash(password.toUtf8(),QCryptographicHash::Sha1 );
    return(hashedPassword==paswordHash.toHex());

}

/**
 * @brief CogWheelUserSettings::loadUserSettings
 * @param userName
 */
void CogWheelUserSettings::load(QString userName)
{

    QSettings  userSettings;

    userSettings.beginGroup(userName);
    m_userName = userName;
    m_userPassword = userSettings.value("password").toString();
    m_rootPath = userSettings.value("root").toString();
    m_accountName = userSettings.value("account").toString();
    m_enabled=userSettings.value("enabled").toBool();
    m_writeAccess=userSettings.value("writeaccess").toBool();
    userSettings.endGroup();

}

/**
 * @brief CogWheelUserSettings::saveUserSettings
 * @param userName
 */
void CogWheelUserSettings::save(QString userName)
{
    Q_UNUSED(userName);
}

// ============================
// CLASS PRIVATE DATA ACCESSORS
// ============================

QString CogWheelUserSettings::getUserName() const
{
    return m_userName;
}

void CogWheelUserSettings::setUserName(const QString &userName)
{
    m_userName = userName;
}

QString CogWheelUserSettings::getUserPassword() const
{
    return m_userPassword;
}

void CogWheelUserSettings::setUserPassword(const QString &userPassword)
{
    m_userPassword = userPassword;
}

QString CogWheelUserSettings::getAccountName() const
{
    return m_accountName;
}

void CogWheelUserSettings::setAccountName(const QString &accountName)
{
    m_accountName = accountName;
}

bool CogWheelUserSettings::getEnabled() const
{
    return m_enabled;
}

void CogWheelUserSettings::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

bool CogWheelUserSettings::getWriteAccess() const
{
    return m_writeAccess;
}

void CogWheelUserSettings::setWriteAccess(bool writeAccess)
{
    m_writeAccess = writeAccess;
}

QString CogWheelUserSettings::getRootPath() const
{
    return m_rootPath;
}

void CogWheelUserSettings::setRootPath(const QString &rootPath)
{
    m_rootPath = rootPath;
}
