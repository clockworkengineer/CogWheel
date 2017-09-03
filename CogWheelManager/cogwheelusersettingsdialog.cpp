/*
 * File:   cogwheelusersettingsdialog.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

//
// Class: CogWheelUserSettingsDialog
//
// Description: Class to display user settings dialog.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelusersettingsdialog.h"
#include "ui_cogwheelusersettingsdialog.h"

/**
 * @brief CogWheelUserSettingsDialog::CogWheelUserSettingsDialog
 *
 * Create  user settings dialog. If a user name is passed in then the
 * user field is set ready only and we have an edit dialog.
 *
 * @param userName  User name (== "" then new user)
 * @param parent
 */
CogWheelUserSettingsDialog::CogWheelUserSettingsDialog(QString userName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CogWheelUserSettingsDialog)
{
    ui->setupUi(this);

    if (!userName.isEmpty()) {
        m_settings.setUserName(userName);
        m_settings.load(userName);
        populateDialog();
        ui->userName->setReadOnly(true);
    }
}

/**
 * @brief CogWheelUserSettingsDialog::~CogWheelUserSettingsDialog
 *
 * Dialog destructor.
 *
 */
CogWheelUserSettingsDialog::~CogWheelUserSettingsDialog()
{
    delete ui;
}

/**
 * @brief CogWheelUserSettingsDialog::populateDialog
 *
 * Populate user settings dialog.
 *
 */
void CogWheelUserSettingsDialog::populateDialog()
{

    ui->userName->setText(m_settings.getUserName());
    ui->rootDirectory->setText(m_settings.getRootPath());
    ui->account->setText(m_settings.getAccountName());
    ui->enabledUser->setChecked(m_settings.getEnabled());
    ui->writeAccess->setChecked(m_settings.getWriteAccess());

}

/**
 * @brief CogWheelUserSettingsDialog::on_saveButton_clicked
 *
 * Save current user settings away to config.
 *
 */
void CogWheelUserSettingsDialog::on_saveButton_clicked()
{

    m_settings.setUserName(ui->userName->text());
    m_settings.setUserPassword(ui->password->text());
    m_settings.setRootPath(ui->rootDirectory->text());
    m_settings.setAccountName(ui->account->text());
    m_settings.setEnabled(ui->enabledUser->checkState() == Qt::Checked);
    m_settings.setWriteAccess(ui->writeAccess->checkState() == Qt::Checked);

    m_settings.save(m_settings.getUserName());

    this->close();
}

/**
 * @brief CogWheelUserSettingsDialog::on_cancelButton_clicked
 *
 * Close dialog.
 *
 */
void CogWheelUserSettingsDialog::on_cancelButton_clicked()
{
    this->close();
}
