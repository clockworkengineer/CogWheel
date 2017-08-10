#include <QCoreApplication>
#include "cogwheelserver.h"
#include "cogwheelusersettings.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    CogWheelServer server;

    CogWheelUserSettings::defaultSettings();

    server.startServer();

    return a.exec();
}
