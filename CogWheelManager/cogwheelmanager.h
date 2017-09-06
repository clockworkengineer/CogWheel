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
#include <QSettings>

class CogWheelManager : public QLocalServer
{
    Q_OBJECT

    // Manager response function pointer

    typedef void (CogWheelManager::*ResponseFunction) (QDataStream & input) ;

public:

    // Constructor / Destructor

    explicit CogWheelManager(QObject *parent = nullptr);
    ~CogWheelManager();

    // Load manager settings

    void load();

    // Manager Control

    bool startManager();
    void stopManager();

    // Write command to server

    void writeCommandToController(const QString &command);

    // Controller commands

    void serverStatus(QDataStream &input);
    void connectionList(QDataStream &input);

    // Private data accessors

    bool isActive() const;
    void setActive(bool isActive);
    QString serverPath() const;
    void setServerPath(const QString &serverPath);
    QString serverName() const;
    void setServerName(const QString &serverName);

private:

    // Reset manager socket

    void resetManagerSocket();

protected:

    // QLocalServer override

    void incomingConnection(quintptr handle);

signals:

    void serverStatusUpdate(QString status);
    void connectionListUpdate(const QStringList &connections);

public slots:

    // Manager socket

    void connected();
    void disconnected();
    void error(QLocalSocket::LocalSocketError socketError);
    void readyRead();
    void bytesWritten(qint64 bytes);

private:

    QString m_serverPath;                   // Path to CogWheel Server
    QString m_serverName;                   // Named local socket

    bool m_active=false;                    // == true manager active
    bool m_listening=false;                 // == true manager listening on socket
    QLocalSocket *m_managerSocket;          // Manager socket
    quint32 m_commandResponseBlockSize=0;   // Commanf reply block size.

    // Manager response table

    static QHash<QString, ResponseFunction> m_managerResponseTable;

};

#endif // COGWHEELMANAGER_H
