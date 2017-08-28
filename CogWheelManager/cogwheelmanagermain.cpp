/*
 * File:   cogwheelmanagermain.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

//
// Class: CogWheelManagerMain
//
// Description:
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelmanagermain.h"
#include "ui_cogwheelmanagermain.h"
#include "cogwheelserversettingsdialog.h"
#include "cogwheeluserlistdialog.h"

/**
 * @brief CogWheelManagerMain::CogWheelManagerMain
 * @param parent
 */
CogWheelManagerMain::CogWheelManagerMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CogWheelManagerMain)
{
    ui->setupUi(this);

    QCoreApplication::setOrganizationName("ClockWorkEngineer");
    QCoreApplication::setApplicationName("CogWheel");

}

/**
 * @brief CogWheelManagerMain::~CogWheelManagerMain
 */
CogWheelManagerMain::~CogWheelManagerMain()
{
    delete ui;
}

/**
 * @brief CogWheelManagerMain::on_actionEditServerSettings_triggered
 */
void CogWheelManagerMain::on_actionEditServerSettings_triggered()
{

    CogWheelServerSettingsDialog    serverSettings;

    serverSettings.exec();

}

/**
 * @brief CogWheelManagerMain::on_actionEditUser_triggered
 */
void CogWheelManagerMain::on_actionEditUser_triggered()
{

    CogWheelUserListDialog  userList;

    userList.exec();

}
