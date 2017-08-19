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

#include <QObject>
#include <QTcpServer>

class CogWheelServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit CogWheelServer(bool autoStart = false, QObject *parent = nullptr);

    void startServer(void);
    void stopServer(void);

protected:
    void incomingConnection(qintptr handle);

signals:
    void accept(qint64 handle);

public slots: 
    void error(const QString &errorMessage);
    void info(const QString &message);

private:

    CogWheelConnections m_connections;          // Connections handler
    CogWheelServerSettings m_serverSettings;    // Server settings

};

#endif // COGWHEELSERVER_H
