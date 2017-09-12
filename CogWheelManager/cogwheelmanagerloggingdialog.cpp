#include "cogwheelmanagerloggingdialog.h"
#include "ui_cogwheelmanagerloggingdialog.h"

CogWheelManagerLoggingDialog::CogWheelManagerLoggingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CogWheelManagerLoggingDialog)
{
    ui->setupUi(this);


}

CogWheelManagerLoggingDialog::~CogWheelManagerLoggingDialog()
{
    delete ui;
}

void CogWheelManagerLoggingDialog::logWindowUpdate(const QStringList &logBuffer)
{
    for (auto line : logBuffer) {
        ui->logTextArea->append(line);
    }
}
