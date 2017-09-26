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
// the manager/controller socket used to control the server.
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

    // Setup windows data

    ui->setupUi(this);

    // Initialise config parameters

    QCoreApplication::setOrganizationName(kCWOrganizationName);
    QCoreApplication::setApplicationName(kCWApplicationName);

    // Load manager config data

    m_serverManager.load();

    // Remove any old manager socket servers

    QLocalServer::removeServer(m_serverManager.serverName()+kCWManagerPostfix);

    // Start manager

    m_serverManager.startUpManager();

    // Controller command signals/slots

    connect(&m_serverManager,&CogWheelManager::serverStatusUpdate, this, &CogWheelManagerMain::serverStatusUpdate);
    connect(&m_serverManager,&CogWheelManager::connectionListUpdate, this, &CogWheelManagerMain::connectionListUpdate);
    connect(&m_serverManager,&CogWheelManager::logWindowUpdate, this, &CogWheelManagerMain::logWindowUpdate);

    ui->logListView->setModel(&m_serverLoggingBuffer);

    // Setup window initial state

    serverStatusUpdate(kCWStatusTERMINATED);

    // If server not started and auto start set then launch

    if (m_serverManager.serverAutoStart() && !m_serverManager.managerSocket()) {
        launchServer();
    }

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
void CogWheelManagerMain::launchServer()
{

    m_serverProcess = new QProcess();
    m_serverProcess->startDetached(m_serverManager.serverPath());
    m_serverProcess->waitForStarted(-1);
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

    m_serverManager.writeCommandToController(kCWCommandKILL);

    if (m_serverProcess) {
        m_serverProcess->deleteLater();
        m_serverProcess=nullptr;
    }


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
    m_serverManager.writeCommandToController(kCWCommandSTART);
}

/**
 * @brief CogWheelManagerMain::on_stopButton_clicked
 *
 * Send stop command to server.
 *
 */
void CogWheelManagerMain::on_stopButton_clicked()
{
    m_serverManager.writeCommandToController(kCWCommandSTOP);

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
        launchServer();
    } else {
        killServer();
    }
}

/**
 * @brief CogWheelManagerMain::serverStatusUpdate
 *
 * Update server status sent by controller.
 *
 * @param status
 */

void CogWheelManagerMain::serverStatusUpdate(const QString status)
{
    qDebug() << "Server Status " << status;

    if (status==kCWStatusSTOPPED) {
        ui->serverStatus->setText("<b style=color:red;>Running but stopped.</b>");
        ui->launchKillButton->setText("Kill");
        ui->startButton->setEnabled(true);
        ui->stopButton->setEnabled(false);
    } else if (status==kCWStatusRUNNING) {
        ui->serverStatus->setText("<b style=color:green;>Running.</b>");
        ui->launchKillButton->setText("Kill");
        ui->startButton->setEnabled(false);
        ui->stopButton->setEnabled(true);
    } else if (status==kCWStatusTERMINATED) {
        ui->serverStatus->setText("<b>Not Running.</b>");
        ui->launchKillButton->setText("Launch");
        ui->startButton->setEnabled(false);
        ui->stopButton->setEnabled(false);
    }

    ui->connectionList->clear();

}

/**
 * @brief CogWheelManagerMain::connectionListUpdate
 *
 * Update connections list sent by controller.
 *
 * @param connections
 */
void CogWheelManagerMain::connectionListUpdate(const QStringList &connections)
{
    ui->connectionList->clear();
    ui->connectionList->addItems(connections);

}

/**
 * @brief CogWheelManagerLoggingDialog::logWindowUpdate
 *
 * Append recieved logging buffer contents to dialog text area.
 *
 * @param logBuffer
 */
void CogWheelManagerMain::logWindowUpdate(const QStringList &logBuffer)
{
    for (auto line : logBuffer) {
        m_serverLoggingBuffer.insertRow(m_serverLoggingBuffer.rowCount());
        QModelIndex index = m_serverLoggingBuffer.index(m_serverLoggingBuffer.rowCount()-1);
        m_serverLoggingBuffer.setData(index, line);
    }

    ui->logListView->setCurrentIndex(m_serverLoggingBuffer.index(m_serverLoggingBuffer.rowCount()-1));

}
