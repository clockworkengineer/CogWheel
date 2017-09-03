/*
 * File:   cogwheeluserlistdialog.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

//
// Class: CogWheelUserListDialog
//
// Description: Class to display user list dialog.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheeluserlistdialog.h"
#include "ui_cogwheeluserlistdialog.h"
#include "cogwheelusersettingsdialog.h"

/**
 * @brief CogWheelUserListDialog::CogWheelUserListDialog
 *
 * Load user list from config and display dialog.
 *
 * @param parent
 */
CogWheelUserListDialog::CogWheelUserListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CogWheelUserListDialog)
{
    ui->setupUi(this);

    ui->editButton->setEnabled(false);
    ui->RemoveButton->setEnabled(false);

    loadUserList();
    populateUserList();

}

/**
 * @brief CogWheelUserListDialog::~CogWheelUserListDialog
 *
 * Dialog destructor.
 *
 */
CogWheelUserListDialog::~CogWheelUserListDialog()
{
    delete ui;
}

/**
 * @brief CogWheelUserListDialog::
 *
 * Load user list from config.
 *
 */
void CogWheelUserListDialog::loadUserList()
{
    QSettings userListSettings;

    userListSettings.beginGroup("UserList");
    m_userList = userListSettings.value("users").toStringList();
    userListSettings.endGroup();
}

/**
 * @brief CogWheelUserListDialog::saveUserList
 *
 * Save current user list to config.
 *
 */
void CogWheelUserListDialog::saveUserList()
{

    QSettings userListSettings;
    userListSettings.beginGroup("UserList");
    userListSettings.setValue("users",this->m_userList);
    userListSettings.endGroup();

}

/**
 * @brief CogWheelUserListDialog::populateUserList
 *
 * Populate user list in dialog.
 *
 */
void CogWheelUserListDialog::populateUserList()
{

    ui->userList->clear();
    ui->userList->addItems(this->m_userList);

}

/**
 * @brief CogWheelUserListDialog::on_newButton_clicked
 *
 * Display new user dialog.
 *
 */
void CogWheelUserListDialog::on_newButton_clicked()
{
    CogWheelUserSettingsDialog  userSettings("");

    userSettings.exec();

    QString userName = userSettings.m_settings.getUserName();
    if (!userName.isEmpty()) {
        this->m_userList.append(userName);
        this->populateUserList();
        saveUserList();
    }

}

/**
 * @brief CogWheelUserListDialog::on_editButton_clicked
 *
 * Display edit user dialog.
 *
 */
void CogWheelUserListDialog::on_editButton_clicked()
{

    QListWidgetItem *userToEdit= ui->userList->currentItem();

    if(userToEdit != nullptr) {

        CogWheelUserSettingsDialog  userSettings(userToEdit->text());

        userSettings.exec();

    }

}

/**
 * @brief CogWheelUserListDialog::on_RemoveButton_clicked
 *
 * Remove user from list and its config data.
 */
void CogWheelUserListDialog::on_RemoveButton_clicked()
{

    QListWidgetItem *userToDelete = ui->userList->currentItem();

    if(userToDelete != nullptr) {
        QString userName = userToDelete->text();
        int userIndex = this->m_userList.indexOf(userName);
        if (userIndex != -1) {
            this->m_userList.removeAt(userIndex);
            this->populateUserList();
        }
        QSettings userSettings;
        userSettings.beginGroup(userName);
        userSettings.remove("");
        userSettings.endGroup();
        saveUserList();
    }
}

/**
 * @brief CogWheelUserListDialog::on_userList_clicked
 *
 * Activate buttons on user list being clicked.
 *
 * @param index
 */
void CogWheelUserListDialog::on_userList_clicked(const QModelIndex &index)
{
    Q_UNUSED(index);

    ui->editButton->setEnabled(true);
    ui->RemoveButton->setEnabled(true);
}
