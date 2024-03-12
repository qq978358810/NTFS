#ifndef HIGHFINDDIALOG_H
#define HIGHFINDDIALOG_H

#include <QDialog>
#include <QStyleFactory>
#include <IncludeFile.h>

class QAbstractButton;

namespace Ui {
class HighFindDialog;
}

class HighFindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HighFindDialog(QWidget *parent = nullptr);
    ~HighFindDialog();
signals:
    void SignalHighFind(S_HIGHFIND_UPDATE_VIEW);

private slots:

    void on_leftFileCreationTimeDateEdit_dateChanged(const QDate &date);

    void on_rightFileCreationTimeDateEdit_dateChanged(const QDate &date);

    void on_leftFileChangeTimeDateEdit_dateChanged(const QDate &date);

    void on_rightFileChangeTimeDateEdit_dateChanged(const QDate &date);

    void on_leftFileReadTimeDateEdit_dateChanged(const QDate &date);

    void on_rightFileReadTimeDateEdit_dateChanged(const QDate &date);

//    void on_completeCheckBox_clicked(bool checked);

//    void on_RegExpCheckBox_clicked(bool checked);

    void on_FileAttributeLineComboBox_currentIndexChanged(const QString &arg1);

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_resetPushButton_clicked();

    void on_leftFileRealSizeComboBox_activated(int index);

    void on_rightFileRealSizeComboBox_activated(int index);

    void on_leftFileCreationTimeCheckBox_stateChanged(int arg1);

    void on_rightFileCreationTimeCheckBox_stateChanged(int arg1);

    void on_leftFileChangeTimeCheckBox_stateChanged(int arg1);

    void on_rightFileChangeTimeCheckBox_stateChanged(int arg1);

    void on_leftFileReadTimeCheckBox_stateChanged(int arg1);

    void on_rightFileReadTimeCheckBox_stateChanged(int arg1);

    void on_RegExpCheckBox_stateChanged(int arg1);

    void on_completeCheckBox_stateChanged(int arg1);

    void on_FileNameLineEdit_textEdited(const QString &arg1);

private:
    Ui::HighFindDialog *ui;
};

#endif // HIGHFINDDIALOG_H
