/*
 * File:   cogwheelmanagersingleinstance.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

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

#include "cogwheelmanagersingleinstance.h"

/**
 * @brief CogWheelManagerSingleInstance::CogWheelManagerSingleInstance
 *
 * See if application is first or a new instance.
 *
 * @param appInstance
 * @param parent
 */
CogWheelManagerSingleInstance::CogWheelManagerSingleInstance(QApplication *appInstance, QObject *parent) : QLocalServer(parent), m_appInstance(appInstance)
{

    // Create socket and try to connect

    m_appInstanceSocket = new QLocalSocket();

    m_appInstanceSocket->connectToServer("CogWheelManagerInstance");
    m_appInstanceSocket->waitForConnected(-1);

    // Failure then first time run so set up listen

    if (m_appInstanceSocket->state() ==  QLocalSocket::UnconnectedState) {
        removeServer("CogWheelManagerInstance");
        listen("CogWheelManagerInstance");
        return;
    }

    // Disconnect and delete socket and sets already running

    m_appInstanceSocket->disconnectFromServer();
    m_appInstanceSocket->waitForDisconnected(-1);
    m_appInstanceSocket->deleteLater();
    m_alreadyRunning=true;

}

/**
 * @brief CogWheelManagerSingleInstance::~CogWheelManagerSingleInstance
 */
CogWheelManagerSingleInstance::~CogWheelManagerSingleInstance()
{

}

/**
 * @brief CogWheelManagerSingleInstance::incomingConnection
 *
 * A new instance has tried to connect so bring application to front.
 *
 * @param handle
 */
void CogWheelManagerSingleInstance::incomingConnection(quintptr handle)
{

    Q_UNUSED(handle);

    m_mainWindow->raise();

}

// ============================
// CLASS PRIVATE DATA ACCESSORS
// ============================

/**
 * @brief CogWheelManagerSingleInstance::setMainWindow
 * @param mainWindow
 */
void CogWheelManagerSingleInstance::setMainWindow(QMainWindow *mainWindow)
{
    m_mainWindow = mainWindow;
}

/**
 * @brief CogWheelManagerSingleInstance::alreadyRunning
 * @return
 */
bool CogWheelManagerSingleInstance::alreadyRunning() const
{
    return m_alreadyRunning;
}
