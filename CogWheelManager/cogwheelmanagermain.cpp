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

    m_serverManager.load();
    m_serverManager.startManager();

    connect(&m_serverManager,&CogWheelManager::serverStatusUpdate, this, &CogWheelManagerMain::serverStatusUpdate);
    connect(&m_serverManager,&CogWheelManager::connectionListUpdate, this, &CogWheelManagerMain::connectionListUpdate);

    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(false);
    ui->launchKillButton->setText("Launch");
    ui->serverStatus->setText("<b>Not Running.</b>");

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
 * @brief CogWheelManagerMain::launchServer
 *
 * Launch FTP server as detached process.
 *
 * @return == true on success.
 */
bool CogWheelManagerMain::launchServer()
{

    m_serverProcess = new QProcess();
    m_serverProcess->startDetached( m_serverManager.serverPath());
    m_serverProcess->waitForStarted(-1);

    m_serverManager.startManager();

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

    m_serverManager.writeCommandToController("KILL");

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
    m_serverManager.writeCommandToController("START");
}

/**
 * @brief CogWheelManagerMain::on_stopButton_clicked
 *
 * Send stop command to server.
 *
 */
void CogWheelManagerMain::on_stopButton_clicked()
{
    m_serverManager.writeCommandToController("STOP");

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
            ui->startButton->setEnabled(false);
            ui->stopButton->setEnabled(false);
            ui->connectionList->clear();
            ui->serverStatus->setText("<b>Running.</b>");
        }
    } else {
        killServer();
        ui->launchKillButton->setText("Launch");
        ui->startButton->setEnabled(false);
        ui->stopButton->setEnabled(false);
        ui->connectionList->clear();
        ui->serverStatus->setText("<b>Not Running.</b>");
    }
}

void CogWheelManagerMain::serverStatusUpdate(const QString status)
{
  qDebug() << "Server Status " << status;

  if (status=="STOPPED") {
      ui->serverStatus->setText("<b>Running but stopped.</b>");
      ui->startButton->setEnabled(true);
      ui->stopButton->setEnabled(false);
      ui->launchKillButton->setText("Kill");
  } else if (status=="RUNNING") {
      ui->serverStatus->setText("<b>Running.</b>");
      ui->startButton->setEnabled(false);
      ui->stopButton->setEnabled(true);
      ui->launchKillButton->setText("Kill");
  }

   ui->connectionList->clear();

}

void CogWheelManagerMain::connectionListUpdate(const QStringList &connections)
{
    ui->connectionList->clear();
    ui->connectionList->addItems(connections);

}

