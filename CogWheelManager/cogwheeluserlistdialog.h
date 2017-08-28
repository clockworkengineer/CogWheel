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
    explicit CogWheelUserListDialog(QWidget *parent = 0);
    ~CogWheelUserListDialog();

    void loadUserList();
    void saveUserList();
    void populateUserList();

private slots:

    void on_newButton_clicked();
    void on_editButton_clicked();
    void on_RemoveButton_clicked();
    void on_userList_clicked(const QModelIndex &index);

private:
    Ui::CogWheelUserListDialog *ui;

    QStringList m_userList;

};

#endif // COGWHEELUSERLISTDIALOG_H
