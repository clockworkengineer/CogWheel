#ifndef COGWHEELMANAGERLOGGINGDIALOG_H
#define COGWHEELMANAGERLOGGINGDIALOG_H

//
// Class: CogWheelManagerLoggingDialog
//
// Description: Class to display server logging window and place any logging
// text received from the server into it.
//

// =============
// INCLUDE FILES
// =============

#include <QDialog>
#include <QStringListModel>

// =================
// CLASS DECLARATION
// =================

namespace Ui {
class CogWheelManagerLoggingDialog;
}

class CogWheelManagerLoggingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CogWheelManagerLoggingDialog(QWidget *parent = 0);
    ~CogWheelManagerLoggingDialog();

    static void clearLoggingBuffer();

public slots:

    void  logWindowUpdate(const QStringList &logBuffer);

private:

    Ui::CogWheelManagerLoggingDialog *ui;   // Qt dialog data

    static QStringListModel m_loggingBuffer;

};

#endif // COGWHEELMANAGERLOGGINGDIALOG_H
