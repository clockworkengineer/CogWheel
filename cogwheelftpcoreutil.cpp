/*
 * File:   cogwheelftpcoreutil.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

//
// Namespace: CogWheelFTPCoreUtil
//
// Description: CogWheelFTPCore utility functions.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelftpcoreutil.h"
#include "cogwheellogger.h"

namespace CogWheelFTPCoreUtil {

/**
 * @brief mapPathToLocal
 *
 * Map a FTP root relative path to local filesystem.
 *
 * @param connection   Pointer to control channel instance.
 * @param path         Path to map to local filesystem.
 *
 * @return Local file system path string,
 */
QString mapPathToLocal(CogWheelControlChannel *connection, const QString &path)
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

    cogWheelInfo(connection->socketHandle(),"Mapping local "+path+" to "+mappedPath);

    return(mappedPath);
}

/**
 * @brief mapPathFromLocal
 *
 * Map a given local filesystem path to a FTP root path.
 *
 * @param connection   Pointer to control channel instance.
 * @param path         Path to map from local filesystem.
 *
 * @return  FTP root path.
 */
QString mapPathFromLocal(CogWheelControlChannel *connection, const QString &path)
{

    QString mappedPath { QFileInfo(path).absoluteFilePath()};

    cogWheelInfo(connection->socketHandle(),"mapped path : "+mappedPath);

    // Strip off root path

    if (mappedPath.startsWith(connection->rootDirectory())) {
        mappedPath = mappedPath.remove(0,connection->rootDirectory().length());

        // If trying to go above root then reset to root

    } else if (mappedPath.length() < connection->rootDirectory().length()){
        mappedPath = "";
    }

    cogWheelInfo(connection->socketHandle(),"Mapping local from "+path+" to "+mappedPath);

    return(mappedPath);
}

/**
 * @brief buildFilePermissions
 *
 * Build files permissions (for LIST) into QString and return.
 *
 * @param fileInfo  File to produce permissions for.
 *
 * @return Files permissions as a QString.
 */
QString buildFilePermissions(const QFileInfo &fileInfo)
{

    char permissions[10];

    permissions[0] = (fileInfo.permissions() & QFile::ReadUser) ? 'r' : '-';
    permissions[1] = (fileInfo.permissions() & QFile::WriteUser) ? 'w' : '-';
    permissions[2] = (fileInfo.permissions() & QFile::ExeUser) ? 'x' : '-';
    permissions[3] = (fileInfo.permissions() & QFile::ReadGroup) ? 'r' : '-';
    permissions[4] = (fileInfo.permissions() & QFile::WriteGroup) ? 'w' : '-';
    permissions[5] = (fileInfo.permissions() & QFile::ExeGroup) ? 'x' : '-';
    permissions[6] = (fileInfo.permissions() & QFile::ReadOther) ? 'r' : '-';
    permissions[7] = (fileInfo.permissions() & QFile::WriteOther) ? 'w' : '-';
    permissions[8] = (fileInfo.permissions() & QFile::ExeOther) ? 'x' : '-';
    permissions[9] = 0;

    return(permissions);

}

/**
 * @brief buildUnixFilePermissions
 *
 * Build files unix permissions into octal QString and return.
 *
 * @param fileInfo  File to produce permissions for.
 *
 * @return Files permissions as a octal QString.
 */
QString buildUnixFilePermissions(const QFileInfo &fileInfo)
{

    unsigned short permissions=0;

    permissions |= (fileInfo.permissions() & QFile::ReadUser) ? 0400 : 0;
    permissions |= (fileInfo.permissions() & QFile::WriteUser) ? 0200 : 0;
    permissions |= (fileInfo.permissions() & QFile::ExeUser) ? 0100 : 0;
    permissions |= (fileInfo.permissions() & QFile::ReadGroup) ? 0040 : 0;
    permissions |= (fileInfo.permissions() & QFile::WriteGroup) ? 0020 : 0;
    permissions |= (fileInfo.permissions() & QFile::ExeGroup) ? 0010 : 0;
    permissions |= (fileInfo.permissions() & QFile::ReadOther) ? 0004 : 0;
    permissions |= (fileInfo.permissions() & QFile::WriteOther) ? 0002 : 0;
    permissions |= (fileInfo.permissions() & QFile::ExeOther) ? 0001 : 0;

    return("0"+QString::number(permissions,8));

}

/**
 * @brief buildMLSDCommonLine
 *
 * Build common file facts to QString and return.
 *
 * @param fileInfo  File to produce facts for.
 *
 * @return Common file facts as a QString.
 */
QString buildMLSDCommonLine(const QFileInfo &fileInfo)
{
    QString line;

    line.append(static_cast<QString>("Modify=")+fileInfo.lastModified().toString("yyyyMMddhhmmss;"));
    line.append(static_cast<QString>("Create=")+fileInfo.created().toString("yyyyMMddhhmmss;"));
    line.append(static_cast<QString>("UNIX.mode=")+buildUnixFilePermissions(fileInfo)+";");
    line.append(static_cast<QString>("UNIX.owner=")+QString::number(fileInfo.ownerId())+";");
    line.append(static_cast<QString>("UNIX.group=")+QString::number(fileInfo.groupId())+";");

    return line;

}

/**
 * @brief buildLISTLine
 *
 * Build list line for passed in QFileInfo. The format of which
 * is the same as given for the Linux 'ls -l' command.
 *
 * @param fileInfo  File to produce list line for.
 *
 * @return List line QString.
 */
QString buildLISTLine(const QFileInfo &fileInfo)
{

    QChar   fileType= '-';
    QString line;
    QString ownerGroup;

    if (fileInfo.isSymLink()) {
        fileType = 'l';
    } else if (fileInfo.isDir()){
        fileType = 'd';
    }

    line.append(fileType+buildFilePermissions(fileInfo)+" 1 ");

    ownerGroup = fileInfo.owner();
    if(ownerGroup.isEmpty()) {
        ownerGroup = "0";
    }
    line.append(ownerGroup.leftJustified(10,' ',true)+" ");

    ownerGroup = fileInfo.group();
    if(ownerGroup.isEmpty()) {
        ownerGroup = "0";
    }
    line.append(ownerGroup.leftJustified(10,' ',true)+" ");

    line.append(QString::number(fileInfo.size()).rightJustified(10,' ',true)+" ");
    line.append(fileInfo.lastModified().toString("MMM dd hh:mm").rightJustified(12,' ',true)+" ");
    line.append(fileInfo.fileName()+"\r\n");

    return(line);


}

/**
 * @brief buildMLSDPathLine
 *
 * Build list line for requested path in MLSD.
 *
 * @param pathInfo  Directory info to produce MLSD line for.
 * @param path      Directory path.
 *
 * @return MLSD list line QString.
 */
QString buildMLSDPathLine(const QFileInfo &pathInfo, const QString &path)
{
    return (static_cast<QString>("Type=cdir;")+buildMLSDCommonLine(pathInfo)+" "+path+"\r\n");
}

/**
 * @brief buildMLSDLine
 *
 * Build list line for single file in MLSD list.
 *
 * @param fileInfo  File information
 *
 * @return List line for file QString.
 */
QString buildMLSDLine(const QFileInfo &fileInfo)
{

    QString line;

    if(fileInfo.isDir()){
        line.append("Type=dir;");
    }else {
        line.append((static_cast<QString>("Type=file;")+"Size=")+QString::number(fileInfo.size())+";");
    }

    line.append(buildMLSDCommonLine(fileInfo)+" "+fileInfo.fileName()+"\r\n");

    return line;

}

} // namespace CogWheelFTPCoreUtil
