/*
 * File:   cogwheelserversettingsdialog.h
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

#ifndef COGWHEELSERVERSETTINGSDIALOG_H
#define COGWHEELSERVERSETTINGSDIALOG_H

//
// Class: CogWheelServerSettingsDialog
//
// Description: Class to display server settings dialog.
//

// =============
// INCLUDE FILES
// =============

#include "../cogwheel.h"
#include "../cogwheelserversettings.h"

#include <QDialog>
#include <QSettings>

// =================
// CLASS DECLARATION
// =================

namespace Ui {
class CogWheelServerSettingsDialog;
}

class CogWheelServerSettingsDialog : public QDialog
{
    Q_OBJECT

public:

    // Class exception

    struct Exception : public std::runtime_error {

        Exception(const QString & messageStr)
            : std::runtime_error(QString("CogWheelServerSettingsDialog Failure: " + messageStr).toStdString()) {
        }

    };

    // Constructor / Destructor

    explicit CogWheelServerSettingsDialog(QWidget *parent = 0);
    ~CogWheelServerSettingsDialog();

    // Private data accsessors

    CogWheelServerSettings settings() const;

private slots:

    // Dialog controls

    void on_saveButton_clicked();
    void on_cancelButton_clicked();


private:

    Ui::CogWheelServerSettingsDialog *ui;   // Qt dialog data

    CogWheelServerSettings m_settings;      // Server settings

};

#endif // COGWHEELSERVERSETTINGSDIALOG_H
