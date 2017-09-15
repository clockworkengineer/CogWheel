/*
 * File:   cogwheelmanagermain.h
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

#ifndef COGWHEELMANAGERMAIN_H
#define COGWHEELMANAGERMAIN_H

//
// Class: CogWheelManagerMain
//
// Description: Class for CogManager main window. It creates, uses and destroys
// the manager/controller socket used to control the server.
//

// =============
// INCLUDE FILES
// =============

#include "../cogwheel.h"

#include "cogwheelmanager.h"
#include "cogwheelmanagerloggingdialog.h"
#include <QMainWindow>
#include <QProcess>

// =================
// CLASS DECLARATION
// =================

namespace Ui {
class CogWheelManagerMain;
}

class CogWheelManagerMain : public QMainWindow
{
    Q_OBJECT

public:

    // Constructor / Destructor

    explicit CogWheelManagerMain(QWidget *parent = 0);
    ~CogWheelManagerMain();

    // Launch and kill server

    bool launchServer();
    void killServer();

private:
    void loggingEnableDisable();

private slots:

    // Window  controls

    void on_actionEditServerSettings_triggered();
    void on_actionEditUser_triggered();
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_launchKillButton_clicked();
    void on_actionLogging_triggered();

    // Controller command

    void serverStatusUpdate(const QString status);
    void connectionListUpdate(const QStringList &connections);

private:

    Ui::CogWheelManagerMain *ui;        // Qt window data

    CogWheelManagerLoggingDialog m_logWindow;   // server Logging Window
    CogWheelManager m_serverManager;            // Server manager
    QProcess *m_serverProcess=nullptr;          // Server process

};

#endif // COGWHEELMANAGERMAIN_H
