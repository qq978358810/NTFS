#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QMainWindow>
#include "logmodel.h"
#include "IncludeFile.h"


QT_BEGIN_NAMESPACE
class QProcess;
class QLabel;
class UsnMonitorDialog;
QT_END_NAMESPACE

namespace Ui {
class LogWindow;
}

class LogWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LogWindow(QWidget *parent = nullptr);
    ~LogWindow();
    void closeEvent(QCloseEvent *event);
    //创建单例
    static LogWindow * getInstance();

signals:
public slots:

    //添加数据到实时界面
    void SlotRealPlainTextEditAddData(E_LOG_TYPE,QVariant);
    //显示右键菜单
    void SlotShowMenu(const QPoint &pos);
    //复制数据
    void SlotCopyData();

private slots:

    void on_clearButton_clicked();
    //状态栏更新
    void SlotStatusbarUpdata(E_STATUSBAR_UPDATA,QVariant);

    void on_deleteButton_clicked();

    void on_updateButton_clicked();

    void on_upButton_clicked();

    void on_downButton_clicked();

    void on_exportButton_clicked();

    void on_appendButton_clicked();

    void on_insertButton_clicked();

    void on_doubleSpinBox_valueChanged(double arg1);

private:
    Ui::LogWindow *ui;

    //禁止拷贝构造函数。
    LogWindow(const LogWindow &);
    //禁止赋值拷贝函数。
    LogWindow & operator=(const LogWindow &);

    //单例对象
    static QAtomicPointer<LogWindow> m_instance;
    //互斥锁
    static QMutex m_mutex;
    //状态栏显示
    QLabel *m_statuLabe;
    //历史文件路径
    QString m_path;

    //历史文件大小
    qint64 m_historyFileSize;

    QMutex m_mutexSlider;
    //复制数据
    QAction *m_copyData;
    //打开外部命名
    QProcess* m_proc;

    QMenu *m_realMenu;

    //实时model
    RealLogModel *m_usnRealLogModel;
    QThread * m_usnRealLogModelThread;

    //实时监控执行
    RealMonitorModel *m_usnRealMonitorModel;
    QThread * m_usnRealMonitorModelThread;

    UsnMonitorDialog *usnMonotorDialog;
};

#endif // LOGWINDOW_H
