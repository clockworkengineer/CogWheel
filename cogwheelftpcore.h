/*
 * File:   cogwheelftpcore.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

#ifndef COGWHEELFTPCORE_H
#define COGWHEELFTPCORE_H

#include <cogwheelcontrolchannel.h>

#include <QObject>
#include <QMap>
#include <QString>
#include <QFileInfo>

class CogWheelFTPCore : public QObject
{
    Q_OBJECT

    typedef std::function<void (CogWheelControlChannel *, QString) > FTPCommandFunction;

public:
    explicit CogWheelFTPCore(QObject *parent = nullptr);

    static void performCommand(CogWheelControlChannel *connection, const QString &command, const QString &arguments);
    static QString getResponseText(quint16 responseCode);

private:
    static QString buildListLine(QFileInfo &file);
    static QString mapPathToLocal(CogWheelControlChannel *connection, const QString& path);
    static QString mapPathFromLocal(CogWheelControlChannel *connection, const QString& path);

    static void USER(CogWheelControlChannel *connection, QString arguments);
    static void LIST(CogWheelControlChannel *connection, QString arguments);
    static void FEAT(CogWheelControlChannel *connection, QString arguments);
    static void SYST(CogWheelControlChannel *connection, QString arguments);
    static void PWD(CogWheelControlChannel *connection, QString arguments);
    static void TYPE(CogWheelControlChannel *connection, QString arguments);
    static void PORT(CogWheelControlChannel *connection, QString arguments);
    static void CWD(CogWheelControlChannel *connection, QString arguments);
    static void PASS(CogWheelControlChannel *connection, QString arguments);
    static void CDUP(CogWheelControlChannel *connection, QString arguments);
    static void RETR(CogWheelControlChannel *connection, QString arguments);
    static void NOOP(CogWheelControlChannel *connection, QString arguments);
    static void MODE(CogWheelControlChannel *connection, QString arguments);
    static void STOR(CogWheelControlChannel *connection, QString arguments);
    static void PASV(CogWheelControlChannel *connection, QString arguments);
    static void HELP(CogWheelControlChannel *connection, QString arguments);
    static void STAT(CogWheelControlChannel *connection, QString arguments);
    static void SITE(CogWheelControlChannel *connection, QString arguments);
    static void NLST(CogWheelControlChannel *connection, QString arguments);
    static void MKD(CogWheelControlChannel *connection, QString arguments);
    static void RMD(CogWheelControlChannel *connection, QString arguments);
    static void DELE(CogWheelControlChannel *connection, QString arguments);
    static void ACCT(CogWheelControlChannel *connection, QString arguments);
    static void SMNT(CogWheelControlChannel *connection, QString arguments);
    static void QUIT(CogWheelControlChannel *connection, QString arguments);
    static void REIN(CogWheelControlChannel *connection, QString arguments);
    static void STRU(CogWheelControlChannel *connection, QString arguments);
    static void STOU(CogWheelControlChannel *connection, QString arguments);
    static void APPE(CogWheelControlChannel *connection, QString arguments);
    static void ALLO(CogWheelControlChannel *connection, QString arguments);
    static void REST(CogWheelControlChannel *connection, QString arguments);
    static void RNFR(CogWheelControlChannel *connection, QString arguments);
    static void RNTO(CogWheelControlChannel *connection, QString arguments);
    static void ABOR(CogWheelControlChannel *connection, QString arguments);

signals:

public slots:

public:

    static QHash<QString, FTPCommandFunction> m_unauthCommandTable; // Unauthorised user command table
    static QHash<QString, FTPCommandFunction> m_ftpCommandTable;    // Authorised user command table
    static QHash<quint16, QString> m_ftpServerResponse;             // Server response table

};

#endif // COGWHEELFTPCORE_H
