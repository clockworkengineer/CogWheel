/*
 * File:   cogwheelserver.h
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

#ifndef COGWHEELSERVER_H
#define COGWHEELSERVER_H

//
// Class: CogWheelServer
//
// Description: Base server class that loads any settings, listens for
// connections and passes them to the connections handler class.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheel.h"
#include "cogwheelconnections.h"
#include "cogwheelserversettings.h"
#include "cogwheelftpcore.h"

#include <QObject>
#include <QTcpServer>

// =================
// CLASS DECLARATION
// =================

class CogWheelServer : public QTcpServer
{
    Q_OBJECT

public:

    // Class exception

    struct Exception : public std::runtime_error {

        Exception(const QString & messageStr)
            : std::runtime_error(QString("CogWheelServer Failure: " + messageStr).toStdString()) {
        }

    };

    // Constructor / Destructor

    explicit CogWheelServer(bool autoStart = false, QObject *parent = nullptr);
    ~CogWheelServer();

    // Server control

    void startServer(void);
    void stopServer(void);

    // Private data accessors

    bool isRunning() const;
    void setRunning(bool isRunning);
    CogWheelConnections *connections() const;

protected:

    // QTcpServer overrides

    void incomingConnection(qintptr handle);

signals:

    // Accept connection

    void accept(qint64 handle);

private:

    CogWheelConnections m_connections;          // Connections handler
    CogWheelServerSettings m_serverSettings;    // Server settings
    CogWheelFTPCore m_ftpServer;                // FTP server core
    bool m_running=false;                       // == true server running

};

#endif // COGWHEELSERVER_H
