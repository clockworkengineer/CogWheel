#ifndef COGWHEELUSERSETTINGS_H
#define COGWHEELUSERSETTINGS_H

#include <QObject>
#include <QSettings>

class CogWheelUserSettings : public QObject
{
    Q_OBJECT

public:
    explicit CogWheelUserSettings(QObject *parent = nullptr);

    static void defaultSettings();
    static bool checkUserName(const QString& userName);
    static bool checkUserPassword(const QString& userName, const QString& password);
    static QString getRootPath(const QString& userName);

signals:

public slots:

private:
    QString m_userName;
    QString m_userPassword;
    QString m_rootPath;
    QString m_accountName;
    bool m_passiveMode;

};

#endif // COGWHEELUSERSETTINGS_H
