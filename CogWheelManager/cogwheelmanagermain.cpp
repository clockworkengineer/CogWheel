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
// Description: Class for CogManager main window. It creates, uses and destroys
// the manager socket used to control the server.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelmanagermain.h"
#include "ui_cogwheelmanagermain.h"
#include "cogwheelserversettingsdialog.h"
#include "cogwheeluserlistdialog.h"

#include <QThread>

/**
 * @brief CogWheelManagerMain::CogWheelManagerMain
 *
 * Main window constructor.
 *
 * @param parent
 */
CogWheelManagerMain::CogWheelManagerMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CogWheelManagerMain)
{
    ui->setupUi(this);

    QCoreApplication::setOrganizationName("ClockWorkEngineer");
    QCoreApplication::setApplicationName("CogWheel");

    m_serverManager.startManager("CogWheel");

    updateServerStatus();

}

/**
 * @brief CogWheelManagerMain::~CogWheelManagerMain
 *
 * Main window destructor.
 *
 */
CogWheelManagerMain::~CogWheelManagerMain()
{
    delete ui;
}

/**
 * @brief CogWheelManagerMain::updateServerStatus
 *
 * Update server status.
 *
 */
void CogWheelManagerMain::updateServerStatus()
{
    if (!m_serverManager.isActive()) {
        ui->startButton->setEnabled(false);
        ui->stopButton->setEnabled(false);
        ui->launchKillButton->setText("Launch");
        ui->serverStatus->setText("<b>Not Running.</b>");
    } else {
        ui->serverStatus->setText("<b>Running.</b>");
        ui->startButton->setEnabled(false);
        ui->stopButton->setEnabled(true);
    }
}

/**
 * @brief CogWheelManagerMain::launchServer
 *
 * Launch FTP server as detached process.
 *
 * @return == true on success.
 */
bool CogWheelManagerMain::launchServer()
{
    // HARD ENDCODED NEEDS TO CHANGE
    QString program = "/home/robt/QtProjects/build-CogWheel-Desktop_Qt_5_9_0_GCC_64bit-Debug/CogWheel";

    m_serverProcess = new QProcess();
    m_serverProcess->startDetached(program);
    m_serverProcess->waitForStarted(-1);
    QThread::sleep(2);  // WAIT FOR SERVER TO CREATE MANAGER SOCKET (NEEDS TO CHANGE).

    m_serverManager.startManager("CogWheel");

    return(true);

}

/**
 * @brief CogWheelManagerMain::killServer
 *
 * Send kill command to server. Remove its local process object
 * and stop manager.
 *
 */
void CogWheelManagerMain::killServer()
{

    m_serverManager.writeCommand("KILL");

    if (m_serverProcess) {
        m_serverProcess->deleteLater();
        m_serverProcess=nullptr;
    }

    m_serverManager.stopManager();

}

/**
 * @brief CogWheelManagerMain::on_actionEditServerSettings_triggered
 *
 * Display server settings dialog.
 *
 */
void CogWheelManagerMain::on_actionEditServerSettings_triggered()
{

    CogWheelServerSettingsDialog    serverSettings;

    serverSettings.exec();

}

/**
 * @brief CogWheelManagerMain::on_actionEditUser_triggered
 *
 * Display user list dialog.
 *
 */
void CogWheelManagerMain::on_actionEditUser_triggered()
{

    CogWheelUserListDialog  userList;

    userList.exec();

}

/**
 * @brief CogWheelManagerMain::on_startButton_clicked
 *
 * Send start comand to server.
 *
 */
void CogWheelManagerMain::on_startButton_clicked()
{
    m_serverManager.writeCommand("START");
    ui->serverStatus->setText("<b>Running.</b>");
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);

}

/**
 * @brief CogWheelManagerMain::on_stopButton_clicked
 *
 * Send stop command to server.
 *
 */
void CogWheelManagerMain::on_stopButton_clicked()
{
    m_serverManager.writeCommand("STOP");
    ui->serverStatus->setText("<b>Running but stopped.</b>");
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);

}

/**
 * @brief CogWheelManagerMain::on_launchKillButton_clicked
 *
 * Launch/kill server process.
 *
 */
void CogWheelManagerMain::on_launchKillButton_clicked()
{
    if (ui->launchKillButton->text()=="Launch") {
        if (launchServer()) {
            ui->launchKillButton->setText("Kill");
            updateServerStatus();
        }
    } else {
        killServer();
        ui->launchKillButton->setText("Launch");
        updateServerStatus();
    }
}

