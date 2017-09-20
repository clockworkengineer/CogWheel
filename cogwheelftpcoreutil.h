#ifndef COGWHEELFTPCOREUTIL_H
#define COGWHEELFTPCOREUTIL_H
/*
 * File:   cogwheelftpcoreutil.hpp
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
// Description: CogWheelFTPCore utility functions. This includes functions
// for mapping to/from the local file system (root relative) and for the creation
// of list information/fact lines for files returned in reponse to the various FTP
// list commands.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheel.h"
#include "cogwheelcontrolchannel.h"

#include <QDir>
#include <QDateTime>

namespace CogWheelFTPCoreUtil {

// Map a FTP root relative path to local filesystem.

QString mapPathToLocal(CogWheelControlChannel *connection, const QString &path);

// Map a given local filesystem path to a FTP root path.

QString mapPathFromLocal(CogWheelControlChannel *connection, const QString &path);

// Build list line for passed in QFileInfo. The format of which
// is the same as given for the Linux 'ls -l' command.

QString buildLISTLine(const QFileInfo &fileInfo);

// Build fact list for passed in path (directory).

QString buildPathFactList(const QFileInfo &pathInfo, const QString &path);

// Build fact list for file information passed in.

QString buildFileFactList(const QFileInfo &fileInfo);

} // namespace CogWheelFTPCoreUtil
#endif // COGWHEELFTPCOREUTIL_H
