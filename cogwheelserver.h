#ifndef COGWHEELSERVER_H
#define COGWHEELSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QThread>

#include "cogwheelconnection.h"

class CogWheelServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit CogWheelServer(QObject *parent = nullptr);
    void startServer(void);
    void stopServer(void);

protected:
    void incomingConnection(qintptr handle);

signals:

public slots:

private:
    QThread m_serverThread;
    QString m_user="user";
    QString m_password="password";
    qint64 m_controlPort=2221;
    QVector<CogWheelConnection *>  m_connections;
};

#endif // COGWHEELSERVER_H
