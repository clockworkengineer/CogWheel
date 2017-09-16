/*
 * File:   cogwheelusersettings.h
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

//
// Class: CogWheelUserSettings
//
// Description: Class to load and save the user settings. It also
// validates a user exists and authenticates their password.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheel.h"

#include <QSettings>

#include <stdexcept>

// =================
// CLASS DECLARATION
// =================

class CogWheelUserSettings
{

public:

    // Class exception

    struct Exception : public std::runtime_error {

        Exception(const QString & messageStr)
            : std::runtime_error(QString("CogWheelUserSettings Failure: " + messageStr).toStdString()) {
        }

    };

    // User validation and authentication

    static bool checkUserName(const QString& userName);
    static bool checkUserPassword(const QString& hashedPassword, const QString& password);

    // Load and save settings

    void load(QString userName);
    void save(QString userName);

    // Private data accessors

    QString getUserName() const;
    void setUserName(const QString &userName);
    QString getUserPassword() const;
    void setUserPassword(const QString &userPassword);
    QString getAccountName() const;
    void setAccountName(const QString &accountName);
    bool getEnabled() const;
    void setEnabled(bool enabled);
    bool getWriteAccess() const;
    void setWriteAccess(bool writeAccess);
    QString getRootPath() const;
    void setRootPath(const QString &rootPath);

private:
    QString m_userName;         // User name
    QString m_userPassword;     // User password
    QString m_rootPath;         // Root path
    QString m_accountName;      // Account name
    bool m_enabled=false;       // == true user enabled
    bool m_writeAccess=false;   // == true user has write access

};
#endif // COGWHEELUSERSETTINGS_H
