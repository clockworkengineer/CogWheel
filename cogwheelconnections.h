/*
 * File:   cogwheelconnections.h
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

//
// Class: CogWheelConnections
//
// Description: Class to accept new FTP connections from the
// server, create an instance of  control channel, a new thread
// to run the channel on and open the channel. The connection is
// removed on the reciept of a signal to the finshedConnection slot
// function which removes the connection from the list of current
// connections.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheel.h"
#include "cogwheelcontrolchannel.h"
#include "cogwheelserversettings.h"

#include <QObject>
#include <QTimer>

// =================
// CLASS DECLARATION
// =================

class CogWheelConnections : public QObject
{
    Q_OBJECT

public:

    // Class exception

    struct Exception : public std::runtime_error {

        Exception(const QString & messageStr)
            : std::runtime_error(QString("CogWheelConnections Failure: " + messageStr).toStdString()) {
        }

    };

    // Constructor/ Destructor

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
    QTimer *m_connectionListUpdateTimer=nullptr;            // Timer for sending connection updates to manager
    QMap<qint64, CogWheelControlChannel *> m_connections;   // Socket Handle connection mapping
    CogWheelServerSettings m_serverSettings;                // Server settings

};
#endif // COGWHEELCONNECTIONS_H
