/*
 * File:   cogwheelmanager.h
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

#ifndef COGWHEELMANAGER_H
#define COGWHEELMANAGER_H

#include <QObject>
#include <QLocalSocket>
#include <QDataStream>

class CogWheelManager : public QObject
{
    Q_OBJECT

public:

    // Constructor / Destructor

    explicit CogWheelManager(QObject *parent = nullptr);
    ~CogWheelManager();

    // Manager Control

    bool startManager(const QString &socketName);
    void stopManager();

    // Write command to server

    void writeCommand(const QString &command);

    // Private data accessors

    bool isActive() const;
    void setActive(bool isActive);

signals:

public slots:

    // Manager socket

    void connected();
    void disconnected();
    void error(QLocalSocket::LocalSocketError socketError);
    void readyRead();
    void bytesWritten(qint64 bytes);

private:
    bool m_active=false;            // == true manager active
    QString m_serverName;           // Local socket name
    QLocalSocket *m_managerSocket;  // Manager socket

};

#endif // COGWHEELMANAGER_H
