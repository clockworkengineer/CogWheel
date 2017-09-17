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
#include <QMainWindow>
#include <QProcess>
#include <QStringListModel>

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

    // Class exception

    struct Exception : public std::runtime_error {

        Exception(const QString & messageStr)
            : std::runtime_error(QString("CogWheelManagerMain Failure: " + messageStr).toStdString()) {
        }

    };

    // Constructor / Destructor

    explicit CogWheelManagerMain(QWidget *parent = 0);
    ~CogWheelManagerMain();

    // Launch and kill server

    bool launchServer();
    void killServer();

private slots:

    // Window  controls

    void on_actionEditServerSettings_triggered();
    void on_actionEditUser_triggered();
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_launchKillButton_clicked();

    // Controller command

    void serverStatusUpdate(const QString status);
    void connectionListUpdate(const QStringList &connections);
    void logWindowUpdate(const QStringList &logBuffer);

private:

    Ui::CogWheelManagerMain *ui;        // Qt window data

    CogWheelManager m_serverManager;        // Server manager
    QProcess *m_serverProcess=nullptr;      // Server process
    QStringListModel m_serverLoggingBuffer; // Server logging buffer

};
#endif // COGWHEELMANAGERMAIN_H
