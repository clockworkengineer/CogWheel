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
#include <QMutex>

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
    friend void clearLoggingBuffer();
    friend quint64 getLoggingLevel();

    // Private data accessors

    QStringList getLoggingBuffer() const { return m_loggingBuffer; }
    bool getLoggingEnabled() const { return m_enabled; }
    void setLoggingEnabled(bool enabled)  { m_enabled = enabled; }

private:

    // Constuctor

    CogWheelLogger() {};

    // Append message to buffer

    void appendMessageToLogBuffer(const QString &message) {
        m_loggingBufferMutex.lock();
        m_loggingBuffer.append(message);
        m_loggingBufferMutex.unlock();
    }

    // Base line logging (USES QDebug variants at present).

    void info(const QString &message) { if (m_enabled) appendMessageToLogBuffer(message); }
    void error(const QString &message) { if (m_enabled) appendMessageToLogBuffer(message);}
    void warning(const QString &message) { if (m_enabled) appendMessageToLogBuffer(message);}

    bool m_enabled=true;
    quint64 m_loggingLevel;
    QMutex m_loggingBufferMutex;
    QStringList m_loggingBuffer;


};

// Set logging level

inline void setLoggingLevel(const quint64 &logLevel) {
    CogWheelLogger::getInstance().m_loggingLevel = logLevel;
}

inline quint64 getLoggingLevel()
{
    return CogWheelLogger::getInstance().m_loggingLevel;
}

inline void clearLoggingBuffer()
{
    CogWheelLogger::getInstance().m_loggingBufferMutex.lock();
    CogWheelLogger::getInstance().m_loggingBuffer.clear();
    CogWheelLogger::getInstance().m_loggingBufferMutex.unlock();
}

// Base string logging

inline void cogWheelInfo(const QString &message) { if (getLoggingLevel() & CogWheelLogger::Info) CogWheelLogger::getInstance().info(message); }
inline void cogWheelError(const QString &message) { if (getLoggingLevel() & CogWheelLogger::Error) CogWheelLogger::getInstance().error(message); }
inline void cogWheelWarning(const QString &message) { if (getLoggingLevel() & CogWheelLogger::Warning) CogWheelLogger::getInstance().warning(message); }

// Command channel logging (socket handle for command channel is passed in)

inline void cogWheelInfo (qintptr handle, const QString &message)
{
    if (getLoggingLevel() & CogWheelLogger::Channel) {
        CogWheelLogger::getInstance().info(QString("CHANNEL[%1]I: %2").arg(QString::number(handle), message).toStdString().c_str());
    }
}

inline void cogWheelError (qintptr handle, const QString &message)
{
    if (getLoggingLevel() & CogWheelLogger::Channel) {
        CogWheelLogger::getInstance().error(QString("CHANNEL[%1]E: %2").arg(QString::number(handle), message).toStdString().c_str());
    }
}

inline void cogWheelWarning (qintptr handle, const QString &message)
{
    if (getLoggingLevel() & CogWheelLogger::Channel) {
        CogWheelLogger::getInstance().warning(QString("CHANNEL[%1]W: %2").arg(QString::number(handle), message).toStdString().c_str());
    }
}
#endif // COGWHEELLOGGER_H

