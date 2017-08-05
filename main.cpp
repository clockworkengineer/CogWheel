#include <QCoreApplication>
#include "cogwheelserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    CogWheelServer server;

    server.startServer();

    return a.exec();
}
