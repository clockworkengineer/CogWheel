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
// the server. Which includes launch/closedown of the server
// and stopping and starting it.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelmanagermain.h"
#include "cogwheelmanagersingleinstance.h"

#include <QApplication>

// ============================
// ===== MAIN ENTRY POINT =====
// ============================

int main(int argc, char *argv[])
{

    QApplication cogWheelManagerApp(argc, argv);

    CogWheelManagerSingleInstance singleInstance(&cogWheelManagerApp);

    if (!singleInstance.alreadyRunning()){

        try {

            CogWheelManagerMain cogWheelMainWindow;
            singleInstance.setMainWindow(&cogWheelMainWindow);
            cogWheelMainWindow.show();
            return cogWheelManagerApp.exec();

        } catch(std::runtime_error &err) {
            qDebug() << err.what();
            exit(EXIT_FAILURE);

        } catch(...) {
            qDebug() << "An unknown exception occurred.";
            exit(EXIT_FAILURE);
        }

    } else {
        exit(EXIT_FAILURE);
    }

}
