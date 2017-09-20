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

// =============
// INCLUDE FILES
// =============

#include "cogwheelusersettings.h"
#include "cogwheellogger.h"

#include <QCryptographicHash>

// ====================
// CLASS IMPLEMENTATION
// ====================

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
 * @brief CogWheelUserSettings::load
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
 * @brief CogWheelUserSettings::save
 * @param userName
 */
void CogWheelUserSettings::save(QString userName)
{
    QSettings userSettings;

    userSettings.beginGroup(userName);

    // Encrypt the password using SHA1

    if (!m_userPassword.isEmpty()) {
        QByteArray passHash = QCryptographicHash::hash(m_userPassword.toUtf8(),QCryptographicHash::Sha1 );
        QString passHashString(passHash.toHex());
        userSettings.setValue("password", passHashString);
    }
    userSettings.setValue("root", m_rootPath);
    userSettings.setValue("account",m_accountName);
    userSettings.setValue("enabled", m_enabled);
    userSettings.setValue("writeaccess", m_writeAccess);
    userSettings.endGroup();

}

// ============================
// CLASS PRIVATE DATA ACCESSORS
// ============================

/**
 * @brief CogWheelUserSettings::getUserName
 * @return
 */
QString CogWheelUserSettings::getUserName() const
{
    return m_userName;
}

/**
 * @brief CogWheelUserSettings::setUserName
 * @param userName
 */
void CogWheelUserSettings::setUserName(const QString &userName)
{
    m_userName = userName;
}

/**
 * @brief CogWheelUserSettings::getUserPassword
 * @return
 */
QString CogWheelUserSettings::getUserPassword() const
{
    return m_userPassword;
}

/**
 * @brief CogWheelUserSettings::setUserPassword
 * @param userPassword
 */
void CogWheelUserSettings::setUserPassword(const QString &userPassword)
{
    m_userPassword = userPassword;
}

/**
 * @brief CogWheelUserSettings::getAccountName
 * @return
 */
QString CogWheelUserSettings::getAccountName() const
{
    return m_accountName;
}

/**
 * @brief CogWheelUserSettings::setAccountName
 * @param accountName
 */
void CogWheelUserSettings::setAccountName(const QString &accountName)
{
    m_accountName = accountName;
}

/**
 * @brief CogWheelUserSettings::getEnabled
 * @return
 */
bool CogWheelUserSettings::getEnabled() const
{
    return m_enabled;
}

/**
 * @brief CogWheelUserSettings::setEnabled
 * @param enabled
 */
void CogWheelUserSettings::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

/**
 * @brief CogWheelUserSettings::getWriteAccess
 * @return
 */
bool CogWheelUserSettings::getWriteAccess() const
{
    return m_writeAccess;
}

/**
 * @brief CogWheelUserSettings::setWriteAccess
 * @param writeAccess
 */
void CogWheelUserSettings::setWriteAccess(bool writeAccess)
{
    m_writeAccess = writeAccess;
}

/**
 * @brief CogWheelUserSettings::getRootPath
 * @return
 */
QString CogWheelUserSettings::getRootPath() const
{
    return m_rootPath;
}

/**
 * @brief CogWheelUserSettings::setRootPath
 * @param rootPath
 */
void CogWheelUserSettings::setRootPath(const QString &rootPath)
{
    m_rootPath = rootPath;
}
