#ifndef LOGITEMDIALOG_H
#define LOGITEMDIALOG_H

#include <QDialog>
#include <IncludeFile.h>
namespace Ui {
class LogItemDialog;
}

class LogItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogItemDialog(QWidget *parent = nullptr);
    ~LogItemDialog();

signals:
    void SignalUsnLogItem(S_USN_LOG_ITEM);
private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::LogItemDialog *ui;
};

#endif // LOGITEMDIALOG_H
