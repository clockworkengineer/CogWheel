/*
 * File:   cogwheeluserlistdialog.h
 *
 * Author: Robert Tizzard
 *
 * Created on August 10, 2017
 *
 * Copyright 2017.
 *
 */
#ifndef COGWHEELUSERLISTDIALOG_H
#define COGWHEELUSERLISTDIALOG_H

//
// Class: CogWheelUserListDialog
//
// Description: Class to display user list dialog.
//

// =============
// INCLUDE FILES
// =============

#include "../cogwheel.h"

#include <QDialog>
#include <QSettings>

// =================
// CLASS DECLARATION
// =================

namespace Ui {
class CogWheelUserListDialog;
}

class CogWheelUserListDialog : public QDialog
{
    Q_OBJECT

public:

    // Class exception

    struct Exception : public std::runtime_error {

        Exception(const QString & messageStr)
            : std::runtime_error(static_cast<QString>("CogWheelUserListDialog Failure: " + messageStr).toStdString()) {
        }

    };

    // Constructor / Destructor

    explicit CogWheelUserListDialog(QWidget *parent = 0);
    ~CogWheelUserListDialog();

    // Load / Save user list

    void loadUserList();
    void saveUserList();

    // Populate user list control

    void populateUserList();

private slots:

    // Dialog controls

    void on_newButton_clicked();
    void on_editButton_clicked();
    void on_RemoveButton_clicked();
    void on_userList_clicked(const QModelIndex &index);

private:

    Ui::CogWheelUserListDialog *ui;     // Qt dialog data

    QStringList m_userList;             // User list

};

#endif // COGWHEELUSERLISTDIALOG_H
