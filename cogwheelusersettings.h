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
    static bool checkPassword(const QString& userName, const QString& password);
    static QString getHomePath(const QString& userName);

signals:

public slots:

private:
    QString userName;
    QString encyptedPassword;
    QString homePath;
    bool passive;
};

#endif // COGWHEELUSERSETTINGS_H
