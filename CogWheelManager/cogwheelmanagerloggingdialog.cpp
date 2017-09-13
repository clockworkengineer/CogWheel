/*
 * File:   cogwheelmanagerloggingdialog.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */

//
// Class: CogWheelManagerLoggingDialog
//
// Description: Class to display server logging window and place any logging
// text received from the server into it.
//

// =============
// INCLUDE FILES
// =============

#include "cogwheelmanagerloggingdialog.h"
#include "ui_cogwheelmanagerloggingdialog.h"

/**
 * @brief CogWheelManagerLoggingDialog::CogWheelManagerLoggingDialog
 *
 * Dialog contructor.
 *
 * @param parent
 */
CogWheelManagerLoggingDialog::CogWheelManagerLoggingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CogWheelManagerLoggingDialog)
{
    ui->setupUi(this);


}

/**
 * @brief CogWheelManagerLoggingDialog::~CogWheelManagerLoggingDialog
 *
 * Dialog destructor.
 *
 */
CogWheelManagerLoggingDialog::~CogWheelManagerLoggingDialog()
{
    delete ui;
}

/**
 * @brief CogWheelManagerLoggingDialog::logWindowUpdate
 *
 * Append recieved logging buufer contents to dialog text area.
 * (NEED TO MAKE THIS A VIEW SO DONT LOOSE CONTENTS).
 *
 * @param logBuffer
 */
void CogWheelManagerLoggingDialog::logWindowUpdate(const QStringList &logBuffer)
{
    for (auto line : logBuffer) {
        ui->logTextArea->append(line);
    }
}
