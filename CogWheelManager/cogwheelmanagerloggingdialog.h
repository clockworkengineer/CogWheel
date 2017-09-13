#ifndef COGWHEELMANAGERLOGGINGDIALOG_H
#define COGWHEELMANAGERLOGGINGDIALOG_H

#include <QDialog>

namespace Ui {
class CogWheelManagerLoggingDialog;
}

class CogWheelManagerLoggingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CogWheelManagerLoggingDialog(QWidget *parent = 0);
    ~CogWheelManagerLoggingDialog();

public slots:

    void  logWindowUpdate(const QStringList &logBuffer);

private:

    Ui::CogWheelManagerLoggingDialog *ui;   // Qt dialog data

};

#endif // COGWHEELMANAGERLOGGINGDIALOG_H
