#ifndef COGWHEELSERVERSETTINGS_H
#define COGWHEELSERVERSETTINGS_H

#include <QSettings>

class CogWheelServerSettings
{

public:

    // Load and save settings

    void load();
    void save();

    // Private data accessors

    QString serverName() const;
    void setServerName(const QString &serverName);
    QString serverVersion() const;
    void setServerVersion(const QString &serverVersion);
    bool allowSMNT() const;
    void setAllowSMNT(bool allowSMNT);
    qint64 serverPort() const;
    void setServerPort(const qint64 &serverPort);
    qint64 writeBytesSize() const;
    void setWriteBytesSize(const qint64 &writeBytesSize);

private:
    QString m_serverName;            // Server Name
    QString m_serverVersion;         // Server Version
    bool m_allowSMNT=false;          // ==true allow SMNT command
    qint64 m_serverPort;             // Server connection port
    qint64 m_writeBytesSize=1024*32; // No of bytes per write
    bool m_active=false;             // Server active

};
#endif // COGWHEELSERVERSETTINGS_H
