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

/**
 * @brief CogWheelUserSettings::defaultSettings
 */
void CogWheelUserSettings::defaultSettings()
{
   QSettings  defaultSettings;

   defaultSettings.beginGroup("UserList");
   defaultSettings.setValue("users","guest");
   defaultSettings.endGroup();

   defaultSettings.beginGroup("guest");
   defaultSettings.setValue("password","password");
   defaultSettings.setValue("root", "/home/robt");
   defaultSettings.setValue("passive", "false");
   defaultSettings.endGroup();

}

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
    QString encryptedPassword;

    userSettings.beginGroup(userName);
    encryptedPassword = userSettings.value("password").toString();
    userSettings.endGroup();

    return(encryptedPassword==password);

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
