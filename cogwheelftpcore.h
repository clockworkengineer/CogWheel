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
    static QString buildCWDPath(CogWheelConnection *connection, const QString& path);
    static QString buildFilePath(CogWheelConnection *connection, const QString &file);

    static void USER(CogWheelConnection *connection, QStringList commandAndArgments);
    static void LIST(CogWheelConnection *connection, QStringList commandAndArgments);
    static void FEAT(CogWheelConnection *connection, QStringList commandAndArgments);
    static void SYST(CogWheelConnection *connection, QStringList commandAndArgments);
    static void PWD(CogWheelConnection *connection, QStringList commandAndArgments);
    static void TYPE(CogWheelConnection *connection, QStringList commandAndArgments);
    static void PORT(CogWheelConnection *connection, QStringList commandAndArgments);
    static void CWD(CogWheelConnection *connection, QStringList commandAndArgments);
    static void PASS(CogWheelConnection *connection, QStringList commandAndArgments);
    static void CDUP(CogWheelConnection *connection, QStringList commandAndArgments);
    static void RETR(CogWheelConnection *connection, QStringList commandAndArgments);
    static void NOOP(CogWheelConnection *connection, QStringList commandAndArgments);
    static void MODE(CogWheelConnection *connection, QStringList commandAndArgments);
    static void STOR(CogWheelConnection *connection, QStringList commandAndArgments);
    static void PASV(CogWheelConnection *connection, QStringList commandAndArgments);
    static void HELP(CogWheelConnection *connection, QStringList commandAndArgments);
    static void STAT(CogWheelConnection *connection, QStringList commandAndArgments);
    static void SITE(CogWheelConnection *connection, QStringList commandAndArgments);
    static void NLST(CogWheelConnection *connection, QStringList commandAndArgments);
    static void MKD(CogWheelConnection *connection, QStringList commandAndArgments);
    static void RMD(CogWheelConnection *connection, QStringList commandAndArgments);
    static void DELE(CogWheelConnection *connection, QStringList commandAndArgments);
    static void ACCT(CogWheelConnection *connection, QStringList commandAndArgments);
    static void SMNT(CogWheelConnection *connection, QStringList commandAndArgments);
    static void QUIT(CogWheelConnection *connection, QStringList commandAndArgments);
    static void REIN(CogWheelConnection *connection, QStringList commandAndArgments);
    static void STRU(CogWheelConnection *connection, QStringList commandAndArgments);
    static void STOU(CogWheelConnection *connection, QStringList commandAndArgments);
    static void APPE(CogWheelConnection *connection, QStringList commandAndArgments);
    static void ALLO(CogWheelConnection *connection, QStringList commandAndArgments);
    static void REST(CogWheelConnection *connection, QStringList commandAndArgments);
    static void RNFR(CogWheelConnection *connection, QStringList commandAndArgments);
    static void RNTO(CogWheelConnection *connection, QStringList commandAndArgments);
    static void ABOR(CogWheelConnection *connection, QStringList commandAndArgments);

signals:

public slots:

public:

    static QMap<QString, FTPCommandFunction> m_unauthorizedCommandTable;
    static QMap<QString, FTPCommandFunction> m_ftpCommandTable;
    static QMap<quint16, QString> m_ftpServerResponse;

};

#endif // COGWHEELFTPCORE_H
