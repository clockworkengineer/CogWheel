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

QStringListModel CogWheelManagerLoggingDialog::m_loggingBuffer;

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

    ui->logListView->setModel(&m_loggingBuffer);

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
 * @brief CogWheelManagerLoggingDialog::clearLoggingBuffer
 *
 * Clear local logging buffer view.
 *
 */
void CogWheelManagerLoggingDialog::clearLoggingBuffer()
{
    m_loggingBuffer.setStringList( QStringList{} );
}

/**
 * @brief CogWheelManagerLoggingDialog::logWindowUpdate
 *
 * Append recieved logging buffer contents to dialog text area.
 *
 * @param logBuffer
 */
void CogWheelManagerLoggingDialog::logWindowUpdate(const QStringList &logBuffer)
{
    for (auto line : logBuffer) {
        m_loggingBuffer.insertRow(m_loggingBuffer.rowCount());
        QModelIndex index = m_loggingBuffer.index(m_loggingBuffer.rowCount()-1);
        m_loggingBuffer.setData(index, line);
    }

    ui->logListView->setCurrentIndex(m_loggingBuffer.index(m_loggingBuffer.rowCount()-1));

}
