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
// Description: Class to validate users and to read in their settings
// if they exist.
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
 * @param userName
 * @param password
 * @return
 */
bool CogWheelUserSettings::checkUserPassword(const QString& userName, const QString& password)
{

    QSettings  userSettings;
    QString hashedPassword;
    QByteArray paswordHash;

    userSettings.beginGroup(userName);
    paswordHash = QCryptographicHash::hash(password.toUtf8(),QCryptographicHash::Sha1 );
    hashedPassword = userSettings.value("password").toString();
    userSettings.endGroup();

    return(hashedPassword==paswordHash.toHex());

}

/**
 * @brief CogWheelUserSettings::getRootPath
 * @param userName
 * @return
 */
QString CogWheelUserSettings::getRootPath(const QString &userName)
{
    QSettings  userSettings;
    QString rootPath;

    userSettings.beginGroup(userName);
    rootPath = userSettings.value("root").toString();
    userSettings.endGroup();

    return(rootPath);

}
