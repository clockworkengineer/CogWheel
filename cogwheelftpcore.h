/*
 * File:   cogwheelftpcore.h
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

//
// Class: CogWheelFTPCore
//
// Description: Class to provide core FTP command processing. It supports all
// the commands in the rfc959 standard. The command functions are static and have
// two parameters the first which is a pointer to the control channel instance and
// the second a string containing the commands arguments.
//
// Two tables exist that are indexed by command string onto the relevant
// command. The first tabel contains the commands that maybe used in an unauthorised
// mode (minimum) and the second which contains all commands (full) for when a user
// has been authorised either through USER/PASSWORD or logging on anonymously.
//
// A third table contains FTP command extensions and that are kept separate but entries
// are copied to the main table on initialisation.
//
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelcontrolchannel.h"
#include "cogwheelusersettings.h"
#include "cogwheelftpserverreply.h"

#include <QHash>
#include <QString>

// =================
// CLASS DECLARATION
// =================

class CogWheelFTPCore
{

public:

    // Class exception

    struct Exception : public std::runtime_error {

        Exception(const QString & messageStr)
            : std::runtime_error(QString("CogWheelFTPCore Failure: " + messageStr).toStdString()) {
        }

    };

    // Constructor / Destructor

    explicit CogWheelFTPCore();
    ~CogWheelFTPCore();

    // Load server settings and setup

    static void setupServer(const CogWheelServerSettings &serverSettings);

    // Response table accessor

    static QString getResponseText(quint16 responseCode);

    // Perform FTP command

    static void performCommand(CogWheelControlChannel *connection, const QString &command, const QString &arguments);

private:

    // FTP command function pointer

    typedef std::function<void (CogWheelControlChannel *, const QString &) > FTPCommandFunction;

    // Setup tables

    static void loadServerReponseTables();
    static void loadFTPCommandTables();

    // FTP commands (RFC959)

    static void USER(CogWheelControlChannel *connection, const QString &arguments);
    static void LIST(CogWheelControlChannel *connection, const QString &arguments);
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

    // Extended FTP commands (RFC3659, RFC2389)

    static void FEAT(CogWheelControlChannel *connection, const QString &arguments);
    static void MDTM(CogWheelControlChannel *connection, const QString &arguments);
    static void SIZE(CogWheelControlChannel *connection, const QString &arguments);
    static void AUTH(CogWheelControlChannel *connection, const QString &arguments);
    static void PROT(CogWheelControlChannel *connection, const QString &arguments);
    static void PBSZ(CogWheelControlChannel *connection, const QString &arguments);
    static void MLSD(CogWheelControlChannel *connection, const QString &arguments);
    static void MLST(CogWheelControlChannel *connection, const QString &arguments);

private:

    static QHash<QString, FTPCommandFunction> m_unauthCommandTable;       // Unauthorised user command table
    static QHash<QString, FTPCommandFunction> m_ftpCommandTable;          // Authorised user command table
    static QHash<QString, FTPCommandFunction> m_ftpCommandTableExtended;  // Extended command table

    static QHash<quint16, QString> m_ftpServerResponse;   // Server response code table
    static QHash<QString,QString> m_featTailoredRespone;  // Tailored FEAT command responses

    static CogWheelServerSettings m_serverSettings;       // FTP server settinngs

};

#endif // COGWHEELFTPCORE_H
