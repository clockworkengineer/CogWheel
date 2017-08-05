#ifndef COGWHEELFTPCORE_H
#define COGWHEELFTPCORE_H

#include <cogwheelconnection.h>

#include <QObject>
#include <QMap>
#include <QString>
#include <QFileInfo>

class CogWheelFTPCore : public QObject
{
    Q_OBJECT

    typedef std::function<void (CogWheelConnection *, QStringList) > FTPCommandFunction;

public:
    explicit CogWheelFTPCore(QObject *parent = nullptr);

    static void performCommand(CogWheelConnection *connection, QStringList commandAndArgments);
    static QString buildLISTLine(QFileInfo &file);
    static QString getResponseText(quint16 responseCode);

private:
    static void commandUSER(CogWheelConnection *connection, QStringList commandAndArgments);
    static void commandLIST(CogWheelConnection *connection, QStringList commandAndArgments);
    static void commandFEAT(CogWheelConnection *connection, QStringList commandAndArgments);
    static void commandSYST(CogWheelConnection *connection, QStringList commandAndArgments);
    static void commandPWD(CogWheelConnection *connection, QStringList commandAndArgments);
    static void commandTYPE(CogWheelConnection *connection, QStringList commandAndArgments);
    static void commandPORT(CogWheelConnection *connection, QStringList commandAndArgments);
    static void commandCWD(CogWheelConnection *connection, QStringList commandAndArgments);

signals:

public slots:

public:

    static QMap<QString, FTPCommandFunction> m_ftpCommandTable;
    static QMap<quint16, QString> m_ftpServerResponse;

};

#endif // COGWHEELFTPCORE_H
