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

#include "../cogwheelusersettings.h"

#include <QDialog>
#include <QString>
#include <QSettings>

namespace Ui {
class CogWheelUserSettingsDialog;
}

class CogWheelUserSettingsDialog : public QDialog
{
    Q_OBJECT

public:

    // Constructor / Destructor

    explicit CogWheelUserSettingsDialog(QString userName, QWidget *parent = 0);
    ~CogWheelUserSettingsDialog();

    // Popuplate dialog with user settings

    void populateDialog();

private slots:

    // Dialog controls

    void on_saveButton_clicked();
    void on_cancelButton_clicked();

public:
    CogWheelUserSettings m_settings;     // User settings (TO DO: MAKE PRIVATE)

private:
    Ui::CogWheelUserSettingsDialog *ui; // Qt dialog data

};

#endif // COGWHEELUSERSETTINGSDIALOG_H
