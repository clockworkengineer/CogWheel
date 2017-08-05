#include "cogwheelftpcore.h"

#include <QDir>
#include <QDateTime>

QMap<QString, CogWheelFTPCore::FTPCommandFunction> CogWheelFTPCore::m_ftpCommandTable = {
    {"USER", CogWheelFTPCore::commandUSER },
    {"LIST", CogWheelFTPCore::commandLIST },
    {"FEAT", CogWheelFTPCore::commandFEAT },
    {"SYST", CogWheelFTPCore::commandSYST },
    {"PWD", CogWheelFTPCore::commandPWD },
    {"TYPE", CogWheelFTPCore::commandTYPE },
    {"PORT", CogWheelFTPCore::commandPORT },
    {"CWD",CogWheelFTPCore::commandCWD},
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
    {550,"The filename, directory name, or volume label syntax is incorrect."}
};

CogWheelFTPCore::CogWheelFTPCore(QObject *parent) : QObject(parent)
{

}

void CogWheelFTPCore::performCommand(CogWheelConnection *connection, QStringList commandAndArgments)
{

    if (m_ftpCommandTable.contains(commandAndArgments[0])) {
        FTPCommandFunction command=m_ftpCommandTable[commandAndArgments[0]];
        command(connection, commandAndArgments);
    } else {
        qWarning() << "CogWheelFTPCore::performCommand: Unsupported command [" << commandAndArgments[0] << "]";
        connection->sendReply(202);
    }
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

    line.append(temp.leftJustified(10,' ',true));
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

void CogWheelFTPCore::commandUSER(CogWheelConnection *connection, QStringList commandAndArgments)
{

    connection->sendReply(200);

}

void CogWheelFTPCore::commandLIST(CogWheelConnection *connection, QStringList commandAndArgments)
{
    QDir    currentWorkingDirectory(connection->m_currentWorkingDirectory);
    QString listing;

    if (commandAndArgments.size()>1) {
        qWarning() << "LIST argument passed : " << commandAndArgments[1];
    }

    qDebug() << "CogWheelFTPCore::commandLIST";

    for (QFileInfo &item : currentWorkingDirectory.entryInfoList())
    {
        listing.append(buildLISTLine(item));
    }

    connection->sendReply(150);
    connection->sendData(listing);
    connection->m_dataChannel->disconnectFromClient();
    connection->sendReply(226);

}

void CogWheelFTPCore::commandFEAT(CogWheelConnection *connection, QStringList commandAndArgments)
{
    connection->sendReply (500);

}

void CogWheelFTPCore::commandSYST(CogWheelConnection *connection, QStringList commandAndArgments)
{
    connection->sendReply(205, "Linux");

}

void CogWheelFTPCore::commandPWD(CogWheelConnection *connection, QStringList commandAndArgments)
{
    qDebug() << "CogWheelFTPCore::commandPWD : Working Directotry [" << connection->m_currentWorkingDirectory << "]";

    connection->sendReply (257, connection->m_currentWorkingDirectory);
}

void CogWheelFTPCore::commandTYPE(CogWheelConnection *connection, QStringList commandAndArgments)
{
    connection->sendReply(200);
}

void CogWheelFTPCore::commandPORT(CogWheelConnection *connection, QStringList commandAndArgments)
{
    qDebug() << "PORT " << commandAndArgments[1];
    if (connection->m_dataChannel == nullptr) {
        return;
    }
    connection->m_dataChannel  = new CogWheelDataChannel();
    QStringList ipList = commandAndArgments[1].split(',');
    connection->m_dataChannel->setClientHostIP(ipList[0]+"."+ipList[1]+"."+ipList[2]+"."+ipList[3]);
    QString first = ipList[4];
    QString second = ipList[5];
    connection->m_dataChannel->setClientHostPort((first.toInt()<<8)|second.toInt());
    connection->m_dataChannel->startChannel();
    connection->sendReply(200);
}

void CogWheelFTPCore::commandCWD(CogWheelConnection *connection, QStringList commandAndArgments)
{

    //make sure the path exists
    QDir path(commandAndArgments[1]);
    if(!path.exists())
    {
        connection->sendReply(550, "Requested path not found.");
    }
    else
    {
        connection->m_currentWorkingDirectory = commandAndArgments[1];
        connection->sendReply(250);
    }
}
