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

#include <QObject>

class CogWheelConnections : public QObject
{
    Q_OBJECT

public:

    explicit CogWheelConnections(QObject *parent = nullptr);

signals:
    void openConnection(qint64 handle);

public slots:
    void acceptConnection(qint64 handle);
    void finishedConnection(qint64 handle);
    void abortedConnection(qint64 handle);

private:
    QMap<qint64, CogWheelControlChannel *> m_connections;

};

#endif // COGWHEELCONNECTIONS_H
