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

class CogWheelConnections : public QObject
{
    Q_OBJECT

public:

    // Constructor

    explicit CogWheelConnections(QObject *parent = nullptr);

    void closeAll();

    // Private data accessors

    CogWheelServerSettings serverSettings() const;
    void setServerSettings(const CogWheelServerSettings &serverSettings);


signals:

    // Open connection

    void openConnection(qint64 handle);

    // Error, information and warning messages

    void error(const QString &message);
    void info(const QString &message);
    void warning(const QString &message);

    void closeAllConnections();

public slots:
    void acceptConnection(qint64 handle);   // Accept client connection
    void finishedConnection(qint64 handle); // Connection finished
    void abortedConnection(qint64 handle);  // Connection aborted

private:
    QMap<qint64, CogWheelControlChannel *> m_connections;   // Socket Handle connection mapping
    CogWheelServerSettings m_serverSettings;                // Server settings

};

#endif // COGWHEELCONNECTIONS_H
