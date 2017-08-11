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
   defaultSettings.setValue("home", "/home/robt");
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

bool CogWheelUserSettings::checkPassword(const QString& userName, const QString& password)
{

    QSettings  userSettings;
    QString encryptedPassword;

    userSettings.beginGroup(userName);
    encryptedPassword = userSettings.value("password").toString();
    userSettings.endGroup();

    return(encryptedPassword==password);
}

QString CogWheelUserSettings::getHomePath(const QString &userName)
{
    QSettings  userSettings;
    QString homePath;

    userSettings.beginGroup(userName);
    homePath = userSettings.value("home").toString();
    userSettings.endGroup();

    return(homePath);

}
