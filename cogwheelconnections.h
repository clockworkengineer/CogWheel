/*
 * File:   cogwheelcconnections.h
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

#ifndef COGWHEELCONNECTIONS_H
#define COGWHEELCONNECTIONS_H

#include "cogwheelcontrolchannel.h"
#include "cogwheelserversettings.h"

#include <QObject>
#include <QTimer>

class CogWheelConnections : public QObject
{
    Q_OBJECT

public:

    // Constructor/Destructor

    explicit CogWheelConnections(QObject *parent = nullptr);
    ~CogWheelConnections();

    // Close all currenrly opn connections

    void closeAll();

    // Private data accessors

    CogWheelServerSettings serverSettings() const;
    void setServerSettings(const CogWheelServerSettings &serverSettings);


private:

    // Reset connection list update timer

    void resetConnectionListUpdateTimer();

signals:

    // Open connection / close all connections

    void openConnection(qint64 handle);
    void closeAllConnections();

    // Connection list updates for controller

    void updateConnectionList(const QStringList &connections);

public slots:
    void acceptConnection(qint64 handle);   // Accept client connection
    void finishedConnection(qint64 handle); // Connection finished
    void abortedConnection(qint64 handle);  // Connection aborted
    void connectionListToManager();         // Send connection list to manager

private:
    QTimer *m_connectionListUpdateTimer=nullptr;            // Timer for sending connectionupdates to manager
    QMap<qint64, CogWheelControlChannel *> m_connections;   // Socket Handle connection mapping
    CogWheelServerSettings m_serverSettings;                // Server settings

};

#endif // COGWHEELCONNECTIONS_H
