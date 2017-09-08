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
    static CogWheelLogger& getInstance()
    {
        static CogWheelLogger    instance;
        return instance;
    }

    friend void cogWheelInfo(const QString &message);
    friend void cogWheelError(const QString &message);
    friend void cogWheelWarning(const QString &message);

private:
    CogWheelLogger() {};

    void info(const QString &message) { qInfo() << message.toStdString().c_str(); }
    void error(const QString &message) { qDebug() << message.toStdString().c_str(); }
    void warning(const QString &message) { qWarning() << message.toStdString().c_str(); }

public:

    CogWheelLogger(const CogWheelLogger & orig) = delete;
    CogWheelLogger(const CogWheelLogger && orig) = delete;
    CogWheelLogger& operator=(CogWheelLogger other) = delete;

};

inline void cogWheelInfo(const QString &message) { CogWheelLogger::getInstance().info(message); }
inline void cogWheelError(const QString &message) { CogWheelLogger::getInstance().error(message); }
inline void cogWheelWarning(const QString &message) { CogWheelLogger::getInstance().warning(message); }

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

#endif // COGWHEELLOGGER_H
