#ifndef COGWHEELSERVERSETTINGS_H
#define COGWHEELSERVERSETTINGS_H

#include <QSettings>

class CogWheelServerSettings
{

public:
    void load();
    void save();

    QString serverName() const;
    void setServerName(const QString &serverName);
    QString serverVersion() const;
    void setServerVersion(const QString &serverVersion);
    bool allowSMNT() const;
    void setAllowSMNT(bool allowSMNT);
    qint64 serverPort() const;
    void setServerPort(const qint64 &serverPort);

private:
    QString m_serverName;       // Server Name
    QString m_serverVersion;    // Server Version
    bool m_allowSMNT=false;     // ==true allow SMNT command
    qint64 m_serverPort;        // Server connection port

};
#endif // COGWHEELSERVERSETTINGS_H
