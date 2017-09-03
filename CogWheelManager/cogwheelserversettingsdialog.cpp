/*
 * File:   cogwheelserversettingsdialog.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

//
// Class: CogWheelServerSettingsDialog
//
// Description: Class to display server settings dialog.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelserversettingsdialog.h"
#include "ui_cogwheelserversettingsdialog.h"

/**
 * @brief CogWheelServerSettingsDialog::CogWheelServerSettingsDialog
 *
 * Load server settings from config and display.
 *
 * @param parent
 */
CogWheelServerSettingsDialog::CogWheelServerSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CogWheelServerSettingsDialog)
{
    ui->setupUi(this);

    m_settings.load();

    ui->serverName->setText(m_settings.serverName());
    ui->serverPort->setText(QString::number(m_settings.serverPort()));
    ui->serverVersion->setText(m_settings.serverVersion());
    ui->serverEnabled->setChecked(m_settings.serverEnabled());
    ui->serverAllowSMNT->setChecked(m_settings.serverAllowSMNT());
    ui->serverTlsSslEnabled->setChecked(m_settings.serverSslEnabled());
    ui->serverPlainFTPEnabled->setChecked(m_settings.serverPlainFTPEnabled());
    ui->serverAnonymousEnabled->setChecked(m_settings.serverAnonymousEnabled());
    ui->serverKeyFile->setText(m_settings.serverKeyFileName());
    ui->serverCertFile->setText(m_settings.serverCertFileName());

}

/**
 * @brief CogWheelServerSettingsDialog::~CogWheelServerSettingsDialog
 *
 * Dialog destructor.
 *
 */
CogWheelServerSettingsDialog::~CogWheelServerSettingsDialog()
{
    delete ui;
}

/**
 * @brief CogWheelServerSettingsDialog::on_saveButton_clicked
 *
 * Save current server dialog settings to config.
 *
 */
void CogWheelServerSettingsDialog::on_saveButton_clicked()
{

    m_settings.setServerName(ui->serverName->text());
    m_settings.setServerPort(ui->serverPort->text().toUInt());
    m_settings.setServerVersion(ui->serverVersion->text());
    m_settings.setServerEnabled(ui->serverEnabled->checkState() == Qt::Checked);
    m_settings.setServerAllowSMNT(ui->serverAllowSMNT->checkState() == Qt::Checked);
    m_settings.setServerSslEnabled(ui->serverTlsSslEnabled->checkState() == Qt::Checked);
    m_settings.setServerPlainFTPEnabled(ui->serverPlainFTPEnabled->checkState() == Qt::Checked);
    m_settings.setServerAnonymousEnabled(ui->serverAnonymousEnabled->checkState() == Qt::Checked);
    m_settings.setServerKeyFileName(ui->serverKeyFile->text());
    m_settings.setServerCertFileName(ui->serverCertFile->text());

    m_settings.save();

    this->close();
}

/**
 * @brief CogWheelServerSettingsDialog::on_cancelButton_clicked
 *
 * Close dialog.
 *
 */
void CogWheelServerSettingsDialog::on_cancelButton_clicked()
{
    this->close();
}
