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
    QString m_serverName;
    QString m_serverVersion;
    bool m_allowSMNT=false;
    qint64 m_serverPort;

};
#endif // COGWHEELSERVERSETTINGS_H
