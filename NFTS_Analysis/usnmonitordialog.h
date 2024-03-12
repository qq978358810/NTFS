#ifndef USNMONITORDIALOG_H
#define USNMONITORDIALOG_H

#include <QDialog>
#include <QPushButton>
#include "IncludeFile.h"
namespace Ui {
class UsnMonitorDialog;
}

class UsnMonitorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UsnMonitorDialog(QWidget *parent = nullptr);
    ~UsnMonitorDialog();
    S_MONITOR_ITEM getArgs();
    void setArgs(S_MONITOR_ITEM);
    //清空参数
    void clearArgs();

    bool  isOk();

private:
    void SignalUpdateWidget(S_MONITOR_ITEM);
private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

    void on_RegExpCheckBox_stateChanged(int arg1);

    void on_completeCheckBox_stateChanged(int arg1);

    void on_fileNameLineEdit_6_textEdited(const QString &arg1);

private:
    Ui::UsnMonitorDialog *ui;
    bool m_ok;
};

#endif // USNMONITORDIALOG_H
