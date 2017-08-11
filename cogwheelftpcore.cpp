#include "cogwheelftpcore.h"
#include "cogwheelusersettings.h"

#include <QDir>
#include <QDateTime>

QMap<QString, CogWheelFTPCore::FTPCommandFunction> CogWheelFTPCore::m_unauthorizedCommandTable = {
    {"USER", CogWheelFTPCore::USER },
    {"PASS", CogWheelFTPCore::PASS },
    {"TYPE", CogWheelFTPCore::TYPE },
};

QMap<QString, CogWheelFTPCore::FTPCommandFunction> CogWheelFTPCore::m_ftpCommandTable = {
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
};

QMap<quint16, QString> CogWheelFTPCore::m_ftpServerResponse = {
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

QString CogWheelFTPCore::buildLISTLine(QFileInfo &file)
{
    QString line;
    QString temp;

    line.append(file.isDir() ? 'd' : '-');
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

    if(temp == "")
    {
        temp = "0";
    }

    line.append(file.owner().leftJustified(10,' ',true));
    line.append(" ");

    //padded by 10 and left justified
    temp = file.group();

    if(temp == "")
    {
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

QString CogWheelFTPCore::buildCWDPath(CogWheelConnection *connection, const QString &path)
{

    if (path.startsWith('/')) {
        return(path);
    } else {
        return(connection->currentWorkingDirectory()+path);
    }

}

QString CogWheelFTPCore::buildFilePath(CogWheelConnection *connection, const QString &file)
{

    if (file.startsWith('/')) {
        return(file);
    } else {
        return(connection->currentWorkingDirectory()+file);
    }

}

void CogWheelFTPCore::performCommand(CogWheelConnection *connection, QStringList commandAndArgments)
{

    try {
        QString upperCaseCommand=commandAndArgments[0];
        upperCaseCommand=upperCaseCommand.toUpper();
        if (m_ftpCommandTable.contains(upperCaseCommand)) {
            if (connection->authorized()) {
                FTPCommandFunction command=m_ftpCommandTable[upperCaseCommand];
                command(connection, commandAndArgments);
            } else if (m_unauthorizedCommandTable.contains(upperCaseCommand)) {
                FTPCommandFunction command=m_unauthorizedCommandTable[upperCaseCommand];
                command(connection, commandAndArgments);
            } else {
                connection->sendReplyCode(530, "Please login with USER and PASS");
            }
        } else {
            qWarning() << "Unsupported FTP command [" << commandAndArgments[0] << "]";
            connection->sendReplyCode(500);
        }

    } catch(QString err) {
        connection->sendReplyCode(550,err);
    } catch(...) {
        connection->sendReplyCode(550, "Unknown error handling " + commandAndArgments[0] + " command.");
    }

}
void CogWheelFTPCore::USER(CogWheelConnection *connection, QStringList commandAndArgments)
{

    // Anonymous login

    if (commandAndArgments[1]=="anonymous") {

        connection->setAnonymous(true);

    } else if (!CogWheelUserSettings::checkUserName(commandAndArgments[1])) {

        // User does not exist

        connection->sendReplyCode(530, "User name not valid.");
        return;

    }

    // Set user name

    connection->setUserName(commandAndArgments[1]);

    // Set intial workign directory

    if (!connection->anonymous()) {
        connection->setCurrentWorkingDirectory(CogWheelUserSettings::getHomePath(connection->userName()));
    } else {
        connection->setCurrentWorkingDirectory("");
    }

    // Ask for password

    connection->sendReplyCode(331);

}

void CogWheelFTPCore::LIST(CogWheelConnection *connection, QStringList commandAndArgments)
{

    QString path;
    QString listing;

    if (commandAndArgments.size()==1) {
        path = connection->currentWorkingDirectory();
    } else {
        path = commandAndArgments[1];
        if (path[0]!='/') {
            path = connection->currentWorkingDirectory()+path;
        }
    }

    QFileInfo fileInfo { QFile { path }};

    if (!fileInfo.exists()) {
        connection->sendReplyCode(550, "Requested path not found.");
        return;
    }

    if (connection->dataChannel()->connectToClient(connection)) {

        if (fileInfo.isDir()) {
            QDir listDirectory { path };
            for (QFileInfo &item : listDirectory.entryInfoList()) {
                listing.append(buildLISTLine(item));
            }
        } else {
            listing.append(buildLISTLine(fileInfo));
        }

        connection->sendOnDataChannel(listing);
        connection->dataChannel()->disconnectFromClient(connection);
        connection->sendReplyCode(226);

    }

}

void CogWheelFTPCore::FEAT(CogWheelConnection *connection, QStringList commandAndArgments)
{

    connection->sendReplyCode (500);

}

void CogWheelFTPCore::SYST(CogWheelConnection *connection, QStringList commandAndArgments)
{

    connection->sendReplyCode(215, "UNIX Type: L8");

}

void CogWheelFTPCore::PWD(CogWheelConnection *connection, QStringList commandAndArgments)
{

    qDebug() << "PWD " << connection->currentWorkingDirectory();

    connection->sendReplyCode (257, "\""+connection->currentWorkingDirectory()+"\"");

}

void CogWheelFTPCore::TYPE(CogWheelConnection *connection, QStringList commandAndArgments)
{

    connection->sendReplyCode(200);
}

void CogWheelFTPCore::PORT(CogWheelConnection *connection, QStringList commandAndArgments)
{

    QStringList ipList = commandAndArgments[1].split(',');
    connection->dataChannel()->setClientHostIP(ipList[0]+"."+ipList[1]+"."+ipList[2]+"."+ipList[3]);

    QString first = ipList[4];
    QString second = ipList[5];

    connection->dataChannel()->setClientHostPort((first.toInt()<<8)|second.toInt());

    connection->sendReplyCode(200);

}

void CogWheelFTPCore::CWD(CogWheelConnection *connection, QStringList commandAndArgments)
{

    QString newCWDPath { buildCWDPath(connection, commandAndArgments[1]) };

    QDir path(newCWDPath);
    if(!path.exists()) {
        connection->sendReplyCode(550, "Requested path not found.");
    } else {
        connection->setCurrentWorkingDirectory(newCWDPath);
        connection->sendReplyCode(250);
    }

}

void CogWheelFTPCore::PASS(CogWheelConnection *connection, QStringList commandAndArgments)
{

    // For non-anonymous check users password

    if (!connection->anonymous()) {
        if (!CogWheelUserSettings::checkPassword(connection->userName(), commandAndArgments[1])) {
            connection->sendReplyCode(530); // Failure
            return;
        }
        connection->sendReplyCode(230); // Success then login
    } else {
        connection->sendReplyCode(230, "Logged in Anonymous as "+commandAndArgments[1]);
    }

    connection->setAuthorized(true);

}

void CogWheelFTPCore::CDUP(CogWheelConnection *connection, QStringList commandAndArgments)
{

    QDir currentPath  { connection->currentWorkingDirectory() };

    if(!currentPath.cdUp()) {
        connection->sendReplyCode(550, "Requested path not found.");
    } else {
        connection->setCurrentWorkingDirectory(currentPath.absolutePath());
        connection->sendReplyCode(250);
    }

}

void CogWheelFTPCore::RETR(CogWheelConnection *connection, QStringList commandAndArgments)
{

    qDebug() << "RETR " <<  commandAndArgments[1];

    QFile file { buildFilePath(connection,commandAndArgments[1]) } ;

    if(!file.exists()){
        connection->sendReplyCode(450);
        return;
    }

    if (connection->dataChannel()->connectToClient(connection)) {
        connection->dataChannel()->downloadFile(connection, buildFilePath(connection,commandAndArgments[1]) );
        connection->sendReplyCode(226);
    }

}

void CogWheelFTPCore::NOOP(CogWheelConnection *connection, QStringList commandAndArgments)
{
    connection->sendReplyCode(200);
}

void CogWheelFTPCore::MODE(CogWheelConnection *connection, QStringList commandAndArgments)
{
    connection->sendReplyCode(200);
}

void CogWheelFTPCore::STOR(CogWheelConnection *connection, QStringList commandAndArgments)
{

    QFile file { buildFilePath(connection,commandAndArgments[1]) } ;

    //if the file exists, overwrite it
    if(file.exists())
    {
        if(!file.remove())
        {
            connection->sendReplyCode(551, "File could not be overwritten");
            return;
        }
    }

    if (connection->dataChannel()->connectToClient(connection)) {
        connection->dataChannel()->uploadFile(connection, buildFilePath(connection,commandAndArgments[1]) );
    }

}

void CogWheelFTPCore::PASV(CogWheelConnection *connection, QStringList commandAndArgments)
{

    connection->setPassive(true);
    connection->dataChannel()->listenForConnection();

}

void CogWheelFTPCore::HELP(CogWheelConnection *connection, QStringList commandAndArgments)
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

void CogWheelFTPCore::SITE(CogWheelConnection *connection, QStringList commandAndArgments)
{
    connection->sendReplyCode(202);
}

void CogWheelFTPCore::NLST(CogWheelConnection *connection, QStringList commandAndArgments)
{
    QString path;
    QString listing;

    if (commandAndArgments.size()==1) {
        path = connection->currentWorkingDirectory();
    } else {
        path = commandAndArgments[1];
        if (path[0]!='/') {
            path = connection->currentWorkingDirectory()+path;
        }
    }

    QFileInfo fileInfo { QFile { path }};

    if (!fileInfo.exists() || !fileInfo.isDir()) {
        connection->sendReplyCode(550, "Requested path not found.");
    }

    if (connection->dataChannel()->connectToClient(connection)) {

        QDir listDirectory { path };

        for (QString item : listDirectory.entryList()) {
            listing.append(item+"\r\n");
        }

        connection->sendOnDataChannel(listing);
        connection->dataChannel()->disconnectFromClient(connection);
        connection->sendReplyCode(226);

    }

}

void CogWheelFTPCore::MKD(CogWheelConnection *connection, QStringList commandAndArgments)
{

    QString path;

    if (commandAndArgments.size()==1) {
        path = connection->currentWorkingDirectory();
    } else {
        path = commandAndArgments[1];
        if (path[0]!='/') {
            path = connection->currentWorkingDirectory()+path;
        }
    }

    QDir newDirectory(path);

    if(!newDirectory.mkdir(path)){
        connection->sendReplyCode(530);
    }else{
        connection->sendReplyCode(257);
    }

}

void CogWheelFTPCore::RMD(CogWheelConnection *connection, QStringList commandAndArgments)
{
    QString path;

    if (commandAndArgments.size()==1) {
        path = connection->currentWorkingDirectory();
    } else {
        path = commandAndArgments[1];
        if (path[0]!='/') {
            path = connection->currentWorkingDirectory()+path;
        }
    }

    QDir directoryToDelete(path);
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

void CogWheelFTPCore::QUIT(CogWheelConnection *connection, QStringList commandAndArgments)
{

    connection->sendReplyCode(221);
    connection->closeConnection();

}

void CogWheelFTPCore::STAT(CogWheelConnection *connection, QStringList commandAndArgments)
{
    qDebug() << "Need to fill in";
}

void CogWheelFTPCore::DELE(CogWheelConnection *connection, QStringList commandAndArgments)
{
    qDebug() << "Need to fill in";
}

void CogWheelFTPCore::ACCT(CogWheelConnection *connection, QStringList commandAndArgments)
{
    qDebug() << "Need to fill in";
}

void CogWheelFTPCore::SMNT(CogWheelConnection *connection, QStringList commandAndArgments)
{
    qDebug() << "Need to fill in";
}

void CogWheelFTPCore::REIN(CogWheelConnection *connection, QStringList commandAndArgments)
{
    qDebug() << "Need to fill in";
}

void CogWheelFTPCore::STRU(CogWheelConnection *connection, QStringList commandAndArgments)
{
    qDebug() << "Need to fill in";
}

void CogWheelFTPCore::STOU(CogWheelConnection *connection, QStringList commandAndArgments)
{
    qDebug() << "Need to fill in";
}

void CogWheelFTPCore::APPE(CogWheelConnection *connection, QStringList commandAndArgments)
{
    qDebug() << "Need to fill in";
}

void CogWheelFTPCore::ALLO(CogWheelConnection *connection, QStringList commandAndArgments)
{
    qDebug() << "Need to fill in";
}

void CogWheelFTPCore::REST(CogWheelConnection *connection, QStringList commandAndArgments)
{
    qDebug() << "Need to fill in";
}

void CogWheelFTPCore::RNFR(CogWheelConnection *connection, QStringList commandAndArgments)
{
    qDebug() << "Need to fill in";
}

void CogWheelFTPCore::RNTO(CogWheelConnection *connection, QStringList commandAndArgments)
{
    qDebug() << "Need to fill in";
}

void CogWheelFTPCore::ABOR(CogWheelConnection *connection, QStringList commandAndArgments)
{
    qDebug() << "Need to fill in";
}

