#ifndef COGWHEELSERVERSETTINGS_H
#define COGWHEELSERVERSETTINGS_H

#include <QSettings>
#include <QFile>

class CogWheelServerSettings
{

public:

    // Load and save settings

    void load();
    void save();

    bool loadPrivateKeyAndCert();

    // Private data accessors

    QString serverName() const;
    void setServerName(const QString &serverName);
    QString serverVersion() const;
    void setServerVersion(const QString &serverVersion);
    bool serverAllowSMNT() const;
    void setServerAllowSMNT(bool serverAllowSMNT);
    qint64 serverPort() const;
    void setServerPort(const qint64 &serverPort);
    qint64 serverWriteBytesSize() const;
    void setServerWriteBytesSize(const qint64 &serverWriteBytesSize);
    bool serverEnabled() const;
    void setServerEnabled(bool serverEnabled);
    bool serverSslEnabled() const;
    void setServerSslEnabled(bool serverSslEnabled);
    bool serverPlainFTPEnabled() const;
    void setServerPlainFTPEnabled(bool serverPlainFTPEnabled);
    QByteArray serverPrivateKey() const;
    void setServerPrivateKey(const QByteArray &serverPrivateKey);
    QByteArray serverCert() const;
    void setServerCert(const QByteArray &serverCert);
    QString serverKeyFileName() const;
    void setServerKeyFileName(const QString &serverKeyFileName);
    QString serverCertFileName() const;
    void setServerCertFileName(const QString &serverCertFileName);
    bool serverAnonymousEnabled() const;
    void setServerAnonymousEnabled(bool serverAnonymousEnabled);

private:

    QString m_serverName;                  // Server Name
    QString m_serverVersion;               // Server Version
    bool m_serverAllowSMNT=false;          // ==true allow SMNT command
    qint64 m_serverPort;                   // Server connection port
    qint64 m_serverWriteBytesSize=1024*32; // No of bytes per write
    bool m_serverEnabled=false;            // ==true Server enabled
    bool m_serverSslEnabled=false;         // ==true TLS/SSL enabled
    bool m_servePlainFTPEnabled=false;     // ==true Plain insecure FTP enabled
    bool m_serverAnonymousEnabled=false;         // == true anonymous login enabled
    QString m_serverKeyFileName;           // Server private key file name
    QString m_serverCertFileName;          // Server certifucate file

    QByteArray m_serverPrivateKey;         // Server private key
    QByteArray m_serverCert;               // Server Certificate

};
#endif // COGWHEELSERVERSETTINGS_H
