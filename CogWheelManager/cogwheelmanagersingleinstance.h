/*
 * File:   cogwheelmanagersingleinstance.h
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */
#ifndef COGWHEELMANAGERSINGLEINSTANCE_H
#define COGWHEELMANAGERSINGLEINSTANCE_H

#include <QObject>
#include <QLocalServer>
#include <QLocalSocket>
#include <QApplication>
#include <QMainWindow>

class CogWheelManagerSingleInstance : public QLocalServer
{
     Q_OBJECT

public:
    CogWheelManagerSingleInstance(QApplication *appInstance, QObject *parent = nullptr);
    ~CogWheelManagerSingleInstance();

    bool alreadyRunning() const;
    void setMainWindow(QMainWindow *mainWindow);

protected:

    // QLocalServer override

    void incomingConnection(quintptr handle);

private:

    QApplication *m_appInstance;
    QMainWindow *m_mainWindow;
    QLocalSocket *m_appInstanceSocket=nullptr;
    bool m_alreadyRunning=false;

};

#endif // COGWHEELMANAGERSINGLEINSTANCE_H
