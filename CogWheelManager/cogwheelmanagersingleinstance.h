/*
 * File:   cogwheelmanagersingleinstance.h
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */
#ifndef COGWHEELMANAGERSINGLEINSTANCE_H
#define COGWHEELMANAGERSINGLEINSTANCE_H

//
// Class: CogWheelManagerSingleInstance
//
// Description: Class for making sure that olny one instance of the CogWheelManager
// is run at a time. It tries to connect to a given QLocal socket and if it fails then call
// listen() to wait for any connects from new instances. If connect does succeed then
// the manager is already running. Note: The Incoming connection override for the main
// instance brings the program to the front on an connection attempt.
//

// =============
// INCLUDE FILES
// =============

#include "../cogwheel.h"

#include <QObject>
#include <QLocalServer>
#include <QLocalSocket>
#include <QApplication>
#include <QMainWindow>

// =================
// CLASS DECLARATION
// =================

class CogWheelManagerSingleInstance : public QLocalServer
{
     Q_OBJECT

public:
    CogWheelManagerSingleInstance(QApplication *appInstance, QObject *parent = nullptr);
    ~CogWheelManagerSingleInstance();

    bool alreadyRunning() const;
    void setMainWindow(QMainWindow *mainWindow);

protected:

    // QLocalServer override

    void incomingConnection(quintptr handle);

private:

    QApplication *m_appInstance;                // Server Qt application instance
    QMainWindow *m_mainWindow;                  // Manager main window
    QLocalSocket *m_appInstanceSocket=nullptr;  // Instance socket
    bool m_alreadyRunning=false;                // == true already running

};

#endif // COGWHEELMANAGERSINGLEINSTANCE_H
