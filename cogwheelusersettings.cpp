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
// Description:
//
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelusersettings.h"
#include <QtCore>

CogWheelUserSettings::CogWheelUserSettings(QObject *parent) : QObject(parent)
{

}

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

bool CogWheelUserSettings::checkUserName(const QString& userName)
{
      QSettings  userList;
      QStringList userNames;

      userList.beginGroup("UserList");
      userNames = userList.value("users").toStringList();
      userList.endGroup();

      return(!userNames.empty() && userNames.contains(userName));

}

bool CogWheelUserSettings::checkUserPassword(const QString& userName, const QString& password)
{

    QSettings  userSettings;
    QString encryptedPassword;

    userSettings.beginGroup(userName);
    encryptedPassword = userSettings.value("password").toString();
    userSettings.endGroup();

    return(encryptedPassword==password);
}

QString CogWheelUserSettings::getRootPath(const QString &userName)
{
    QSettings  userSettings;
    QString rootPath;

    userSettings.beginGroup(userName);
    rootPath = userSettings.value("root").toString();
    userSettings.endGroup();

    return(rootPath);

}
