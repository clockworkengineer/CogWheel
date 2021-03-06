/*
 * File:   cogwheel.h
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

#ifndef COGWHEEL_H
#define COGWHEEL_H

//
// Description: CogWheel server and manager common definitions and constants.
//

#include <QtGlobal>

// Organization/Application name for config

constexpr const char   *kCWOrganizationName { "ClockWorkEngineer" };
constexpr const char   *kCWApplicationName { "CogWheel" };

// Manager prefix for Manager/Controller local socket

constexpr const char   *kCWManagerPostfix { "Manager" };

// End of Line

constexpr const char   *kCWEOL { "\r\n" };

// Manager/Controller commands

constexpr const char *kCWCommandSTATUS       { "STATUS" };
constexpr const char *kCWCommandCONNECTIONS  { "CONNECTIONS" };
constexpr const char *kCWCommandSTART        { "START" };
constexpr const char *kCWCommandSTOP         { "STOP" };
constexpr const char *kCWCommandKILL         { "KILL" };
constexpr const char *kCWCommandLOGOUTPUT    { "LOGOUTPUT" };

// Status command replies

constexpr const char *kCWStatusSTOPPED       { "STOPPED" };
constexpr const char *kCWStatusRUNNING       { "RUNNING" };
constexpr const char *kCWStatusTERMINATED    { "TERMINATED" };

// Write size for file downloads

constexpr const quint64 kCWWriteBytesSize=1024*32;

// Conenction list update timer milliseconds

constexpr const quint64 kCWConnListUpdateTime=5000;

// Loggingbuffer flush timer

constexpr const quint64 kCWLoggingFlushTimer=1000;

// Default server connection port

constexpr const quint64 kCWDefaultPort=2221;

#endif // COGWHEEL_H
