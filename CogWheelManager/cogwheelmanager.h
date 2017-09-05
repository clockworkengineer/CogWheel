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
#include <QLocalServer>
#include <QDataStream>

class CogWheelManager : public QLocalServer
{
    Q_OBJECT

    // Manager response function pointer

    typedef void (CogWheelManager::*ResponseFunction) (QDataStream & input) ;

public:

    // Constructor / Destructor

    explicit CogWheelManager(QObject *parent = nullptr);
    ~CogWheelManager();

    // Manager Control

    bool startManager(const QString &socketName);
    void stopManager();

    // Write command to server

    void writeCommandToController(const QString &command);

    // Response commands

    void serverStatus(QDataStream &input);

    // Private data accessors

    bool isActive() const;
    void setActive(bool isActive);

private:

    // Reset manager socket

    void resetManagerSocket();

protected:

    // QLocalServer override

    void incomingConnection(quintptr handle);

signals:

    void serverStatusUpdate(QString status);

public slots:

    // Manager socket

    void connected();
    void disconnected();
    void error(QLocalSocket::LocalSocketError socketError);
    void readyRead();
    void bytesWritten(qint64 bytes);

private:
    bool m_active=false;                    // == true manager active
    bool m_listening=false;                 // == true manager listening on socket
    QString m_serverName;                   // Local socket name
    QLocalSocket *m_managerSocket;          // Manager socket
    quint32 m_commandResponseBlockSize=0;   // Commanf reply block size.

    // Manager response table

    static QHash<QString, ResponseFunction> m_managerResponseTable;

};

#endif // COGWHEELMANAGER_H
