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

    // FTP command function pointer

    typedef std::function<void (CogWheelControlChannel *, const QString &) > FTPCommandFunction;

public:

    // Constructor

    explicit CogWheelFTPCore(QObject *parent = nullptr);

    // Response table accessor

    static QString getResponseText(quint16 responseCode);

    // Perform FTP command

    static void performCommand(CogWheelControlChannel *connection, const QString &command, const QString &arguments);

private:

    // Build list file line

    static QString buildListLine(QFileInfo &file);

    // File path mapping functions

    static QString mapPathToLocal(CogWheelControlChannel *connection, const QString& path);
    static QString mapPathFromLocal(CogWheelControlChannel *connection, const QString& path);

    // FTP commands

    static void USER(CogWheelControlChannel *connection, const QString &arguments);
    static void LIST(CogWheelControlChannel *connection, const QString &arguments);
    static void FEAT(CogWheelControlChannel *connection, const QString &arguments);
    static void SYST(CogWheelControlChannel *connection, const QString &arguments);
    static void PWD(CogWheelControlChannel *connection, const QString &arguments);
    static void TYPE(CogWheelControlChannel *connection, const QString &arguments);
    static void PORT(CogWheelControlChannel *connection, const QString &arguments);
    static void CWD(CogWheelControlChannel *connection, const QString &arguments);
    static void PASS(CogWheelControlChannel *connection, const QString &arguments);
    static void CDUP(CogWheelControlChannel *connection, const QString &arguments);
    static void RETR(CogWheelControlChannel *connection, const QString &arguments);
    static void NOOP(CogWheelControlChannel *connection, const QString &arguments);
    static void MODE(CogWheelControlChannel *connection, const QString &arguments);
    static void STOR(CogWheelControlChannel *connection, const QString &arguments);
    static void PASV(CogWheelControlChannel *connection, const QString &arguments);
    static void HELP(CogWheelControlChannel *connection, const QString &arguments);
    static void STAT(CogWheelControlChannel *connection, const QString &arguments);
    static void SITE(CogWheelControlChannel *connection, const QString &arguments);
    static void NLST(CogWheelControlChannel *connection, const QString &arguments);
    static void MKD(CogWheelControlChannel *connection, const QString &arguments);
    static void RMD(CogWheelControlChannel *connection, const QString &arguments);
    static void DELE(CogWheelControlChannel *connection, const QString &arguments);
    static void ACCT(CogWheelControlChannel *connection, const QString &arguments);
    static void SMNT(CogWheelControlChannel *connection, const QString &arguments);
    static void QUIT(CogWheelControlChannel *connection, const QString &arguments);
    static void REIN(CogWheelControlChannel *connection, const QString &arguments);
    static void STRU(CogWheelControlChannel *connection, const QString &arguments);
    static void STOU(CogWheelControlChannel *connection, const QString &arguments);
    static void APPE(CogWheelControlChannel *connection, const QString &arguments);
    static void ALLO(CogWheelControlChannel *connection, const QString &arguments);
    static void REST(CogWheelControlChannel *connection, const QString &arguments);
    static void RNFR(CogWheelControlChannel *connection, const QString &arguments);
    static void RNTO(CogWheelControlChannel *connection, const QString &arguments);
    static void ABOR(CogWheelControlChannel *connection, const QString &arguments);

signals:

    void error(const QString &errorMessage);

public slots:

public:

    static QHash<QString, FTPCommandFunction> m_unauthCommandTable; // Unauthorised user command table
    static QHash<QString, FTPCommandFunction> m_ftpCommandTable;    // Authorised user command table
    static QHash<quint16, QString> m_ftpServerResponse;             // Server response table

};

#endif // COGWHEELFTPCORE_H
