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

    typedef std::function<void (CogWheelConnection *, QString) > FTPCommandFunction;

public:
    explicit CogWheelFTPCore(QObject *parent = nullptr);

    static void performCommand(CogWheelConnection *connection, const QString &command, const QString &arguments);
    static QString buildLISTLine(QFileInfo &file);
    static QString getResponseText(quint16 responseCode);

private:
//    static QString buildCWDPath(CogWheelConnection *connection, const QString& path);
//    static QString buildFilePath(CogWheelConnection *connection, const QString &file);
    static QString mapPathToLocal(CogWheelConnection *connection, const QString& path);
    static QString mapPathFromLocal(CogWheelConnection *connection, const QString& path);

    static void USER(CogWheelConnection *connection, QString arguments);
    static void LIST(CogWheelConnection *connection, QString arguments);
    static void FEAT(CogWheelConnection *connection, QString arguments);
    static void SYST(CogWheelConnection *connection, QString arguments);
    static void PWD(CogWheelConnection *connection, QString arguments);
    static void TYPE(CogWheelConnection *connection, QString arguments);
    static void PORT(CogWheelConnection *connection, QString arguments);
    static void CWD(CogWheelConnection *connection, QString arguments);
    static void PASS(CogWheelConnection *connection, QString arguments);
    static void CDUP(CogWheelConnection *connection, QString arguments);
    static void RETR(CogWheelConnection *connection, QString arguments);
    static void NOOP(CogWheelConnection *connection, QString arguments);
    static void MODE(CogWheelConnection *connection, QString arguments);
    static void STOR(CogWheelConnection *connection, QString arguments);
    static void PASV(CogWheelConnection *connection, QString arguments);
    static void HELP(CogWheelConnection *connection, QString arguments);
    static void STAT(CogWheelConnection *connection, QString arguments);
    static void SITE(CogWheelConnection *connection, QString arguments);
    static void NLST(CogWheelConnection *connection, QString arguments);
    static void MKD(CogWheelConnection *connection, QString arguments);
    static void RMD(CogWheelConnection *connection, QString arguments);
    static void DELE(CogWheelConnection *connection, QString arguments);
    static void ACCT(CogWheelConnection *connection, QString arguments);
    static void SMNT(CogWheelConnection *connection, QString arguments);
    static void QUIT(CogWheelConnection *connection, QString arguments);
    static void REIN(CogWheelConnection *connection, QString arguments);
    static void STRU(CogWheelConnection *connection, QString arguments);
    static void STOU(CogWheelConnection *connection, QString arguments);
    static void APPE(CogWheelConnection *connection, QString arguments);
    static void ALLO(CogWheelConnection *connection, QString arguments);
    static void REST(CogWheelConnection *connection, QString arguments);
    static void RNFR(CogWheelConnection *connection, QString arguments);
    static void RNTO(CogWheelConnection *connection, QString arguments);
    static void ABOR(CogWheelConnection *connection, QString arguments);

signals:

public slots:

public:

    static QHash<QString, FTPCommandFunction> m_unauthCommandTable;
    static QHash<QString, FTPCommandFunction> m_ftpCommandTable;
    static QHash<quint16, QString> m_ftpServerResponse;

};

#endif // COGWHEELFTPCORE_H
