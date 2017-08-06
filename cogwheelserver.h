#ifndef COGWHEELSERVER_H
#define COGWHEELSERVER_H


#include "cogwheelconnections.h"

#include <QObject>
#include <QTcpServer>
//#include <QThread>

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
    void open(qint64 handle);

public slots:

public:
//    QThread m_serverThread;
    QString m_user="user";
    QString m_password="password";
    qint64 m_controlPort=2221;
    CogWheelConnections m_connections;

};

#endif // COGWHEELSERVER_H
