/*
 * File:   cogwheelmanagermain.h
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

#ifndef COGWHEELMANAGERMAIN_H
#define COGWHEELMANAGERMAIN_H

#include <QMainWindow>

namespace Ui {
class CogWheelManagerMain;
}

class CogWheelManagerMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit CogWheelManagerMain(QWidget *parent = 0);
    ~CogWheelManagerMain();

private slots:
    void on_actionEditServerSettings_triggered();

    void on_actionEditUser_triggered();

private:
    Ui::CogWheelManagerMain *ui;
};

#endif // COGWHEELMANAGERMAIN_H
