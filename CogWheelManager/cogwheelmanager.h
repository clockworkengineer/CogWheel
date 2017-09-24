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

//
// Class: CogWheelManager
//
// Description: Class to open up a local socket to server and send commands
// to control it. At present this is just stop, start and kill. It also supports
// commands sent from the server (controller) such as update connection list.
//

// =============
// INCLUDE FILES
// =============

#include "../cogwheel.h"

#include <QObject>
#include <QLocalSocket>
#include <QLocalServer>
#include <QBuffer>
#include <QDataStream>
#include <QSettings>

// =================
// CLASS DECLARATION
// =================

class CogWheelManager : public QLocalServer
{
    Q_OBJECT

    // Controller command function pointer

    typedef void (CogWheelManager::*CommandFunction) (QDataStream & input) ;

public:

    // Class exception

    struct Exception : public std::runtime_error {

        Exception(const QString & messageStr)
            : std::runtime_error(static_cast<QString>("CogWheelManager Failure: " + messageStr).toStdString()) {
        }

    };

    // Constructor / Destructor

    explicit CogWheelManager(QObject *parent = nullptr);
    ~CogWheelManager();

    // Load manager settings

    void load();

    // Manager Control

    void startUpManager();

    // Connect/disconnect server

    void connectToServer();
    void disconnectFromServer();

    // Write command to server

    void writeCommandToController(const QString &command);
    void writeCommandToController(const QString &command, const QString &param1);

    // Controller commands

    void serverStatus(QDataStream &input);
    void connectionList(QDataStream &input);
    void logOutput(QDataStream &input);

    // Private data accessors

    QString serverPath() const;
    void setServerPath(const QString &serverPath);
    QString serverName() const;
    void setServerName(const QString &serverName);

private:

    // Reset manager socket

    void resetManagerSocket();

    // Connect up manager socket signals/slots

    void connectUpManagerSocket();

protected:

    // QLocalServer override

    void incomingConnection(quintptr handle);

signals:

    // Controller command signals

    void serverStatusUpdate(const QString &status);
    void connectionListUpdate(const QStringList &connections);
    void logWindowUpdate(const QStringList &logBuffer);

public slots:

    // Manager socket

    void connected();
    void disconnected();
    void error(QLocalSocket::LocalSocketError socketError);
    void readyRead();

private:

    QString m_serverPath;                   // Path to CogWheel Server
    QString m_serverName;                   // Manager socket name
    QLocalSocket *m_managerSocket=nullptr;  // Manager socket
    quint32 m_commandResponseBlockSize=0;   // Command reply block size.
    QByteArray m_writeRawDataBuffer;        // Write raw data buffer
    QBuffer m_writeQBuffer;                 // Write QBuffer
    QDataStream m_managerWriteStream;       // Write data stream
    QDataStream m_managerReadStream;        // Read data stream

    // Controller command table

    static QHash<QString, CommandFunction> m_controllerCommandTable;

};

#endif // COGWHEELMANAGER_H
