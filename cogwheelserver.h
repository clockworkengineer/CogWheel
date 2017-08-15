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

#include <QObject>
#include <QTcpServer>

class CogWheelServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit CogWheelServer(QObject *parent = nullptr);

    void startServer(void);
    void stopServer(void);

protected:
    void incomingConnection(qintptr handle);

signals:
    void acceptConnection(qint64 handle);

public slots:

private:

    qint64 m_controlPort=2221;

    CogWheelConnections m_connections;

};

#endif // COGWHEELSERVER_H
