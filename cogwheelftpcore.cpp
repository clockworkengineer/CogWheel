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
// A third table has been added for FTP command extensions and kept separate but entries
// are copied to the main table on initialisation.
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

// Extended command table

QHash<QString, CogWheelFTPCore::FTPCommandFunction> CogWheelFTPCore::m_ftpCommandTableExtended;

// Command code message responses (taken from rfc959)

QHash<quint16, QString> CogWheelFTPCore::m_ftpServerResponse;

/**
 * @brief CogWheelFTPCore::CogWheelFTPCore
 *
 * Initialise static tables if not empty.
 *
 */
CogWheelFTPCore::CogWheelFTPCore()
{

    loadFTPCommandTables();
    loadServerReponseTable();

}

/**
 * @brief CogWheelFTPCore::loadServerReponseTable
 *
 * Load server response table.
 *
 */
void CogWheelFTPCore::loadServerReponseTable()
{

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
 * @brief CogWheelFTPCore::loadFTPCommandTable
 *
 * Load main FTP command table.
 *
 */
void CogWheelFTPCore::loadFTPCommandTables()
{

    // Miniumum command table

    if (CogWheelFTPCore::m_unauthCommandTable.isEmpty()) {
        CogWheelFTPCore::m_unauthCommandTable.insert("USER", CogWheelFTPCore::USER);
        CogWheelFTPCore::m_unauthCommandTable.insert("PASS", CogWheelFTPCore::PASS);
        CogWheelFTPCore::m_unauthCommandTable.insert("TYPE", CogWheelFTPCore::TYPE);
        CogWheelFTPCore::m_unauthCommandTable.insert("AUTH", CogWheelFTPCore::AUTH);
    }

    // Full command table

    if (CogWheelFTPCore::m_ftpCommandTable.isEmpty()) {
        CogWheelFTPCore::m_ftpCommandTable.insert("USER", CogWheelFTPCore::USER);
        CogWheelFTPCore::m_ftpCommandTable.insert("PASS", CogWheelFTPCore::PASS);
        CogWheelFTPCore::m_ftpCommandTable.insert("LIST", CogWheelFTPCore::LIST);
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

    // Add extended commands to main table

    if (CogWheelFTPCore::m_ftpCommandTableExtended.isEmpty()) {

        CogWheelFTPCore::m_ftpCommandTableExtended.insert("FEAT", CogWheelFTPCore::FEAT);
        CogWheelFTPCore::m_ftpCommandTableExtended.insert("MDTM", CogWheelFTPCore::MDTM);
        CogWheelFTPCore::m_ftpCommandTableExtended.insert("SIZE", CogWheelFTPCore::SIZE);
        CogWheelFTPCore::m_ftpCommandTableExtended.insert("AUTH", CogWheelFTPCore::AUTH);
        CogWheelFTPCore::m_ftpCommandTableExtended.insert("PROT", CogWheelFTPCore::PROT);
        CogWheelFTPCore::m_ftpCommandTableExtended.insert("PBSZ", CogWheelFTPCore::PBSZ);

        QHashIterator<QString, CogWheelFTPCore::FTPCommandFunction> command(m_ftpCommandTableExtended);
        while(command.hasNext()) {
            command.next();
            CogWheelFTPCore::m_ftpCommandTable.insert(command.key(), command.value());
        }

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

            FTPCommandFunction commandFn;

            if (connection->isAuthorized()) {
                commandFn=m_ftpCommandTable[command];
            } else if (m_unauthCommandTable.contains(command)) {
                commandFn=m_unauthCommandTable[command];
            } else {
                throw FtpServerErrorReply(530, "Please login with USER and PASS.");
            }

            Q_ASSERT(commandFn!=nullptr);

            if (commandFn) {
                commandFn(connection, arguments);
            }

        } else {
            connection->warning("Unsupported FTP command ["+command+"]");
            throw FtpServerErrorReply(500);
        }

    } catch (FtpServerErrorReply response)  {
        connection->sendReplyCode(response.getResponseCode(),response.getMessage());

    } catch(...) {
        connection->error("Unknown error handling " + command + " command.");
        connection->sendReplyCode(550, "Unknown error handling " + command + " command.");
    }

}

// ======
// RFC959
// ======

/**
 * @brief CogWheelFTPCore::USER
 *
 * Login to server with a given user name. If the user is anonymous then the login set
 * is set to anonymous. If the user name does not exist on the server then and error
 * response wil be returned otherwise a password required response (331). The standard
 * indicates that this command may be used at any time by the client to change user but
 * this server does not support this action.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::USER(CogWheelControlChannel *connection, const QString &arguments)
{

    CogWheelUserSettings    userSettings;

    // We asre already logged in

    if (connection->isAuthorized()) {
        throw FtpServerErrorReply(530, "User already logged in.");
    }

    // Anonymous login

    if (arguments=="anonymous") {

        connection->setAnonymous(true);

    } else if (!CogWheelUserSettings::checkUserName(arguments)) {

        // User does not exist

        throw FtpServerErrorReply(530, "User name not valid.");

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
 * Obtain a directory listing of the current working directory or
 * file or directory passed as a parameter and send it over the data chanel.
 * The listng is in a format that is similar to the Linux shell command
 * 'ls -l'. An error (550) is sent to the client if any path or file passed
 * in as argument does not exist.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::LIST(CogWheelControlChannel *connection, const QString &arguments)
{

    QString path { mapPathToLocal(connection, arguments) } ;
    QFileInfo fileInfo { path };

    // Argument does not exist

    if (!fileInfo.exists()) {
        throw FtpServerErrorReply("Requested path not found.");
    }

    // Connect up data channel

    if (connection->connectDataChannel()) {

        QString listing;

        // List files for directory

        if (fileInfo.isDir()) {
            QDir listDirectory { path };
            listDirectory.setFilter(listDirectory.filter() | QDir::Hidden);
            for (QFileInfo &item : listDirectory.entryInfoList()) {
                listing.append(buildListLine(item));
            }

            // List a single file

        } else {
            listing.append(buildListLine(fileInfo));
        }

        // Send listing to client and close data channel

        connection->sendOnDataChannel(listing.toUtf8().data());
        connection->disconnectDataChannel();

    }

}

/**
 * @brief CogWheelFTPCore::SYST
 *
 * Return a reply that indicates the host system on which the server runs. This is currently
 * Unix but may change in th future.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::SYST(CogWheelControlChannel *connection, const QString &arguments)
{

    Q_UNUSED(arguments);

    connection->sendReplyCode(215, "UNIX Type: CogWheel");  // MAY NEED TO CHANGE FOR NEW HOSTS

}

/**
 * @brief CogWheelFTPCore::PWD
 *
 * Return to the client the current woring directory.
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
 * Specify the data type that is be be transfered. At present this parameter is ignored as
 * the server will only do binary (image). This may change in the future.
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
 * Command sent by client to indicate that the data channel is going to be used in active mode.
 * The host ip and port are specified by the argument h1,h2,h3,h4,p1,p2 which is split into a
 * string list of length 6.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::PORT(CogWheelControlChannel *connection, const QString &arguments)
{

    // Signal to data channel mode to be used

    connection->setHostPortForDataChannel(arguments.split(','));
    connection->sendReplyCode(200);

}

/**
 * @brief CogWheelFTPCore::CWD
 *
 * Set the current working directory. Returning an error (550) if it does not
 * exist on the local filesystem.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::CWD(CogWheelControlChannel *connection, const QString &arguments)
{

    QString cwdPath = mapPathToLocal(connection, arguments);
    QDir path { cwdPath };

    if(!path.exists()) {
        throw FtpServerErrorReply("Requested path not found.");
    } else {
        connection->setCurrentWorkingDirectory(mapPathFromLocal(connection, cwdPath));
        connection->sendReplyCode(250);
    }

}

/**
 * @brief CogWheelFTPCore::PASS
 *
 * Validate a users password. If the login is not anonymous and the password is not the
 * the same as the user specified in the last USER command then return an error otherwise
 * set conenction status to authorised.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::PASS(CogWheelControlChannel *connection, const QString &arguments)
{

    // For non-anonymous check users password

    if (!connection->isAnonymous()) {
        if (!CogWheelUserSettings::checkUserPassword(connection->password(), arguments)) {
            throw FtpServerErrorReply(530); // Failure
        }
        connection->sendReplyCode(230);     // Success then login
    } else {
        connection->sendReplyCode(230, "Logged in Anonymous as "+arguments);
    }

    connection->setAuthorized(true);

}

/**
 * @brief CogWheelFTPCore::CDUP
 *
 * Change the current working directory to the currents parent in the file structure (ie. cd ..).
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::CDUP(CogWheelControlChannel *connection, const QString &arguments)
{

    QDir path  { mapPathToLocal(connection, arguments) };

    if(!path.cdUp()) {
        throw FtpServerErrorReply("Requested path not found.");
    } else {
        connection->setCurrentWorkingDirectory(mapPathFromLocal(connection, path.absolutePath()));
        connection->sendReplyCode(250);
    }

}

/**
 * @brief CogWheelFTPCore::RETR
 *
 * Download a specified file from the server. Returns an error response
 * to client if the file does not exist or is not a file.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::RETR(CogWheelControlChannel *connection, const QString &arguments)
{

    QFile file { mapPathToLocal(connection, arguments) } ;

    if(!file.exists()){
        throw FtpServerErrorReply(450);
    }

    QFileInfo fileInfo { file };

    if(!fileInfo.isFile()){
        throw FtpServerErrorReply(450, "Requested object is not a file.");
    }

    if (connection->connectDataChannel()) {
        connection->downloadFileFromDataChannel(mapPathToLocal(connection, arguments ));
    }

}

/**
 * @brief CogWheelFTPCore::NOOP
 *
 * Do nothing command (used to keep control channel open).
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
 * Set file send/receive transter mode. This server uses stream which is the default
 * and cannot be changed. The value passed in is just stored away for reference.
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
 * Upload the specified from client to server. An error is sent to client if the user
 * does not have write access (user setting) or the desination fiel already exists.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::STOR(CogWheelControlChannel *connection, const QString &arguments)
{

    // User does not have write access to server

    if (!connection->writeAccess()) {
        throw FtpServerErrorReply("User needs write access to perform command.");
    }

    // Check destination does not exist

    QFile file { mapPathToLocal(connection,arguments) } ;

    // Remove file if exists

    if(file.exists()) {
        if(!file.remove()) {
            throw FtpServerErrorReply(551, "File could not be overwritten.");
        }
    }

    // Open data channel and upload file.

    if (connection->connectDataChannel()) {
        connection->uploadFileToDataChannel( mapPathToLocal(connection,arguments) );
    }

}

/**
 * @brief CogWheelFTPCore::PASV
 *
 * Data channel is going to be used in passive mode so make data channel listen for
 * connections from client.
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
 * List all commands that the server supports. At present the variant that passes in an argument
 * specifying help and an individual command is not supported.
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
    if (column!=0)helpReply.append("\r\n");
    connection->sendOnControlChannel(helpReply);

    connection->sendReplyCode(214, "Help OK.");
}

/**
 * @brief CogWheelFTPCore::SITE
 *
 * No Site specific commands (so not implemented).
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
 * Produce  a list of files from the passed in path and send over data channel.
 * An error is returned to the client of the passed in path is not a directory
 * or does not exist.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::NLST(CogWheelControlChannel *connection, const QString &arguments)
{

    QString path { mapPathToLocal(connection, arguments) };
    QFileInfo fileInfo { path };

    // Check for directory that exists

    if (!fileInfo.exists() || !fileInfo.isDir()) {
        throw FtpServerErrorReply("Requested path not found.");
    }

    // Open data channel and send down file list

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
 * Make a directory on server. An error is returned if the user does not have write access
 * or the mkdir command fails for some reason.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::MKD(CogWheelControlChannel *connection, const QString &arguments)
{

    // User does not have write access to server

    if (!connection->writeAccess()) {
        throw FtpServerErrorReply("User needs write access to perform command.");
    }

    QString path { mapPathToLocal(connection, arguments) };
    QDir newDirectory { path };

    if(!newDirectory.mkdir(path)){
        throw FtpServerErrorReply(530);
    }else{
        connection->sendReplyCode(257);
    }

}

/**
 * @brief CogWheelFTPCore::RMD
 *
 * Reomove a directory from server. An error is returned if the user does not have write access
 * or the rmdir command fails for some reason.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::RMD(CogWheelControlChannel *connection, const QString &arguments)
{

    // User does not have write access to server

    if (!connection->writeAccess()) {
        throw FtpServerErrorReply("User needs write access to perform command.");
    }

    QString path { mapPathToLocal(connection, arguments) };
    QDir directoryToDelete { path  };

    if(directoryToDelete.exists()){

        if(directoryToDelete.rmdir(path)){
            connection->sendReplyCode(250);
        }else{
            throw FtpServerErrorReply("Could not delete directory.");
        }

    } else{
        throw FtpServerErrorReply("Directory not found.");
    }

}

/**
 * @brief CogWheelFTPCore::QUIT
 *
 * Close down control channel connection to server.
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
 * Delete a specified fiel from the server. An error is returned to the client
 * if the user does not have write access, the files does not exist or if remove()
 * fails to delete the file.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::DELE(CogWheelControlChannel *connection, const QString &arguments)
{

    // User does not have write access to server

    if (!connection->writeAccess()) {
        throw FtpServerErrorReply("User needs write access to perform command.");
    }

    QFile fileToDelete { mapPathToLocal(connection, arguments) };

    if(fileToDelete.exists()){

        if(fileToDelete.remove()) {
            connection->sendReplyCode(250);
        } else {
            throw FtpServerErrorReply("Could not delete file.");
        }
    } else {
        throw FtpServerErrorReply("File not found.");
    }

}

/**
 * @brief CogWheelFTPCore::ACCT
 *
 * Set account name. Not currently used by server so just kept for reference.
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
 * Same as STOR except that an error is passed back to the client
 * if the file already exists on the server.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::STOU(CogWheelControlChannel *connection, const QString &arguments)
{

    // User does not have write access to server

    if (!connection->writeAccess()) {
        throw FtpServerErrorReply("User needs write access to perform command.");
    }

    QString path { mapPathToLocal(connection, arguments) };
    QFile file { path  } ;

    if(file.exists()) {
        throw FtpServerErrorReply(551, "File already exists.");
    }

    if (connection->connectDataChannel()) {
        connection->uploadFileToDataChannel(path);
    }

}

/**
 * @brief CogWheelFTPCore::SMNT
 *
 * Change a users root directory. This command has serious security implications
 * and can be switched on/off through a server setting (default off).
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
            throw FtpServerErrorReply("Could not change root directory.");
        }

    }else{
        throw FtpServerErrorReply("SMNT is not allowed.");
    }
}

/**
 * @brief CogWheelFTPCore::STRU
 *
 * Set the server file structure. This is "file" which is the default and any sent
 * parameter is only filed away for reference.
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
 * Resevrse file space on server. Not impelemented.
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
 * Rename file from. Name of source passed and it must immnediately be followed by a RNTO.
 * An error is returned to the client if the user does not have write access or the source
 * file does not exist.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::RNFR(CogWheelControlChannel *connection, const QString &arguments)
{

    // User does not have write access to server

    if (!connection->writeAccess()) {
        throw FtpServerErrorReply("User needs write access to perform command.");
    }

    QString path { mapPathToLocal(connection, arguments) };

    connection->setRenameFromFileName("");

    // Set source file name

    QFileInfo fileToRename { path };
    if(!fileToRename.exists()){
        throw FtpServerErrorReply("Could not find '" + arguments + "'");
    } else{
        connection->setRenameFromFileName(path);
        connection->sendReplyCode(350);
    }

}

/**
 * @brief CogWheelFTPCore::RNTO
 *
 * Rename a file to passed in destination. This command must come straight after a sucessful
 * RNFR command. ANy error is returned to the client if the user does not have write access
 * to the filesystem, no source has been setup, or there was a problem renaming the file.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::RNTO(CogWheelControlChannel *connection, const QString &arguments)
{

    // User does not have write access to server

    if (!connection->writeAccess()) {
        throw FtpServerErrorReply("User needs write access to perform command.");
    }

    // No source set

    if(connection->renameFromFileName() == "") {
        throw FtpServerErrorReply(503);
    }

    // Rename file

    QFile sourceName(connection->renameFromFileName());

    if(sourceName.rename(mapPathToLocal(connection, arguments) )){
        connection->sendReplyCode(250);
    }else{
        connection->sendReplyCode(553);
    }

    // Reset stored source

    connection->setRenameFromFileName("");

}

/**
 * @brief CogWheelFTPCore::REST
 *
 * Set restore point for an interrupted RETR/STOR command.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::REST(CogWheelControlChannel *connection, const QString &arguments)
{

    bool validInteger;

    connection->setRestoreFilePostion(arguments.toInt(&validInteger));

    if(validInteger){
        connection->sendReplyCode(350,"Restarting at "+arguments+". Send STORE or RETRIEVE.");
    } else{
        connection->setRestoreFilePostion(0);
        throw FtpServerErrorReply(500);
    }

}

/**
 * @brief CogWheelFTPCore::ABOR
 *
 * Abort any transfer taking place on the data chanel.
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
 * Reinitialise control channel state. The channel is left in a connected but unauthorised
 * state.
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
 * Transfer file to server. If the file alreayd exists then the data is appended on to
 * it end. Returns an error to client if the user does not have write access.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::APPE(CogWheelControlChannel *connection, const QString &arguments)
{

    // User does not have write access to server

    if (!connection->writeAccess()) {
        throw FtpServerErrorReply("User needs write access to perform command.");
    }

    if (connection->connectDataChannel()) {
        connection->uploadFileToDataChannel(mapPathToLocal(connection,arguments) );
    }

}

/**
 * @brief CogWheelFTPCore::STAT
 *
 * Send connect status reply back to client.
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

// ========
// RFC 2389
// ========

/**
 * @brief CogWheelFTPCore::FEAT
 *
 * Return a list of extended features (commands) supported by the client.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::FEAT(CogWheelControlChannel *connection, const QString &arguments)
{

    Q_UNUSED(arguments);

    QString featReply;

    featReply.append("211-Extensions supported: \r\n");

    for( auto key :  m_ftpCommandTableExtended.keys() ) {
        featReply.append(" "+key+"\r\n");
    }

    featReply.append(" AUTH TLS\r\n");

    connection->sendOnControlChannel(featReply);

    connection->sendReplyCode(211, "End.");

}

// =======
// RFC3659
// =======

/**
 * @brief CogWheelFTPCore::MDTM
 *
 * Return last modified date/time of a file. An error is returned to the client
 * if the file does not exist.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::MDTM(CogWheelControlChannel *connection, const QString &arguments)
{

    QString file { mapPathToLocal(connection, arguments) } ;
    QFileInfo fileInfo { file };

    if (!fileInfo.exists()) {
        throw FtpServerErrorReply("Requested file not found.");
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
 * Return the size in bytes of a file. An error is returned to the client
 * if the file does not exist.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::SIZE(CogWheelControlChannel *connection, const QString &arguments)
{
    QString file { mapPathToLocal(connection, arguments) } ;
    QFileInfo fileInfo { file };

    if (!fileInfo.exists()) {
        throw FtpServerErrorReply("Requested file not found.");
    }

    qDebug() << "File [" << file << "] Size [" << QString::number(fileInfo.size()) << "]";

    connection->sendReplyCode(213, QString::number(fileInfo.size()));

}

/**
 * @brief CogWheelFTPCore::AUTH
 *
 * Switch on TLS support.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
  */
void CogWheelFTPCore::AUTH(CogWheelControlChannel *connection, const QString &arguments)
{

    if (arguments=="TLS") {
        connection->sendReplyCode(234);
        connection->enbleTLSSupport();
        return;
    }

    connection->sendReplyCode(502);

}

/**
 * @brief CogWheelFTPCore::PROT
 *
 * TLS supports only private and clear protection on data channel.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
 */
void CogWheelFTPCore::PROT(CogWheelControlChannel *connection, const QString &arguments)
{

    if ((arguments=="P")|| (arguments=="C")) {
        connection->sendReplyCode(200);
        connection->setDataChanelProtection(arguments[0]);
        return;
    }

    connection->sendReplyCode(502);
}

/**
 * @brief CogWheelFTPCore::PBSZ
 *
 * TLS needs no buffer protection so size 0.
 *
 * @param connection   Pointer to control channel instance.
 * @param arguments    Command arguments.
*/
void CogWheelFTPCore::PBSZ(CogWheelControlChannel *connection, const QString &arguments)
{

    if (arguments=="0") {
        connection->sendReplyCode(200);
        return;
    }

    connection->sendReplyCode(502);
}




