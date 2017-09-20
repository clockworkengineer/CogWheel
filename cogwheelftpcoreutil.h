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
// Description: CogWheelFTPCore utility functions.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheel.h"
#include "cogwheelcontrolchannel.h"

#include <QDir>
#include <QDateTime>

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
QString mapPathToLocal(CogWheelControlChannel *connection, const QString &path);

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
QString mapPathFromLocal(CogWheelControlChannel *connection, const QString &path);

/**
 * @brief buildFilePermissions
 *
 * Build files permissions (for LIST) into QString and return.
 *
 * @param fileInfo  File to produce permissions for.
 *
 * @return Files permissions as a QString.
 */
QString buildFilePermissions(const QFileInfo &fileInfo);

/**
 * @brief buildUnixFilePermissions
 *
 * Build files unix permissions into octal QString and return.
 *
 * @param fileInfo  File to produce permissions for.
 *
 * @return Files permissions as a octal QString.
 */
QString buildUnixFilePermissions(const QFileInfo &fileInfo);


/**
 * @brief buildMLSDCommonLine
 *
 * Build common file facts to QString and return.
 *
 * @param fileInfo  File to produce facts for.
 *
 * @return Common file facts as a QString.
 */
QString buildMLSDCommonLine(const QFileInfo &fileInfo);

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
QString buildLISTLine(const QFileInfo &fileInfo);

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
QString buildMLSDPathLine(const QFileInfo &pathInfo, const QString &path);


/**
 * @brief buildMLSDLine
 *
 * Build list line for single file in MLSD list.
 *
 * @param fileInfo  File information
 *
 * @return List line for file QString.
 */
QString buildMLSDLine(const QFileInfo &fileInfo);

} // namespace CogWheelFTPCoreUtil
#endif // COGWHEELFTPCOREUTIL_H
