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

#include <QDialog>
#include <QSettings>

namespace Ui {
class CogWheelUserListDialog;
}

class CogWheelUserListDialog : public QDialog
{
    Q_OBJECT

public:

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

    Ui::CogWheelUserListDialog *ui;     // Q dialog data
    QStringList m_userList;             // User list

};

#endif // COGWHEELUSERLISTDIALOG_H