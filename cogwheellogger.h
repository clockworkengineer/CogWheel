/*
 * File:   cogwheellogger.h
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

#ifndef COGWHEELLOGGER_H
#define COGWHEELLOGGER_H

#include <QString>
#include <QDebug>

class CogWheelLogger
{

public:

    // Logging levels

    enum Level {
        None = 0,
        Info = 1,
        Warning = 2,
        Error = 4,
        Channel = 8,
        Command = 16,
        All = ~0
    };

    // Get instance

    static CogWheelLogger& getInstance()
    {
        static CogWheelLogger    instance;
        return instance;
    }

    // Disable anything not needed

    CogWheelLogger(const CogWheelLogger & orig) = delete;
    CogWheelLogger(const CogWheelLogger && orig) = delete;
    CogWheelLogger& operator=(CogWheelLogger other) = delete;

    // Friend functions

    friend void cogWheelInfo(const QString &message);
    friend void cogWheelInfo (qintptr handle, const QString &message);
    friend void cogWheelError(const QString &message);
    friend void cogWheelError(qintptr handle, const QString &message);
    friend void cogWheelWarning(const QString &message);
    friend void cogWheelWarning(qintptr handle, const QString &message);
    friend void setLoggingLevel(const quint64 &logLevel);
    friend quint64 getLogLevel();

private:

    // Constuctor

    CogWheelLogger() {};

    // Base line logging (USES QDebug variants at present).

    void info(const QString &message) { qInfo() << message.toStdString().c_str(); }
    void error(const QString &message) { qDebug() << message.toStdString().c_str(); }
    void warning(const QString &message) { qWarning() << message.toStdString().c_str(); }

    quint64 m_logLevel;


};

// Set logging level

inline void setLoggingLevel(const quint64 &logLevel) {
    CogWheelLogger::getInstance().m_logLevel = logLevel;
}

inline quint64 getLogLevel()
{
    return CogWheelLogger::getInstance().m_logLevel;
}

// Base string logging

inline void cogWheelInfo(const QString &message) { if (getLogLevel() & CogWheelLogger::Info) CogWheelLogger::getInstance().info(message); }
inline void cogWheelError(const QString &message) { if (getLogLevel() & CogWheelLogger::Error) CogWheelLogger::getInstance().error(message); }
inline void cogWheelWarning(const QString &message) { if (getLogLevel() & CogWheelLogger::Warning) CogWheelLogger::getInstance().warning(message); }

// Command channel logging (socket handle for command channel is passed in)

inline void cogWheelInfo (qintptr handle, const QString &message)
{
    if (getLogLevel() & CogWheelLogger::Channel) {
        CogWheelLogger::getInstance().info(QString("CONT[%1]I: %2").arg(QString::number(handle), message).toStdString().c_str());
    }
}

inline void cogWheelError (qintptr handle, const QString &message)
{
    if (getLogLevel() & CogWheelLogger::Channel) {
        CogWheelLogger::getInstance().error(QString("CONT[%1]E: %2").arg(QString::number(handle), message).toStdString().c_str());
    }
}

inline void cogWheelWarning (qintptr handle, const QString &message)
{
    if (getLogLevel() & CogWheelLogger::Channel) {
        CogWheelLogger::getInstance().warning(QString("CONT[%1]W: %2").arg(QString::number(handle), message).toStdString().c_str());
    }
}

#endif // COGWHEELLOGGER_H



