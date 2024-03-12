#include "logmonitoringdialog.h"
#include "ui_logmonitoringdialog.h"

LogMonitoringDialog::LogMonitoringDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogMonitoringDialog)
{
    ui->setupUi(this);
}

LogMonitoringDialog::~LogMonitoringDialog()
{
    delete ui;
}
