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

    enum LoggingLevel {
        None = 0,
        Info = 1,
        Warning = 2,
        Error = 4
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
    friend void cogWheelError(const QString &message);
    friend void cogWheelWarning(const QString &message);
    friend void setLoggingLevel(const quint64 &logLevel);

private:

    // Constuctor

    CogWheelLogger() {};

    // Base line logging (USES QDebug variants at present).

    void info(const QString &message) { if (m_logLevel & Info) qInfo() << message.toStdString().c_str(); }
    void error(const QString &message) { if (m_logLevel & Error)qDebug() << message.toStdString().c_str(); }
    void warning(const QString &message) { if (m_logLevel & Warning) qWarning() << message.toStdString().c_str(); }

    quint64 m_logLevel;


};

// Base string logging

inline void cogWheelInfo(const QString &message) { CogWheelLogger::getInstance().info(message); }
inline void cogWheelError(const QString &message) { CogWheelLogger::getInstance().error(message); }
inline void cogWheelWarning(const QString &message) { CogWheelLogger::getInstance().warning(message); }

// Command channel logging (socket handle for command channel is passed in)

inline void cogWheelInfo (qintptr handle, QString message)
{
    cogWheelInfo(QString("CONT[%1]I: %2").arg(QString::number(handle), message).toStdString().c_str());
}

inline void cogWheelError (qintptr handle, QString message)
{
    cogWheelError(QString("CONT[%1]E: %2").arg(QString::number(handle), message).toStdString().c_str());
}

inline void cogWheelWarning (qintptr handle, QString message)
{
    cogWheelWarning(QString("CONT[%1]W: %2").arg(QString::number(handle), message).toStdString().c_str());
}

// Set logging level

inline void setLoggingLevel(const quint64 &logLevel) {
    CogWheelLogger::getInstance().m_logLevel = logLevel;
}
#endif // COGWHEELLOGGER_H



