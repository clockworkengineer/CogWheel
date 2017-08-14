#include "cogwheelftpcore.h"
#include "cogwheelusersettings.h"

#include <QDir>
#include <QDateTime>

QHash<QString, CogWheelFTPCore::FTPCommandFunction> CogWheelFTPCore::m_unauthCommandTable = {
    {"USER", CogWheelFTPCore::USER },
    {"PASS", CogWheelFTPCore::PASS },
    {"TYPE", CogWheelFTPCore::TYPE },
};

QHash<QString, CogWheelFTPCore::FTPCommandFunction> CogWheelFTPCore::m_ftpCommandTable = {
    {"USER", CogWheelFTPCore::USER },
    {"PASS", CogWheelFTPCore::PASS },
    {"LIST", CogWheelFTPCore::LIST },
    {"FEAT", CogWheelFTPCore::FEAT },
    {"SYST", CogWheelFTPCore::SYST },
    {"PWD", CogWheelFTPCore::PWD },
    {"TYPE", CogWheelFTPCore::TYPE },
    {"PORT", CogWheelFTPCore::PORT },
    {"CWD", CogWheelFTPCore::CWD},
    {"CDUP", CogWheelFTPCore::CDUP},
    {"RETR", CogWheelFTPCore::RETR},
    {"MODE", CogWheelFTPCore::MODE},
    {"NOOP", CogWheelFTPCore::NOOP},
    {"STOR", CogWheelFTPCore::STOR},
    {"PASV", CogWheelFTPCore::PASV},
    {"HELP", CogWheelFTPCore::HELP},
    {"SITE", CogWheelFTPCore::SITE},
    {"NLST", CogWheelFTPCore::NLST},
    {"MKD", CogWheelFTPCore::MKD},
    {"RMD", CogWheelFTPCore::RMD},
    {"QUIT", CogWheelFTPCore::QUIT},
    {"DELE", CogWheelFTPCore::DELE},
    {"ACCT", CogWheelFTPCore::ACCT},
    {"STOU", CogWheelFTPCore::STOU},
    {"STRU", CogWheelFTPCore::STRU},
    {"SMNT", CogWheelFTPCore::SMNT},
    {"ALLO", CogWheelFTPCore::ALLO},
    {"RNFR", CogWheelFTPCore::RNFR},
    {"RNTO", CogWheelFTPCore::RNTO},
    {"REST", CogWheelFTPCore::REST},
    {"ABOR", CogWheelFTPCore::ABOR},
    {"REIN", CogWheelFTPCore::REIN},
    {"APPE", CogWheelFTPCore::APPE},
};

QHash<quint16, QString> CogWheelFTPCore::m_ftpServerResponse = {
    {110,"Restart marker reply."},
    {120,"Service ready in nnn minutes."},
    {125,"Data connection already open; transfer starting."},
    {150,"File status okay; about to open data connection."},
    {200,"Command okay."},
    {202,"Command not implemented, superfluous at this site."},
    {211,"System status, nothing to report."},
    {212,"Directory status."},
    {213,"End of status."},
    {214,"Help command successful."},
    {215,"NAME system type."},
    {220,"Service ready for new user."},
    {221,"Service closing control connection."},
    {225,"Data connection open; no transfer in progress."},
    {226,"Closing data connection."},
    {227,"Entering Passive Mode (h1,h2,h3,h4,p1,p2)."},
    {230,"User logged in, proceed."},
    {250,"Requested file action okay, completed."},
    {257,"Path was created."},
    {331,"Password required."},
    {332,"Need account for login."},
    {350,"Requested file action pending further information."},
    {421,"Service not available, closing control connection."},
    {425,"Can't open data connection."},
    {426,"Connection closed; transfer aborted."},
    {450,"Requested file action not taken."},
    {451,"Requested action aborted: local error in processing."},
    {452,"Requested action not taken."},
    {500,"Syntax error, command unrecognized."},
    {501,"Syntax error in parameters or arguments."},
    {502,"Command not implemented."},
    {503,"Bad sequence of commands."},
    {504,"Command not implemented for that parameter."},
    {530,"Not logged in."},
    {532,"Need account for storing files."},
    {550,"Requested action not taken."},
    {551,"Requested action aborted: page type unknown."},
    {552,"Requested file action aborted."},
    {553,"Requested action not taken."},
};

CogWheelFTPCore::CogWheelFTPCore(QObject *parent) : QObject(parent)
{

}

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

    //padded by 10 and left justified
    temp = file.group();

    if(temp == "") {
        temp = "0";
    }

    line.append(temp.leftJustified(10,' ',true));
    line.append(" ");

    //padded by 10 and right justified
    temp = QString::number(file.size());
    line.append(temp.rightJustified(10,' ',true));
    line.append(" ");

    //padded by 12 and left justified
    temp = file.lastModified().toString("MMM dd hh:mm");

    line.append(temp.rightJustified(12,' ',true));
    line.append(" ");

    line.append(file.fileName());
    line.append("\r\n");

    return(line);

}

QString CogWheelFTPCore::getResponseText(quint16 responseCode)
{
    if (m_ftpServerResponse.contains(responseCode)) {
        return(m_ftpServerResponse[responseCode]);
    } else {
        return("");
    }
}

QString CogWheelFTPCore::mapPathToLocal(CogWheelControlChannel *connection, const QString &path)
{

    QString mappedLocalPath;

    if (path.startsWith('/')) {
        mappedLocalPath = connection->rootDirectory()+path;
    } else {
        mappedLocalPath = connection->rootDirectory()+connection->currentWorkingDirectory();
        if (connection->currentWorkingDirectory()=="/") {
            mappedLocalPath += path;
        } else {
            mappedLocalPath += ("/" + path);
        }
    }

    qDebug() << "Mapping local " << path << " to " << mappedLocalPath;

    return(mappedLocalPath);
}

QString CogWheelFTPCore::mapPathFromLocal(CogWheelControlChannel *connection, const QString &path)
{

    QString mappedPath { QFileInfo(path).absoluteFilePath()};

    qDebug() << "mapped path : " << mappedPath;

    // Strip off root path

    if (mappedPath.startsWith(connection->rootDirectory())) {
        mappedPath = mappedPath.remove(0,connection->rootDirectory().length());

        // If trying to go above root then reset to root

    } else if (mappedPath.length() < connection->rootDirectory().length()){
        qDebug() << "Have gone above root so reset.";
        mappedPath = "";
    }

    qDebug() << "Mapping local from " << path << " to " << mappedPath;

    return(mappedPath);
}


void CogWheelFTPCore::performCommand(CogWheelControlChannel *connection, const QString &command, const QString &arguments)
{

    try {

        qDebug() << "COMMAND : [" << command << "]  ARGUMENT [" << arguments << "]";

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
            qWarning() << "Unsupported FTP command [" << command << "]";
            connection->sendReplyCode(500);
        }


    } catch(QString err) {
        connection->sendReplyCode(550,err);
    } catch(...) {
        connection->sendReplyCode(550, "Unknown error handling " + command + " command.");
    }

}
void CogWheelFTPCore::USER(CogWheelControlChannel *connection, QString arguments)
{

    // Anonymous login

    if (arguments=="anonymous") {

        connection->setAnonymous(true);

    } else if (!CogWheelUserSettings::checkUserName(arguments)) {

        // User does not exist

        connection->sendReplyCode(530, "User name not valid.");
        return;

    }

    // Set user name

    connection->setUserName(arguments);

    // Set intial workign directory

    if (!connection->isAnonymous()) {
        connection->setRootDirectory(CogWheelUserSettings::getRootPath(connection->userName()));
    } else {
        connection->setRootDirectory("/tmp");
    }

    // Start at root

    connection->setCurrentWorkingDirectory("");

    // Ask for password

    connection->sendReplyCode(331);

}

void CogWheelFTPCore::LIST(CogWheelControlChannel *connection, QString arguments)
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
            for (QFileInfo &item : listDirectory.entryInfoList()) {
                listing.append(buildListLine(item));
            }
        } else {
            listing.append(buildListLine(fileInfo));
        }

        connection->sendOnDataChannel(listing);
        connection->disconnectDataChannel();

    }

}

void CogWheelFTPCore::FEAT(CogWheelControlChannel *connection, QString arguments)
{

    connection->sendReplyCode (500);

}

void CogWheelFTPCore::SYST(CogWheelControlChannel *connection, QString arguments)
{

    connection->sendReplyCode(215, "UNIX Type: L8");

}

void CogWheelFTPCore::PWD(CogWheelControlChannel *connection, QString arguments)
{

    qDebug() << "PWD " << connection->currentWorkingDirectory();

    connection->sendReplyCode (257, "\""+connection->currentWorkingDirectory()+"\"");

}

void CogWheelFTPCore::TYPE(CogWheelControlChannel *connection, QString arguments)
{
    connection->setTransferType(arguments[0]);  // Just keep first character for the record
    connection->sendReplyCode(200);
}

void CogWheelFTPCore::PORT(CogWheelControlChannel *connection, QString arguments)
{

    QStringList ipList = arguments.split(',');

    connection->setHostPortForDataChannel(ipList);

    connection->sendReplyCode(200);

}

void CogWheelFTPCore::CWD(CogWheelControlChannel *connection, QString arguments)
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

void CogWheelFTPCore::PASS(CogWheelControlChannel *connection, QString arguments)
{

    // For non-anonymous check users password

    if (!connection->isAnonymous()) {
        if (!CogWheelUserSettings::checkUserPassword(connection->userName(), arguments)) {
            connection->sendReplyCode(530); // Failure
            return;
        }
        connection->sendReplyCode(230); // Success then login
    } else {
        connection->sendReplyCode(230, "Logged in Anonymous as "+arguments);
    }

    connection->setAuthorized(true);

}

void CogWheelFTPCore::CDUP(CogWheelControlChannel *connection, QString arguments)
{

    QDir path  { mapPathToLocal(connection, arguments) };

    if(!path.cdUp()) {
        connection->sendReplyCode(550, "Requested path not found.");
    } else {
        connection->setCurrentWorkingDirectory(mapPathFromLocal(connection, path.absolutePath()));
        connection->sendReplyCode(250);
    }

}

void CogWheelFTPCore::RETR(CogWheelControlChannel *connection, QString arguments)
{

    QFile file { mapPathToLocal(connection, arguments) } ;

    if(!file.exists()){
        connection->sendReplyCode(450);
        return;
    }

    if (connection->connectDataChannel()) {
        connection->downloadFileFromDataChannel(mapPathToLocal(connection, arguments ));
        connection->sendReplyCode(226);
    }

}

void CogWheelFTPCore::NOOP(CogWheelControlChannel *connection, QString arguments)
{
    connection->sendReplyCode(200);
}

void CogWheelFTPCore::MODE(CogWheelControlChannel *connection, QString arguments)
{

    connection->setTransferMode(arguments[0]);

    connection->sendReplyCode(200);
}

void CogWheelFTPCore::STOR(CogWheelControlChannel *connection, QString arguments)
{

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

void CogWheelFTPCore::PASV(CogWheelControlChannel *connection, QString arguments)
{

    connection->setPassive(true);
    connection->listenForConnectionOnDataChannel();

}

void CogWheelFTPCore::HELP(CogWheelControlChannel *connection, QString arguments)
{
    QString helpReply;
    int column;

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

void CogWheelFTPCore::SITE(CogWheelControlChannel *connection, QString arguments)
{
    connection->sendReplyCode(202);
}

void CogWheelFTPCore::NLST(CogWheelControlChannel *connection, QString arguments)
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

        connection->sendOnDataChannel(listing);
        connection->disconnectDataChannel();

    }

}

void CogWheelFTPCore::MKD(CogWheelControlChannel *connection, QString arguments)
{

    QString path { mapPathToLocal(connection, arguments) };
    QDir newDirectory { path };

    if(!newDirectory.mkdir(path)){
        connection->sendReplyCode(530);
    }else{
        connection->sendReplyCode(257);
    }

}

void CogWheelFTPCore::RMD(CogWheelControlChannel *connection, QString arguments)
{

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

void CogWheelFTPCore::QUIT(CogWheelControlChannel *connection, QString arguments)
{

    connection->sendReplyCode(221);
    connection->closeConnection();

}

void CogWheelFTPCore::DELE(CogWheelControlChannel *connection, QString arguments)
{

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

void CogWheelFTPCore::ACCT(CogWheelControlChannel *connection, QString arguments)
{

    connection->setAccountName(arguments);
    connection->sendReplyCode(200);

}

void CogWheelFTPCore::STOU(CogWheelControlChannel *connection, QString arguments)
{
    QString path { mapPathToLocal(connection, arguments) };
    QFile file { path  } ;

    if(file.exists())
    {
        connection->sendReplyCode(551, "File already exists.");
        return;
    }

    if (connection->connectDataChannel()) {
        connection->uploadFileToDataChannel(path);
    }

}

void CogWheelFTPCore::SMNT(CogWheelControlChannel *connection, QString arguments)
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
        connection->sendReplyCode(550, "SMNT not allowed.");
    }
}

void CogWheelFTPCore::STRU(CogWheelControlChannel *connection, QString arguments)
{
    connection->setFileStructure(arguments[0]);
    connection->sendReplyCode(200);
}

void CogWheelFTPCore::ALLO(CogWheelControlChannel *connection, QString arguments)
{
    connection->sendReplyCode(200);
}


void CogWheelFTPCore::RNFR(CogWheelControlChannel *connection, QString arguments)
{
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

void CogWheelFTPCore::RNTO(CogWheelControlChannel *connection, QString arguments)
{

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

void CogWheelFTPCore::REST(CogWheelControlChannel *connection, QString arguments)
{

    if(connection->renameFromFileName() == "") {
        connection->sendReplyCode(550);
        return;
    }

    bool validInteger;

    connection->setRestoreFilePostion(arguments.toInt(&validInteger));

    if(validInteger){
        connection->sendReplyCode(350);
    } else{
        connection->setRestoreFilePostion(0);
        connection->sendReplyCode(500);
    }

}

void CogWheelFTPCore::ABOR(CogWheelControlChannel *connection, QString arguments)
{

    connection->abortOnDataChannel();
    connection->sendReplyCode(226);

}

void CogWheelFTPCore::REIN(CogWheelControlChannel *connection, QString arguments)
{

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

void CogWheelFTPCore::APPE(CogWheelControlChannel *connection, QString arguments)
{

    if (connection->connectDataChannel()) {
        connection->uploadFileToDataChannel(mapPathToLocal(connection,arguments) );
    }

}

void CogWheelFTPCore::STAT(CogWheelControlChannel *connection, QString arguments)
{
    qDebug() << "Need to fill in";
}



