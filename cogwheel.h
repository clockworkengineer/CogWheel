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

#include <QtCore>

constexpr const char   *kCWOrganizationName { "ClockWorkEngineer" };
constexpr const char   *kCWApplicationName { "CogWheel" };

constexpr const char *kCWCommandSTATUS       { "STATUS" };
constexpr const char *kCWCommandCONNECTIONS  { "CONNECTIONS" };
constexpr const char *kCWCommandSTART        { "START" };
constexpr const char *kCWCommandSTOP         { "STOP" };
constexpr const char *kCWCommandKILL         { "KILL" };

constexpr const char *kCWStatusSTOPPED       { "STOPPED" };
constexpr const char *kCWStatusRUNNING       { "RUNNING" };

constexpr const quint64 kCWWriteBytesSize=1024*32;
constexpr const quint64 kCWConnListUpdateTime=5000;
constexpr const quint64 kCWDefaultPort=2221;

#endif // COGWHEEL_H
