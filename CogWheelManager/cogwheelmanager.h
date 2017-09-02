#ifndef COGWHEELMANAGER_H
#define COGWHEELMANAGER_H

#include <QObject>
#include <QLocalSocket>
#include <QDataStream>

class CogWheelManager : public QObject
{
    Q_OBJECT
public:
    explicit CogWheelManager(QObject *parent = nullptr);

    void startManager(const QString &socketName);
    void stopManager();
    void writeCommand(const QString &command);

signals:

public slots:
    void connected();
    void disconnected();
    void error(QLocalSocket::LocalSocketError socketError);
    void readyRead();
    void bytesWritten(qint64 bytes);

private:
    QString m_socketName;
    QLocalSocket *m_managerSocket;

};

#endif // COGWHEELMANAGER_H
