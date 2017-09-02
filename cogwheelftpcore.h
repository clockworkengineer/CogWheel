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

#include "cogwheelcontrolchannel.h"
#include "cogwheelusersettings.h"

#include <stdexcept>

#include <QMap>
#include <QString>
#include <QFileInfo>

class CogWheelFTPCore
{

      //
      // Used to signal a program error and to trigger a reply to server. Three constructors
      // are provided for object creation with the default code of 550 being used if none is supplied
      // and a message taken from the built in response table if that isn't passed.
      //

      struct FtpServerErrorReply : public std::exception {

          FtpServerErrorReply (quint16 responseCode) noexcept : responseCode(responseCode) { message = getResponseText(responseCode); }
          FtpServerErrorReply (quint16 responseCode, QString const& message) noexcept :  message(message), responseCode(responseCode) { }
          FtpServerErrorReply (QString const& message) noexcept  :  message(message)  { }

      public:
          QString getMessage() const { return message; }
          quint16 getResponseCode() const { return responseCode; }

      private:
          QString message;
          quint16 responseCode=550; // Default code: error action not taken.

      };

    // FTP command function pointer

    typedef std::function<void (CogWheelControlChannel *, const QString &) > FTPCommandFunction;

public:

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

    // Setup tables

    static void loadServerReponseTable();
    static void loadFTPCommandTables();

    // Build list file line

    static QString buildListLine(QFileInfo &file);

    // File path mapping functions

    static QString mapPathToLocal(CogWheelControlChannel *connection, const QString& path);
    static QString mapPathFromLocal(CogWheelControlChannel *connection, const QString& path);

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

private:

    static QHash<QString, FTPCommandFunction> m_unauthCommandTable;       // Unauthorised user command table
    static QHash<QString, FTPCommandFunction> m_ftpCommandTable;          // Authorised user command table
    static QHash<QString, FTPCommandFunction> m_ftpCommandTableExtended;  // Extended command table

    static QHash<quint16, QString> m_ftpServerResponse;   // Server response code table

    static QHash<QString,QString> m_featTailoredRespone;  // Tailored FEAT command responses

    static CogWheelServerSettings m_serverSettings; // FTP server settinngs

};

#endif // COGWHEELFTPCORE_H
