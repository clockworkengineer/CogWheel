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
// Description:
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelmanagermain.h"
#include <QApplication>

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    QApplication cogWheelManagerApp(argc, argv);

    CogWheelManagerMain cogWheelMainWindow;

    cogWheelMainWindow.show();

    return cogWheelManagerApp.exec();
}
