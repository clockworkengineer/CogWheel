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
// Description:
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelusersettingsdialog.h"
#include "ui_cogwheelusersettingsdialog.h"

/**
 * @brief CogWheelUserSettingsDialog::CogWheelUserSettingsDialog
 * @param userName
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
 */
CogWheelUserSettingsDialog::~CogWheelUserSettingsDialog()
{
    delete ui;
}

/**
 * @brief CogWheelUserSettingsDialog::populateDialog
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
 */
void CogWheelUserSettingsDialog::on_cancelButton_clicked()
{

    this->close();

}
