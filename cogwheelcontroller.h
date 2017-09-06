/*
 * File:   cogwheelcontroller.h
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */
#ifndef COGWHEELCONTROLLER_H
#define COGWHEELCONTROLLER_H

#include "cogwheelserver.h"

#include <QObject>
#include <QLocalServer>
#include <QLocalSocket>
#include <QDataStream>
#include <QCoreApplication>

class CogWheelController : public QLocalServer
{

    Q_OBJECT

    // Controller command function pointer

    typedef void (CogWheelController::*CommandFunction) (QDataStream & input);

public:

    // Constructor / Destructor

    CogWheelController(QCoreApplication *cogWheelApp,const QString &socketName, QObject *parent = nullptr);
    ~CogWheelController();

    // Start/Stop controller

    void startController();
    void stopController();

    // Write command to manager

    void writeCommandToManager(const QString &command, const QString param1);
    void writeCommandToManager(const QString &command, const QStringList param1);

    // Pricate data accessors

    static CogWheelServer *server();

private:

    // Reset Controller socket

    void resetControllerSocket();

    // Controller commands

    void startServer(QDataStream &input);
    void stopServer(QDataStream &input);
    void killServer(QDataStream &input);

protected:

    // QLocalServer override

    void incomingConnection(quintptr handle);

public slots:

    // Controller socket

    void connected();
    void disconnected();
    void error(QLocalSocket::LocalSocketError socketError);
    void readyRead();
    void bytesWritten(qint64 bytes);

    // Command slots

    void updateConnectionList(const QStringList &connections);

private:

    bool m_active=false;                        // == true controller active
    bool m_listening=false;                     // == true controller listening for connects
    QString m_serverName;                       // Named local socket
    QLocalSocket *m_controllerSocket=nullptr;   // Controller local socket
    quint32 m_commandBlockSize=0;               // Current command block size.

    static QCoreApplication *m_cogWheelApplication;  // Qt Application object
    static CogWheelServer *m_server;                 // FTP Server instance

    // Controller command table

    static QHash<QString, CommandFunction> m_controllerCommandTable;

};
#endif // COGWHEELCONTROLLER_H
