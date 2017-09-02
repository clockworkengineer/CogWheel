/*
 * File:   cogwheelserver.cpp
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

#include "cogwheelconnections.h"
#include "cogwheelserversettings.h"
#include "cogwheelftpcore.h"

#include <QObject>
#include <QTcpServer>

class CogWheelServer : public QTcpServer
{
    Q_OBJECT

public:

    // Constructor / Destructor

    explicit CogWheelServer(bool autoStart = false, QObject *parent = nullptr);
    ~CogWheelServer();

    // Server control

    void startServer(void);
    void stopServer(void);

    // Private data accessors

    bool isRunning() const;
    void setRunning(bool isRunning);

protected:

    // QTcpServer overrides

    void incomingConnection(qintptr handle);

signals:

    // Accept connection

    void accept(qint64 handle);

public slots:

    // Error, information and warning messages

    void error(const QString &message);
    void info(const QString &message);
    void warning(const QString &message);

private:

    CogWheelConnections m_connections;          // Connections handler
    CogWheelServerSettings m_serverSettings;    // Server settings
    CogWheelFTPCore m_ftpServer;                // FTP server core

    bool m_running=false;   // == true server running


};

#endif // COGWHEELSERVER_H
