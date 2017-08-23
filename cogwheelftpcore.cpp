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

//
// Class: CogWheelFTPCore
//
// Description: Class to provide core FTP command processing. It supports all
// the commands in the rfc959 standard. The command functions are static and have
// two parameters the first which is a pointer to the control channel instance and
// the second a string containing the commands arguments.
//
// Note: Two tables exist that are indexed by command string onto the relevant
// command. The first tabel contains the commands that maybe used in an unauthorised
// mode (minimum) and the second which contains all commands (full) for when a user
// has been authorised either through USER/PASSWORD or logging on anonymously.
//
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelftpcore.h"
#include "cogwheelusersettings.h"

#include <QDir>
#include <QDateTime>

// Unathorised command table (minimum)

QHash<QString, CogWheelFTPCore::FTPCommandFunction> CogWheelFTPCore::m_unauthCommandTable;

// Authorised command table (full)

QHash<QString, CogWheelFTPCore::FTPCommandFunction> CogWheelFTPCore::m_ftpCommandTable;

// rfc3659 commands

QHash<QString, CogWheelFTPCore::FTPCommandFunction> CogWheelFTPCore::m_ftpCommandTable3659;

// Command code message responses (taken from rfc959)

QHash<quint16, QString> CogWheelFTPCore::m_ftpServerResponse;


/**
 * @brief CogWheelFTPCore::CogWheelFTPCore
 *
 * Initialise static tables if not empty.
 *
 * @param parent    Parent object (should be null).
 */
CogWheelFTPCore::CogWheelFTPCore(QObject *parent) : QObject(parent)
{

   initialiseTables();

}

/**
 * @brief CogWheelFTPCore::initialisation
 *
 * Setup FTP internal static tables.
 *
 */
void CogWheelFTPCore::initialiseTables()
{

    // Miniumum command table

    if (CogWheelFTPCore::m_unauthCommandTable.isEmpty()) {
        CogWheelFTPCore::m_unauthCommandTable.insert("USER", CogWheelFTPCore::USER);
        CogWheelFTPCore::m_unauthCommandTable.insert("PASS", CogWheelFTPCore::PASS);
        CogWheelFTPCore::m_unauthCommandTable.insert("TYPE", CogWheelFTPCore::TYPE);
    }

    // Full command table

    if (CogWheelFTPCore::m_ftpCommandTable.isEmpty()) {
        CogWheelFTPCore::m_ftpCommandTable.insert("USER", CogWheelFTPCore::USER);
        CogWheelFTPCore::m_ftpCommandTable.insert("PASS", CogWheelFTPCore::PASS);
        CogWheelFTPCore::m_ftpCommandTable.insert("LIST", CogWheelFTPCore::LIST);
        CogWheelFTPCore::m_ftpCommandTable.insert("FEAT", CogWheelFTPCore::FEAT);
        CogWheelFTPCore::m_ftpCommandTable.insert("SYST", CogWheelFTPCore::SYST);
        CogWheelFTPCore::m_ftpCommandTable.insert("PWD", CogWheelFTPCore::PWD);
        CogWheelFTPCore::m_ftpCommandTable.insert("TYPE", CogWheelFTPCore::TYPE);
        CogWheelFTPCore::m_ftpCommandTable.insert("PORT", CogWheelFTPCore::PORT);
        CogWheelFTPCore::m_ftpCommandTable.insert("CWD", CogWheelFTPCore::CWD);
        CogWheelFTPCore::m_ftpCommandTable.insert("CDUP", CogWheelFTPCore::CDUP);
        CogWheelFTPCore::m_ftpCommandTable.insert("RETR", CogWheelFTPCore::RETR);
        CogWheelFTPCore::m_ftpCommandTable.insert("MODE", CogWheelFTPCore::MODE);
        CogWheelFTPCore::m_ftpCommandTable.insert("NOOP", CogWheelFTPCore::NOOP);
        CogWheelFTPCore::m_ftpCommandTable.insert("STOR", CogWheelFTPCore::STOR);
        CogWheelFTPCore::m_ftpCommandTable.insert("PASV", CogWheelFTPCore::PASV);
        CogWheelFTPCore::m_ftpCommandTable.insert("HELP", CogWheelFTPCore::HELP);
        CogWheelFTPCore::m_ftpCommandTable.insert("SITE", CogWheelFTPCore::SITE);
        CogWheelFTPCore::m_ftpCommandTable.insert("NLST", CogWheelFTPCore::NLST);
        CogWheelFTPCore::m_ftpCommandTable.insert("MKD", CogWheelFTPCore::MKD);
        CogWheelFTPCore::m_ftpCommandTable.insert("RMD", CogWheelFTPCore::RMD);
        CogWheelFTPCore::m_ftpCommandTable.insert("QUIT", CogWheelFTPCore::QUIT);
        CogWheelFTPCore::m_ftpCommandTable.insert("DELE", CogWheelFTPCore::DELE);
        CogWheelFTPCore::m_ftpCommandTable.insert("ACCT", CogWheelFTPCore::ACCT);
        CogWheelFTPCore::m_ftpCommandTable.insert("STOU", CogWheelFTPCore::STOU);
        CogWheelFTPCore::m_ftpCommandTable.insert("STRU", CogWheelFTPCore::STRU);
        CogWheelFTPCore::m_ftpCommandTable.insert("SMNT", CogWheelFTPCore::SMNT);
        CogWheelFTPCore::m_ftpCommandTable.insert("ALLO", CogWheelFTPCore::ALLO);
        CogWheelFTPCore::m_ftpCommandTable.insert("RNFR", CogWheelFTPCore::RNFR);
        CogWheelFTPCore::m_ftpCommandTable.insert("RNTO", CogWheelFTPCore::RNTO);
        CogWheelFTPCore::m_ftpCommandTable.insert("REST", CogWheelFTPCore::REST);
        CogWheelFTPCore::m_ftpCommandTable.insert("ABOR", CogWheelFTPCore::ABOR);
        CogWheelFTPCore::m_ftpCommandTable.insert("REIN", CogWheelFTPCore::REIN);
        CogWheelFTPCore::m_ftpCommandTable.insert("APPE", CogWheelFTPCore::APPE);
        CogWheelFTPCore::m_ftpCommandTable.insert("STAT", CogWheelFTPCore::STAT);
    }

    // Add rfc3659 commands to main table

    if (CogWheelFTPCore::m_ftpCommandTable3659.isEmpty()) {

        CogWheelFTPCore::m_ftpCommandTable3659.insert("MDTM", CogWheelFTPCore::MDTM);
        CogWheelFTPCore::m_ftpCommandTable3659.insert("SIZE", CogWheelFTPCore::SIZE);

        QHashIterator<QString, CogWheelFTPCore::FTPCommandFunction> command(m_ftpCommandTable3659);

        while(command.hasNext()) {
            command.next();
            CogWheelFTPCore::m_ftpCommandTable.insert(command.key(), command.value());
        }

    }

    // Server reponse codes and text

    if (CogWheelFTPCore::m_ftpServerResponse.isEmpty()) {
        CogWheelFTPCore::m_ftpServerResponse.insert(110,"Restart marker reply.");
        CogWheelFTPCore::m_ftpServerResponse.insert(120,"Service ready in nnn minutes.");
        CogWheelFTPCore::m_ftpServerResponse.insert(125,"Data connection already open; transfer starting.");
        CogWheelFTPCore::m_ftpServerResponse.insert(150,"File status okay; about to open data connection.");
        CogWheelFTPCore::m_ftpServerResponse.insert(200,"Command okay.");
        CogWheelFTPCore::m_ftpServerResponse.insert(202,"Command not implemented, superfluous at this site.");
        CogWheelFTPCore::m_ftpServerResponse.insert(211,"System status, nothing to report.");
        CogWheelFTPCore::m_ftpServerResponse.insert(212,"Directory status.");
        CogWheelFTPCore::m_ftpServerResponse.insert(213,"End of status.");
        CogWheelFTPCore::m_ftpServerResponse.insert(214,"Help command successful.");
        CogWheelFTPCore::m_ftpServerResponse.insert(215,"NAME system type.");
        CogWheelFTPCore::m_ftpServerResponse.insert(220,"Service ready for new user.");
        CogWheelFTPCore::m_ftpServerResponse.insert(221,"Service closing control connection.");
        CogWheelFTPCore::m_ftpServerResponse.insert(225,"Data connection open; no transfer in progress.");
        CogWheelFTPCore::m_ftpServerResponse.insert(226,"Closing data connection.");
        CogWheelFTPCore::m_ftpServerResponse.insert(227,"Entering Passive Mode (h1,h2,h3,h4,p1,p2).");
        CogWheelFTPCore::m_ftpServerResponse.insert(230,"User logged in, proceed.");
        CogWheelFTPCore::m_ftpServerResponse.insert(250,"Requested file action okay, completed.");
        CogWheelFTPCore::m_ftpServerResponse.insert(257,"Path was created.");
        CogWheelFTPCore::m_ftpServerResponse.insert(331,"Password required.");
        CogWheelFTPCore::m_ftpServerResponse.insert(332,"Need account for login.");
        CogWheelFTPCore::m_ftpServerResponse.insert(350,"Requested file action pending further information.");
        CogWheelFTPCore::m_ftpServerResponse.insert(421,"Service not available, closing control connection.");
        CogWheelFTPCore::m_ftpServerResponse.insert(425,"Can't open data connection.");
        CogWheelFTPCore::m_ftpServerResponse.insert(426,"Connection closed; transfer aborted.");
        CogWheelFTPCore::m_ftpServerResponse.insert(450,"Requested file action not taken.");
        CogWheelFTPCore::m_ftpServerResponse.insert(451,"Requested action aborted: local error in processing.");
        CogWheelFTPCore::m_ftpServerResponse.insert(452,"Requested action not taken.");
        CogWheelFTPCore::m_ftpServerResponse.insert(500,"Syntax error, command unrecognized.");
        CogWheelFTPCore::m_ftpServerResponse.insert(501,"Syntax error in parameters or arguments.");
        CogWheelFTPCore::m_ftpServerResponse.insert(502,"Command not implemented.");
        CogWheelFTPCore::m_ftpServerResponse.insert(503,"Bad sequence of commands.");
        CogWheelFTPCore::m_ftpServerResponse.insert(504,"Command not implemented for that parameter.");
        CogWheelFTPCore::m_ftpServerResponse.insert(530,"Not logged in.");
        CogWheelFTPCore::m_ftpServerResponse.insert(532,"Need account for storing files.");
        CogWheelFTPCore::m_ftpServerResponse.insert(550,"Requested action not taken.");
        CogWheelFTPCore::m_ftpServerResponse.insert(551,"Requested action aborted: page type unknown.");
        CogWheelFTPCore::m_ftpServerResponse.insert(552,"Requested file action aborted.");
        CogWheelFTPCore::m_ftpServerResponse.insert(553,"Requested action not taken.");
    }
}

/**
 * @brief CogWheelFTPCore::buildListLine
 *
 * Build list line for passed in QFileInfo. The format of which
 * is the same as given for the Linux 'ls -l' command.
 *
 * @param file  File to produce list line for.
 *
 * @return List line string.
 */
QString CogWheelFTPCore::buildListLine(QFileInfo &file)
{
    QString line;
    QString temp;

    if (file.isSymLink()) {
        line.append('l');
    } else if (file.isDir()){
        line.append('d');
    } else {
        line.append('-');
    }

    line.append((file.permissions() & QFile::ReadUser) ? 'r' : '-');
    line.append((file.permissions() & QFile::WriteUser) ? 'w' : '-');
    line.append((file.permissions() & QFile::ExeUser) ? 'x' : '-');
    line.append((file.permissions() & QFile::ReadGroup) ? 'r' : '-');
    line.append((file.permissions() & QFile::WriteGroup) ? 'w' : '-');
    line.append((file.permissions() & QFile::ExeGroup) ? 'x' : '-');
    line.append((file.permissions() & QFile::ReadOther) ? 'r' : '-');
    line.append((file.permissions() & QFile::WriteOther) ? 'w' : '-');
    line.append((file.permissions() & QFile::ExeOther) ? 'x' : '-');

    line.append(" 1 ");

    temp = file.owner();

    if(temp == "") {
        temp = "0";
    }

    line.append(file.owner().leftJustified(10,' ',true));
    line.append(" ");

    // padded by 10 and left justified

    temp = file.group();

    if(temp == "") {
        temp = "0";
    }

    line.append(temp.leftJustified(10,' ',true));
    line.append(" ");

    // padded by 10 and right justified

    temp = QString::number(file.size());
    line.append(temp.rightJustified(10,' ',true));
    line.append(" ");

    // padded by 12 and left justified

    temp = file.lastModified().toString("MMM dd hh:mm");

    line.append(temp.rightJustified(12,' ',true));
    line.append(" ");

    line.append(file.fileName());
    line.append("\r\n");

    return(line);

}

/**
 * @brief CogWheelFTPCore::getResponseText
 *
 * Get string for a given reponse code or "" if one does not exist.
 *
 * @param responseCode   Server response code.
 *
 * @return Reponse code string.
 */
QString CogWheelFTPCore::getResponseText(quint16 responseCode)
{
    if (m_ftpServerResponse.contains(responseCode)) {
        return(m_ftpServerResponse[responseCode]);
    } else {
        return("");
    }
}

/**
 * @brief CogWheelFTPCore::mapPathToLocal
 *
 * Map a FTP root relative path to local filesystem.
 *
 * @param connection   Pointer to control channel instance.
 * @param path         Path to map to local filesystem.
 *
 * @return Local file system path string,
 */
QString CogWheelFTPCore::mapPathToLocal(CogWheelControlChannel *connection, const QString &path)
{

    QString mappedPath;

    if (path.startsWith('/')) {
        mappedPath = connection->rootDirectory()+path;
    } else {
        mappedPath = connection->rootDirectory()+connection->currentWorkingDirectory();
        if (connection->currentWorkingDirectory()=="/") {
            mappedPath += path;
        } else {
            mappedPath += ("/" + path);
        }
    }

    connection->info("Mapping local "+path+" to "+mappedPath);

    return(mappedPath);
}

/**
 * @brief CogWheelFTPCore::mapPathFromLocal
 *
 * Map a given local filesystem path to a FTP root path.
 *
 * @param connection   Pointer to control channel instance.
 * @param path         Path to map from local filesystem.
 *
 * @return  FTP root path.
 */
QString CogWheelFTPCore::mapPathFromLocal(CogWheelControlChannel *connection, const QString &path)
{

    QString mappedPath { QFileInfo(path).absoluteFilePath()};

    connection->info("mapped path : "+mappedPath);

    // Strip off root path

    if (mappedPath.startsWith(connection->rootDirectory())) {
        mappedPath = mappedPath.remove(0,connection->rootDirectory().length());

        // If trying to go above root then reset to root

    } else if (mappedPath.length() < connection->rootDirectory().length()){
        connection->error("Have gone above root so reset.");
        mappedPath = "";
    }

    connection->info("Mapping local from "+path+" to "+mappedPath);

    return(mappedPath);
}

/**
 * @brief CogWheelFTPCore::performCommand
 *
 * Execute FTP command. If the user has been authenicated then then they
 * get a full command list otherwise only a small subset. Note: This is
 * where all command exceptions are handled.
 *
 * @param connection   Pointer to control channel instance.
 * @param command      FTP command.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::performCommand(CogWheelControlChannel *connection, const QString &command, const QString &arguments)
{

    try {

        connection->info("COMMAND : ["+command+"]  ARGUMENT ["+arguments+"]");

        if (m_ftpCommandTable.contains(command)) {

            if (connection->isAuthorized()) {
                FTPCommandFunction commandFn=m_ftpCommandTable[command];
                commandFn(connection, arguments);
            } else if (m_unauthCommandTable.contains(command)) {
                FTPCommandFunction commandFn=m_unauthCommandTable[command];
                commandFn(connection, arguments);
            } else {
                connection->sendReplyCode(530, "Please login with USER and PASS");
            }

        } else {
            connection->error("Unsupported FTP command ["+command+"]");
            connection->sendReplyCode(500);
        }


    } catch(QString err) {
        connection->sendReplyCode(550,err);
    } catch(...) {
        connection->sendReplyCode(550, "Unknown error handling " + command + " command.");
    }

}

/**
 * @brief CogWheelFTPCore::USER
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::USER(CogWheelControlChannel *connection, const QString &arguments)
{

    CogWheelUserSettings    userSettings;

    // Anonymous login

    if (arguments=="anonymous") {

        connection->setAnonymous(true);

    } else if (!CogWheelUserSettings::checkUserName(arguments)) {

        // User does not exist

        connection->sendReplyCode(530, "User name not valid.");
        return;

    }

    userSettings.load(arguments);

    // Set user name, password and write access

    connection->setUserName(userSettings.getUserName());
    connection->setPassword(userSettings.getUserPassword());
    connection->setWriteAccess(userSettings.getWriteAccess());

    // Set intial workign directory

    if (!connection->isAnonymous()) {
        connection->setRootDirectory(userSettings.getRootPath());
    } else {
        connection->setRootDirectory("/tmp");
    }

    // Start at root

    connection->setCurrentWorkingDirectory("");

    // Ask for password

    connection->sendReplyCode(331);

}

/**
 * @brief CogWheelFTPCore::LIST
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::LIST(CogWheelControlChannel *connection, const QString &arguments)
{

    QString path { mapPathToLocal(connection, arguments) } ;
    QFileInfo fileInfo { path };

    if (!fileInfo.exists()) {
        connection->sendReplyCode(550, "Requested path not found.");
        return;
    }

    if (connection->connectDataChannel()) {

        QString listing;

        if (fileInfo.isDir()) {
            QDir listDirectory { path };
            listDirectory.setFilter(listDirectory.filter() | QDir::Hidden);
            for (QFileInfo &item : listDirectory.entryInfoList()) {
                listing.append(buildListLine(item));
            }
        } else {
            listing.append(buildListLine(fileInfo));
        }

        connection->sendOnDataChannel(listing.toUtf8().data());
        connection->disconnectDataChannel();

    }

}

/**
 * @brief CogWheelFTPCore::FEAT
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::FEAT(CogWheelControlChannel *connection, const QString &arguments)
{

    Q_UNUSED(arguments);

    QString featReply;

    featReply.append("211-Extensions supported: \r\n");

    for( auto key :  m_ftpCommandTable3659.keys() ) {
        featReply.append(" "+key+"\r\n");

    }

    connection->sendOnControlChannel(featReply);

    connection->sendReplyCode(211, "END.");

}

/**
 * @brief CogWheelFTPCore::SYST
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::SYST(CogWheelControlChannel *connection, const QString &arguments)
{

    Q_UNUSED(arguments);

    connection->sendReplyCode(215, "UNIX Type: CogWheel");

}

/**
 * @brief CogWheelFTPCore::PWD
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::PWD(CogWheelControlChannel *connection, const QString &arguments)
{

    Q_UNUSED(arguments);

    connection->info("PWD "+connection->currentWorkingDirectory());

    connection->sendReplyCode (257, "\""+connection->currentWorkingDirectory()+"\"");

}

/**
 * @brief CogWheelFTPCore::TYPE
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::TYPE(CogWheelControlChannel *connection, const QString &arguments)
{
    connection->setTransferType(arguments[0]);  // Just keep first character for the record
    connection->sendReplyCode(200);
}

/**
 * @brief CogWheelFTPCore::PORT
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::PORT(CogWheelControlChannel *connection, const QString &arguments)
{

    connection->setHostPortForDataChannel(arguments.split(','));
    connection->sendReplyCode(200);

}

/**
 * @brief CogWheelFTPCore::CWD
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::CWD(CogWheelControlChannel *connection, const QString &arguments)
{

    QString cwdPath = mapPathToLocal(connection, arguments);
    QDir path { cwdPath };

    if(!path.exists()) {
        connection->sendReplyCode(550, "Requested path not found.");
    } else {
        connection->setCurrentWorkingDirectory(mapPathFromLocal(connection, cwdPath));
        connection->sendReplyCode(250);
    }

}

/**
 * @brief CogWheelFTPCore::PASS
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::PASS(CogWheelControlChannel *connection, const QString &arguments)
{

    // For non-anonymous check users password

    if (!connection->isAnonymous()) {
        if (!CogWheelUserSettings::checkUserPassword(connection->password(), arguments)) {
            connection->sendReplyCode(530); // Failure
            return;
        }
        connection->sendReplyCode(230); // Success then login
    } else {
        connection->sendReplyCode(230, "Logged in Anonymous as "+arguments);
    }

    connection->setAuthorized(true);

}

/**
 * @brief CogWheelFTPCore::CDUP
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::CDUP(CogWheelControlChannel *connection, const QString &arguments)
{

    QDir path  { mapPathToLocal(connection, arguments) };

    if(!path.cdUp()) {
        connection->sendReplyCode(550, "Requested path not found.");
    } else {
        connection->setCurrentWorkingDirectory(mapPathFromLocal(connection, path.absolutePath()));
        connection->sendReplyCode(250);
    }

}

/**
 * @brief CogWheelFTPCore::RETR
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::RETR(CogWheelControlChannel *connection, const QString &arguments)
{

    QFile file { mapPathToLocal(connection, arguments) } ;

    if(!file.exists()){
        connection->sendReplyCode(450);
        return;
    }

    QFileInfo fileInfo { file };

    if(fileInfo.isDir()){
        connection->sendReplyCode(450, "Requested file is a directory.");
        return;
    }

    if (connection->connectDataChannel()) {
        connection->downloadFileFromDataChannel(mapPathToLocal(connection, arguments ));
    }

}

/**
 * @brief CogWheelFTPCore::NOOP
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::NOOP(CogWheelControlChannel *connection, const QString &arguments)
{

    Q_UNUSED(arguments);

    connection->sendReplyCode(200);
}

/**
 * @brief CogWheelFTPCore::MODE
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::MODE(CogWheelControlChannel *connection, const QString &arguments)
{

    connection->setTransferMode(arguments[0]);
    connection->sendReplyCode(200);
}

/**
 * @brief CogWheelFTPCore::STOR
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::STOR(CogWheelControlChannel *connection, const QString &arguments)
{

    if (!connection->writeAccess()) {
        connection->sendReplyCode(550,"User needs write access to perform command");
        return;
    }

    QFile file { mapPathToLocal(connection,arguments) } ;

    if(file.exists()) {
        if(!file.remove()) {
            connection->sendReplyCode(551, "File could not be overwritten");
            return;

        }
    }

    if (connection->connectDataChannel()) {
        connection->uploadFileToDataChannel( mapPathToLocal(connection,arguments) );
    }

}

/**
 * @brief CogWheelFTPCore::PASV
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::PASV(CogWheelControlChannel *connection, const QString &arguments)
{

    Q_UNUSED(arguments);

    connection->setPassive(true);
    connection->listenForConnectionOnDataChannel();

}

/**
 * @brief CogWheelFTPCore::HELP
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::HELP(CogWheelControlChannel *connection, const QString &arguments)
{

    Q_UNUSED(arguments);

    QString helpReply;
    int column=0;

    helpReply.append("214-The following commands are available:\r\n");

    for( auto key :  m_ftpCommandTable.keys() ) {
        helpReply.append(" "+key);
        if (column++ == 8) {
            helpReply.append("\r\n");
            column=0;
        }
    }
    helpReply.append("\r\n");
    connection->sendOnControlChannel(helpReply);

    connection->sendReplyCode(214, "Help OK.");
}

/**
 * @brief CogWheelFTPCore::SITE
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::SITE(CogWheelControlChannel *connection, const QString &arguments)
{

    Q_UNUSED(arguments);

    connection->sendReplyCode(202);
}

/**
 * @brief CogWheelFTPCore::NLST
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::NLST(CogWheelControlChannel *connection, const QString &arguments)
{

    QString path { mapPathToLocal(connection, arguments) };
    QFileInfo fileInfo { path };

    if (!fileInfo.exists() || !fileInfo.isDir()) {
        connection->sendReplyCode(550, "Requested path not found.");
    }

    if (connection->connectDataChannel()) {

        QString listing;
        QDir listDirectory { path };

        for (QString item : listDirectory.entryList()) {
            listing.append(item+"\r\n");
        }

        connection->sendOnDataChannel(listing.toUtf8().data());
        connection->disconnectDataChannel();

    }

}

/**
 * @brief CogWheelFTPCore::MKD
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::MKD(CogWheelControlChannel *connection, const QString &arguments)
{

    if (!connection->writeAccess()) {
        connection->sendReplyCode(550,"User needs write access to perform command");
        return;
    }

    QString path { mapPathToLocal(connection, arguments) };
    QDir newDirectory { path };

    if(!newDirectory.mkdir(path)){
        connection->sendReplyCode(530);
    }else{
        connection->sendReplyCode(257);
    }

}

/**
 * @brief CogWheelFTPCore::RMD
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::RMD(CogWheelControlChannel *connection, const QString &arguments)
{

    if (!connection->writeAccess()) {
        connection->sendReplyCode(550,"User needs write access to perform command");
        return;
    }

    QString path { mapPathToLocal(connection, arguments) };
    QDir directoryToDelete { path  };

    if(directoryToDelete.exists()){

        if(directoryToDelete.rmdir(path)){
            connection->sendReplyCode(250);
        }else{
            connection->sendReplyCode(550,"Could not delete directory!");
        }

    } else{
        connection->sendReplyCode(550,"Directory not found!");
    }

}

/**
 * @brief CogWheelFTPCore::QUIT
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::QUIT(CogWheelControlChannel *connection, const QString &arguments)
{

    Q_UNUSED(arguments);

    connection->sendReplyCode(221);
    connection->closeConnection();

}

/**
 * @brief CogWheelFTPCore::DELE
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::DELE(CogWheelControlChannel *connection, const QString &arguments)
{

    if (!connection->writeAccess()) {
        connection->sendReplyCode(550,"User needs write access to perform command");
        return;
    }

    QFile fileToDelete { mapPathToLocal(connection, arguments) };

    if(fileToDelete.exists()){

        if(fileToDelete.remove()) {
            connection->sendReplyCode(250);
        } else {
            connection->sendReplyCode(550,"Could not delete file!");
        }
    } else {
        connection->sendReplyCode(550,"File not found!");
    }

}

/**
 * @brief CogWheelFTPCore::ACCT
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::ACCT(CogWheelControlChannel *connection, const QString &arguments)
{

    connection->setAccountName(arguments);
    connection->sendReplyCode(200);

}

/**
 * @brief CogWheelFTPCore::STOU
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::STOU(CogWheelControlChannel *connection, const QString &arguments)
{

    if (!connection->writeAccess()) {
        connection->sendReplyCode(550,"User needs write access to perform command");
        return;
    }

    QString path { mapPathToLocal(connection, arguments) };
    QFile file { path  } ;

    if(file.exists()) {
        connection->sendReplyCode(551, "File already exists.");
        return;
    }

    if (connection->connectDataChannel()) {
        connection->uploadFileToDataChannel(path);
    }

}

/**
 * @brief CogWheelFTPCore::SMNT
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::SMNT(CogWheelControlChannel *connection, const QString &arguments)
{

    if(connection->isAllowSMNT()) {

        QDir newRootDirectory(arguments);

        if(newRootDirectory.exists()){
            connection->setRootDirectory(newRootDirectory.absolutePath());
            connection->sendReplyCode(250, "SMNT command successful.");
        } else {
            connection->sendReplyCode(550, "Could not change root directory");
        }

    }else{
        connection->sendReplyCode(550, "SMNT is not allowed.");
    }
}

/**
 * @brief CogWheelFTPCore::STRU
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::STRU(CogWheelControlChannel *connection, const QString &arguments)
{
    connection->setFileStructure(arguments[0]);
    connection->sendReplyCode(200);
}

/**
 * @brief CogWheelFTPCore::ALLO
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::ALLO(CogWheelControlChannel *connection, const QString &arguments)
{

    Q_UNUSED(arguments);

    connection->sendReplyCode(200);
}

/**
 * @brief CogWheelFTPCore::RNFR
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::RNFR(CogWheelControlChannel *connection, const QString &arguments)
{

    if (!connection->writeAccess()) {
        connection->sendReplyCode(550,"User needs write access to perform command");
        return;
    }

    QString path { mapPathToLocal(connection, arguments) };

    connection->setRenameFromFileName("");

    QFileInfo fileToRename { path };
    if(!fileToRename.exists()){
        connection->sendReplyCode(550, "Could not find '" + arguments + "'");
    } else{
        connection->setRenameFromFileName(path);
        connection->sendReplyCode(350);
    }

}

/**
 * @brief CogWheelFTPCore::RNTO
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::RNTO(CogWheelControlChannel *connection, const QString &arguments)
{

    if (!connection->writeAccess()) {
        connection->sendReplyCode(550,"User needs write access to perform command");
        return;
    }

    if(connection->renameFromFileName() == "") {
        connection->sendReplyCode(503);
        return;
    }

    QFile sourceName(connection->renameFromFileName());

    if(sourceName.rename(mapPathToLocal(connection, arguments) )){
        connection->sendReplyCode(250);
    }else{
        connection->sendReplyCode(553);
    }

    connection->setRenameFromFileName("");
}

/**
 * @brief CogWheelFTPCore::REST
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::REST(CogWheelControlChannel *connection, const QString &arguments)
{

    if(connection->renameFromFileName() == "") {
        connection->sendReplyCode(550);
        return;
    }

    bool validInteger;

    connection->setRestoreFilePostion(arguments.toInt(&validInteger));

    if(validInteger){
        connection->sendReplyCode(350,"Restarting at "+arguments+". Send STORE or RETRIEVE");
    } else{
        connection->setRestoreFilePostion(0);
        connection->sendReplyCode(500);
    }

}

/**
 * @brief CogWheelFTPCore::ABOR
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::ABOR(CogWheelControlChannel *connection, const QString &arguments)
{

    Q_UNUSED(arguments);

    connection->abortOnDataChannel();

}


/**
 * @brief CogWheelFTPCore::REIN
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::REIN(CogWheelControlChannel *connection, const QString &arguments)
{

    Q_UNUSED(arguments);

    connection->setAccountName("");
    connection->setAllowSMNT(false);
    connection->setAnonymous(false);
    connection->setAuthorized(false);
    connection->setClientHostIP("");
    connection->setCurrentWorkingDirectory("");
    connection->setPassive(false);
    connection->setPassword("");
    connection->setRenameFromFileName("");
    connection->setRestoreFilePostion(0);
    connection->setRootDirectory("");
    connection->setServerIP("");
    connection->setUserName("");

    connection->sendReplyCode(250);

}

/**
 * @brief CogWheelFTPCore::APPE
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::APPE(CogWheelControlChannel *connection, const QString &arguments)
{

    if (!connection->writeAccess()) {
        connection->sendReplyCode(550,"User needs write access to perform command");
        return;
    }

    if (connection->connectDataChannel()) {
        connection->uploadFileToDataChannel(mapPathToLocal(connection,arguments) );
    }

}

/**
 * @brief CogWheelFTPCore::STAT
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::STAT(CogWheelControlChannel *connection, const QString &arguments)
{

    if(!arguments.isEmpty()) {

        connection->sendOnControlChannel("213-Status of " + arguments + "\r\n");

        QDir pathToList(mapPathToLocal(connection, arguments));

        if(pathToList.exists()) {
            for (auto item : pathToList.entryInfoList()){
                connection->sendOnControlChannel(buildListLine(item));
            }
        }else{
            connection->sendOnControlChannel(arguments+" does not exist.\r\n");
        }

        connection->sendReplyCode(213);
        return;
    }


    // We are in a transfer

    if(connection->dataChannel()) {
        connection->sendReplyCode(211, "Transfering data.");
        return;
    }

    // No File transfer and no argument

    if(!connection->dataChannel() && arguments.isEmpty()) {
        connection->sendOnControlChannel("213- "+ connection->serverName() + " (" + connection->serverIP()+ ") FTP Server Status:" + "\r\n");
        connection->sendOnControlChannel("Version "+ connection->serverVersion() + "\r\n");
        connection->sendOnControlChannel("Connected from "+connection->clientHostIP()+"\r\n");
        if (connection->isAnonymous()) {
            connection->sendOnControlChannel("Logged in anonymously \r\n");
        } else {
            connection->sendOnControlChannel("Logged in as user " + connection->userName() + "\r\n");
        }
        if (connection->dataChannel()==nullptr) {
            connection->sendOnControlChannel("No data connection.\r\n");
        }else {
            connection->sendOnControlChannel("Trasferring data.\r\n");
        }
        connection->sendReplyCode(213);
    }

}

/**
 * @brief CogWheelFTPCore::MDTM
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::MDTM(CogWheelControlChannel *connection, const QString &arguments)
{

    QString file { mapPathToLocal(connection, arguments) } ;
    QFileInfo fileInfo { file };

    if (!fileInfo.exists()) {
        connection->sendReplyCode(550, "Requested file not found.");
        return;
    }

    if (!fileInfo.lastModified().toString("zzz").isEmpty()){
        connection->sendReplyCode(213, fileInfo.lastModified().toString("yyyyMMddhhmmss.zzz"));
    } else {
        connection->sendReplyCode(213, fileInfo.lastModified().toString("yyyyMMddhhmmss"));
    }


}

/**
 * @brief CogWheelFTPCore::SIZE
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::SIZE(CogWheelControlChannel *connection, const QString &arguments)
{
    QString file { mapPathToLocal(connection, arguments) } ;
    QFileInfo fileInfo { file };

    if (!fileInfo.exists()) {
        connection->sendReplyCode(550, "Requested file not found.");
        return;
    }

    qDebug() << "File [" << file << "] Size [" << QString::number(fileInfo.size()) << "]";

    connection->sendReplyCode(213, QString::number(fileInfo.size()));

}



