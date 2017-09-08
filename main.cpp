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


#include "cogwheelcontroller.h"
#include "cogwheelusersettings.h"
#include "cogwheellogger.h"

#include <QCoreApplication>

#include <QString>
#include <QLockFile>
#include <QDir>

/**
 * @brief alreadyRunning
 *
 * Simple lock file to guarantee one server instance.
 *
 * @return == true Server already running.
 */
static bool alreadyRunning() {

    QString tmpDir = QDir::tempPath();
    static QLockFile lockFile(tmpDir + "/CogWheel.lock");

    if(!lockFile.tryLock(100)){
        return (true);
    } else {
        return(false);
    }

}

// ============================
// ===== MAIN ENTRY POINT =====
// ============================

int main(int argc, char *argv[])
{
    QCoreApplication cogWheelServerApplication(argc, argv);

    if  (!alreadyRunning()) {

        setLoggingLevel(CogWheelLogger::LoggingLevel::Info |
                    CogWheelLogger::LoggingLevel::Error |
                    CogWheelLogger::LoggingLevel::Warning);

        // Initialise Organisation and Application names

        QCoreApplication::setOrganizationName("ClockWorkEngineer");
        QCoreApplication::setApplicationName("CogWheel");

        // Server controller instance

        CogWheelController controller(&cogWheelServerApplication);

        controller.startController();

        if (controller.server() && controller.server()->isRunning()) {
            return cogWheelServerApplication.exec();
        } else {
            cogWheelInfo("CogWheel FTP Server not started.");
        }

    } else {
        cogWheelInfo("CogWheel FTP Server already running.");
        exit(EXIT_FAILURE);
    }
}
