QT += core
QT += network
QT -= gui

CONFIG += c++11

TARGET = CogWheel
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += CogWheelServer/main.cpp \
    CogWheelServer/cogwheelserver.cpp \
    CogWheelServer/cogwheeldatachannel.cpp \
    CogWheelServer/cogwheelftpcore.cpp \
    CogWheelServer/cogwheelconnections.cpp \
    CogWheelSettings/cogwheelusersettings.cpp \
    CogWheelServer/cogwheelcontrolchannel.cpp \
    CogWheelSettings/cogwheelserversettings.cpp \
    CogWheelServer/cogwheelcontroller.cpp \
    CogWheelServer/cogwheelftpcoreutil.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    CogWheelServer/cogwheelserver.h \
    CogWheelServer/cogwheeldatachannel.h \
    CogWheelServer/cogwheelftpcore.h \
    CogWheelServer/cogwheelconnections.h \
    CogWheelSettings/cogwheelusersettings.h \
    CogWheelServer/cogwheelcontrolchannel.h \
    CogWheelSettings/cogwheelserversettings.h \
    CogWheelServer/cogwheelcontroller.h \
    CogWheelServer/cogwheellogger.h \
    CogWheelServer/cogwheel.h \
    CogWheelServer/cogwheelftpserverreply.h \
    CogWheelServer/cogwheelftpcoreutil.h

INCLUDEPATH += $$PWD/CogWheelServer/ \
               $$PWD/CogWheelSettings/
DEPENDPATH += $$PWD/CogWheelServer/ \
              $$PWD/CogWheelSettings/
