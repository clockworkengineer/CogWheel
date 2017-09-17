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

//
// Class: CogWheelLogger
//
// Description: Used to provide server logging to manager program / file. It is an singleton
// class whose interface and implementaion is provided in this single include file.
// Note: If the server is not connected to the manager only the last kCWLoggingBufferLineMax
// lines are kept in the buffer (for simplicity and space). Logging is also sent to a file is
// so configured.
//

// =============
// INCLUDE FILES
// =============

#include <QString>
#include <QMutex>
#include <QFile>
#include <QDateTime>

// =================
// CLASS DECLARATION
// =================

class CogWheelLogger
{

public:

    // Class exception

    struct Exception : public std::runtime_error {

        Exception(const QString & messageStr)
            : std::runtime_error(QString("CogWheelLogger Failure: " + messageStr).toStdString()) {
        }

    };

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

    // Clear logging buffer

    void clearLoggingBuffer()
    {
        m_loggingBufferMutex.lock();
        m_loggingBuffer.clear();
        m_loggingBufferMutex.unlock();
    }

    QString getLogFileName() const
    {
        return m_logFileName;
    }

    // Private data accessors

    QStringList getLoggingBuffer() const { return m_loggingBuffer; }
    bool getLoggingEnabled() const { return m_enabled; }
    void setLoggingEnabled(bool enabled)  { m_enabled = enabled; }

    // Friend functions

    friend void cogWheelInfo(const QString &message);
    friend void cogWheelInfo (qintptr handle, const QString &message);
    friend void cogWheelError(const QString &message);
    friend void cogWheelError(qintptr handle, const QString &message);
    friend void cogWheelWarning(const QString &message);
    friend void cogWheelWarning(qintptr handle, const QString &message);
    friend void setLoggingLevel(const QStringList &logLevels);
    friend void clearLoggingBuffer();
    friend quint64 getLoggingLevel();
    friend void flushLoggingFile();
    friend void  setLogFileName(const QString &logFileName);

private:

    // Constuctor

    CogWheelLogger() {};

    // Append message to buffer (use mutex as logging can happen on different threads).

    void appendMessageToLogBuffer(const QString &message) {
        m_loggingBufferMutex.lock();
        if (m_loggingBuffer.size() == kCWLoggingBufferLineMax) {
            m_loggingBuffer.removeFirst();
        }
        m_loggingBuffer.append(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss")+" : "+message);
        if (m_logFile.isOpen()) {
            m_logFile.write(m_loggingBuffer.back().toUtf8()+"\n");
            m_logFile.flush();  // THIS IS INEFFICIENT AND NEEDS TO BE CHANGED
        }
        m_loggingBufferMutex.unlock();
    }

    // Base line logging.

    void info(const QString &message) { if (m_enabled) appendMessageToLogBuffer(message); }
    void error(const QString &message) { if (m_enabled) appendMessageToLogBuffer(message);}
    void warning(const QString &message) { if (m_enabled) appendMessageToLogBuffer(message);}

    bool m_enabled=false;           // == true logging enabled
    quint64 m_loggingLevel;         // Logging level
    QMutex m_loggingBufferMutex;    // Logging buffer mutex
    QStringList m_loggingBuffer;    // Logging buffer
    QString m_logFileName;          // Logging file name
    QFile m_logFile;                // Logging file

};

// Set logging level (could do this with a table)

inline void setLoggingLevel(const QStringList &logLevels) {

    CogWheelLogger::getInstance().m_loggingLevel=CogWheelLogger::None;

    for (auto level : logLevels) {
        if (level=="None") {
            CogWheelLogger::getInstance().m_loggingLevel |= CogWheelLogger::None;
        } else if (level =="Warning") {
            CogWheelLogger::getInstance().m_loggingLevel |= CogWheelLogger::Warning;
        } else if (level =="Info") {
            CogWheelLogger::getInstance().m_loggingLevel |= CogWheelLogger::Info;
        } else if (level =="Error") {
            CogWheelLogger::getInstance().m_loggingLevel |= CogWheelLogger::Error;
        } else if (level =="Channel") {
            CogWheelLogger::getInstance().m_loggingLevel |= CogWheelLogger::Channel;
        }else if (level =="All") {
            CogWheelLogger::getInstance().m_loggingLevel |= CogWheelLogger::All;
        }

    }

}

// Get loggin level

inline quint64 getLoggingLevel()
{
    return CogWheelLogger::getInstance().m_loggingLevel;
}

// Flush logging to file

inline void flushLoggingFile()
{
    if (CogWheelLogger::getInstance().m_logFile.isOpen())  {
        CogWheelLogger::getInstance().m_logFile.flush();
    }
}

// Open logging for for append if configured

inline void  setLogFileName(const QString &logFileName)
{
    if (!logFileName.isEmpty()) {
        if (!CogWheelLogger::getInstance().m_logFile.isOpen()) {
            CogWheelLogger::getInstance().m_logFileName = logFileName;
            CogWheelLogger::getInstance().m_logFile.setFileName(logFileName);
            CogWheelLogger::getInstance().m_logFile.open(QFile::Append);
        }
    }
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

