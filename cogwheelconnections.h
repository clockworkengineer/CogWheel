#ifndef COGWHEELCONNECTIONS_H
#define COGWHEELCONNECTIONS_H

#include "cogwheelconnection.h"

#include <QObject>

class CogWheelConnections : public QObject
{
    Q_OBJECT

public:
    explicit CogWheelConnections(QObject *parent = nullptr);

signals:
    void openConnection(qint64 handle);

public slots:
    void acceptConnection(qint64 handle);
    void finishedConnection(qint64 handle);
    void abortedConnection(qint64 handle);

private:
    QMap<qint64, CogWheelConnection *> m_connections;

};

#endif // COGWHEELCONNECTIONS_H
