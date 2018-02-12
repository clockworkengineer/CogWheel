/*
 * File:   cogwheelusersettingsdialog.h
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

#ifndef COGWHEELUSERSETTINGSDIALOG_H
#define COGWHEELUSERSETTINGSDIALOG_H

//
// Class: CogWheelUserSettingsDialog
//
// Description: Class to display user settings dialog.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheel.h"
#include "cogwheelusersettings.h"

#include <QDialog>
#include <QString>
#include <QSettings>

// =================
// CLASS DECLARATION
// =================

namespace Ui {
class CogWheelUserSettingsDialog;
}

class CogWheelUserSettingsDialog : public QDialog
{
    Q_OBJECT

public:

    // Class exception

    struct Exception : public std::runtime_error {

        Exception(const QString & messageStr)
            : std::runtime_error(static_cast<QString>("CogWheelUserSettingsDialog Failure: " + messageStr).toStdString()) {
        }

    };

    // Constructor / Destructor

    explicit CogWheelUserSettingsDialog(QString userName, QWidget *parent = 0);
    ~CogWheelUserSettingsDialog();

    // Popuplate dialog with user settings

    void populateDialog();

    // Private data accessors

    CogWheelUserSettings settings() const;

private slots:

    // Dialog controls

    void on_saveButton_clicked();
    void on_cancelButton_clicked();

private:

    Ui::CogWheelUserSettingsDialog *ui;  // Qt dialog data

    CogWheelUserSettings m_settings;     // User settings

};

#endif // COGWHEELUSERSETTINGSDIALOG_H
