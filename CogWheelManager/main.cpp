/*
 * File:   main.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

//
// Program: CogWheelManager
//
// Description: Companion program to the CogWheel FTP server
// for managing server settings, of users and control over
// the server. Which includes lanuch/closedown of the server
// and stopping and starting it.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelmanagermain.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication cogWheelManagerApp(argc, argv);

    CogWheelManagerMain cogWheelMainWindow;

    cogWheelMainWindow.show();

    return cogWheelManagerApp.exec();
}
