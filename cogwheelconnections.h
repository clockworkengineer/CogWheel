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
    void open(qint64 handle);
    void close();

public slots:
    void accept(qint64 handle);
    void close(qint64 handle);
//    void finished(qint64 handle);
    void aborted(qint64 handle);

private:
    QVector<CogWheelConnection *>  m_connections;

};

#endif // COGWHEELCONNECTIONS_H
