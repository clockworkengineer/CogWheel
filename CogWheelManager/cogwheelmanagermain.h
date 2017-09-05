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

#include "cogwheelmanager.h"
#include <QMainWindow>
#include <QProcess>

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

private slots:

    // Window  controls

    void on_actionEditServerSettings_triggered();
    void on_actionEditUser_triggered();
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_launchKillButton_clicked();

    void serverStatusUpdate(const QString status);

private:

    Ui::CogWheelManagerMain *ui;        // Qt window data

    CogWheelManager m_serverManager;    // Server manager
    QProcess *m_serverProcess=nullptr;  // Server process

};

#endif // COGWHEELMANAGERMAIN_H
