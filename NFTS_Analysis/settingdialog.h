#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include "IncludeFile.h"

QT_BEGIN_NAMESPACE
class QProcess;
class QSystemTrayIcon;
class QAbstractButton;
QT_END_NAMESPACE

#define AUTO_RUN_KEY	"HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define ADMIN_RUN_KEY   "HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers"
namespace Ui {
class SettingDialog;
}


class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = nullptr);
    ~SettingDialog();

    void AppAutoRun(bool checked);//开机自起
    void SystemTray(bool checked);//系统托盘

    void SystemAdminRun(bool checked);//以管理员身份运行

    S_SETTING_ARGS setArgs();//设置参数
    void Show();

    //创建托盘菜单
    void createMenu();
    static QProcess *AutoRunprocess;
protected:
    //关闭事件
    //void closeEvent(QCloseEvent *event);
signals:
    void SignalUpdateWidget();
protected:
    void SystemTrayProtected();//系统托盘
private slots:
    void on_AppAutoRuncheckBox_clicked(bool checked);

    void on_SystemTrayCheckBox_clicked(bool checked);

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_real_timeSearchCheckBox_clicked(bool checked);

    void on_enableDelFileFlagCheckBox_clicked(bool checked);

    void on_dataBaseLoadCheckBox_clicked(bool checked);

    void on_columnsComboBox_activated(int index);

    void on_pushButton_clicked();

    void on_hideSearchButton_clicked(bool checked);

    //当在系统托盘点击菜单内的显示主界面操作
    //void SlotShowMainAction();

    //当在系统托盘点击菜单内的退出程序操作
    //void SlotExitAppAction();

    void SlotShowWindow();//显示窗口
    void SlotNewWindow();//新建窗口
    void SlotOption();//选项
    void SlotExit();//退出
    void SlotAbout();//关于
    void on_windowTopCheckBox_clicked(bool checked);

    void on_usnRecordNoMonitoringCheckBox_clicked(bool checked);

private:
    Ui::SettingDialog *ui;
    QSystemTrayIcon *m_sysTrayIcon ;

};

#endif // SETTINGDIALOG_H
