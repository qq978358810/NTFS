#ifndef LOGMONITORINGDIALOG_H
#define LOGMONITORINGDIALOG_H

#include <QDialog>

namespace Ui {
class LogMonitoringDialog;
}

class LogMonitoringDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogMonitoringDialog(QWidget *parent = nullptr);
    ~LogMonitoringDialog();

private:
    Ui::LogMonitoringDialog *ui;
};

#endif // LOGMONITORINGDIALOG_H
