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

    typedef std::function<void (QDataStream &) > CommandFunction;

public:

    // Constructor / Destructor

    CogWheelController(QCoreApplication *cogWheelApp,const QString &socketName, QObject *parent = nullptr);
    ~CogWheelController();

    // Start/Stop controller

    void startController();
    void stopController();

    static CogWheelServer *server();

private:

    // Controller commands

    static void startServer(QDataStream &input);
    static void stopServer(QDataStream &input);
    static void killServer(QDataStream &input);

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

private:

    QString m_socketName;                       // Named local socket
    QLocalSocket *m_controllerSocket=nullptr;   // Controller local socket
    quint32 m_commandBlockSize=0;               // Current command block size.

    static QCoreApplication *m_cogWheelApplication;  //
    static CogWheelServer *m_server;                 // FTP Server instance

    // Controller command table

    static QHash<QString, CommandFunction> m_controllerCommandTable;

};
#endif // COGWHEELCONTROLLER_H
