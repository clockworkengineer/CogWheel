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
// Program: CogWheel
//
// Description: Qt based FTP Server. It is not a full implementation of
// the rfc 959 standard but it does support all the main commands and the
// transfer of files in binary (no ASCII based transfer is suported as this
// is deemed unnecessary). Each connection is created in a separate thread
// and the control channel socket left permanently open for commands; the
// data channel is created/destroyed as and when needed.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelserver.h"
#include "cogwheelusersettings.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Server instance (passing true auto starts it)

    CogWheelServer server(true);

    return a.exec();
}
