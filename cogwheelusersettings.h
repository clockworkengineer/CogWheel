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

#ifndef COGWHEELUSERSETTINGS_H
#define COGWHEELUSERSETTINGS_H

#include <QSettings>

class CogWheelUserSettings
{

public:
    static bool checkUserName(const QString& userName);
    static bool checkUserPassword(const QString& userName, const QString& password);
    static QString getRootPath(const QString& userName);

signals:

public slots:

private:
    QString m_userName;         // User name
    QString m_userPassword;     // User password
    QString m_rootPath;         // Root path
    QString m_accountName;      // Account name

};
#endif // COGWHEELUSERSETTINGS_H
