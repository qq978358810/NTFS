#ifndef WINDOWCLASS_H
#define WINDOWCLASS_H

#include <QLineEdit>
#include <QMainWindow>
#include "mytablemodel.h"
#include "IncludeFile.h"
QT_BEGIN_NAMESPACE
class HighFindDialog;
class InputEdit;
class QPushButton;
class QComboBox;
class MyTableView;
class QListView;
class QLabel;
class QProgressBar;
class QActionGroup;
class QThread;
class QTimer;
class SettingDialog;
class QHBoxLayout;
class QGridLayout;
QT_END_NAMESPACE


//搜索框
class InputEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit InputEdit(QWidget *parent = nullptr){};

    void keyPressEvent(QKeyEvent *ev);
signals:
    void SignalSerch(bool);//执行搜索
};

namespace Ui {
class WindowClass;
}

class WindowClass : public QMainWindow
{
    Q_OBJECT

public:
    explicit WindowClass(QRect rect,QWidget *parent = nullptr);
    ~WindowClass();

    void closeEvent(QCloseEvent *event);
    //void showEvent(QShowEvent *event);
    QString toStr(quint64);

    MyTableModel* getModel()
    {
        return _model;
    }

    void updateViewData();//更新界面数据
    QPushButton* getSearchButton()
    {
        return serachButton;
    }

signals:
    void SignalNewWindow(QRect);//新建窗口
    void SignalCloseWindow();//关闭窗口
    void SignalExitWindow();//退出程序

    void SignalDataChangeBegin(quint32);
    void SignalDataChangeEnd();
public slots:
    void SlotNewWindow();//新建窗口
    void SlotExport();//导出
    void SlotCloseWindow();//关闭窗口
    void SlotExitWindow();//退出程序
    void SlotUsnLog(bool checked);//usn日志
    void SlotRunLog(bool checked);//run日志

    void SlotCaseInsensitive(bool checked);//区分大小写
    void SlotComplete(bool checked);//完全匹配
    void SlotHighFind();//高级搜索
    void SlotRegExp(bool checked);//使用正则
    void SlotExtend(bool checked);//扩展名

    void SlotSetting();//设置
    void SlotAbout();//关于

    void SlotActionGroup(QAction *action);//处理按钮组的动作
    void SlotRecvInitData();//接收初始化数据  -- 当界面打开之前数据处理类未处理好，后面利用此槽接收一次
    void SlotStatusbarUpdata(E_STATUSBAR_UPDATA,QVariant);//状态栏更新
    void SlotSearchData(bool isOk=true);//搜索数据
    void SlotSortData();//排序数据

    void SlotUpdateWidget();//更新界面

private:
    Ui::WindowClass *ui;

    HighFindDialog *m_highFindDialog;
    InputEdit * lineEdit ;
    QPushButton *serachButton;
    QComboBox * comboBox ;
    MyTableView *tableView ;
    QListView * listView;
    QLabel * statuLabelAll;

    QProgressBar *progressbar;
    QActionGroup *sortColumnActionGroup;//排序
    QActionGroup *sortOrderActionGroup;//升序或降序
    S_UPDATE_VIEW m_updateViewDataStruct;
    bool progressbarIsExe = false;//进度条进行中 为TRUE显示


    QThread * _modelThread = NULL;//避免野指针，析构时用做判断条件
    MyTableModel* _model;
    //QElapsedTimer m_elapsedT;
    SettingDialog *settingDialog;

    QTimer* m_timerSerach;//搜索太快，采用定时搜索
    //QTimer* m_timerSort;//排序太快，采用定时排序
    int m_logicalIndex;
    Qt::SortOrder m_order;




    QMenu *fileMenu;
    QAction *action_new;
    QAction *action_export;
    QAction *action_close ;
    QAction *action_exit ;

    QMenu *functionMenu;
    QAction *action_usnLog ;
    QAction *action_runLog ;

    QMenu *viewMenu;
    QMenu *action_sortMenu;

    QMenu *searchMenu;
    QAction *action_CaseInsensitive ;

    QAction *action_complete;
    QAction *action_highFind;
    QAction *action_RegExp;
    QAction *action_extend;

    QMenu *optionMenu;
    QAction *action_setting;
    QAction *action_about;

    QHBoxLayout *hBox;
    QGridLayout *gridL ;
};

#endif // WINDOWCLASS_H
