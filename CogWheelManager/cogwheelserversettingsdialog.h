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

#include "../cogwheelserversettings.h"

#include <QDialog>
#include <QSettings>

namespace Ui {
class CogWheelServerSettingsDialog;
}

class CogWheelServerSettingsDialog : public QDialog
{
    Q_OBJECT

public:

    // Constructor / Destructor

    explicit CogWheelServerSettingsDialog(QWidget *parent = 0);
    ~CogWheelServerSettingsDialog();

private slots:

    // Dialog controls

    void on_saveButton_clicked();
    void on_cancelButton_clicked();

private:

    Ui::CogWheelServerSettingsDialog *ui;   // Qt dialog data

    CogWheelServerSettings m_settings;      // Server settings

};

#endif // COGWHEELSERVERSETTINGSDIALOG_H
