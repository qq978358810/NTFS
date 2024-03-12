#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "IncludeFile.h"
#include <QApplication>
QT_BEGIN_NAMESPACE
class DataManage;
QT_END_NAMESPACE
class MainWindow : public QApplication
{
    Q_OBJECT

public:
    MainWindow(int &argc, char **argv);
    ~MainWindow();

    void manageDriver();//管理所有设备
    void closeEvent(QCloseEvent *event);
    void createUi();//创建界面
    QRect initWindowRect();//初始化窗口大小和位置


    bool notify(QObject *receiver, QEvent *e) override;
signals:
    void SignalRecordLog(E_LOG_TYPE, QString);//记录日志，其他线程写入
    void finishAllThread();//结束所有线程
    void SignalSeedCount();//发送到—model计数有多少个盘符
public slots:

    void SlotExitProgram();//退出程序
    void SlotNewWindow(QRect);//新建窗口
    void closeWindow(QObject *receiver);//关闭窗口
    void showWindows();//显示全部窗口
    void about();//关于
public:

    //QList<WindowClass*> windowList;//保存新建窗口//管理内存
    DataManage * m_dataManage;
    QThread *m_dataManageThread;

};
#endif // MAINWINDOW_H
