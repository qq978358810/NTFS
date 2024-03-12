#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QRegExp>
#include <QFileDialog>
#include <QTableView>
#include <QListView>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QKeySequence>
#include <QScrollBar>
#include <QProgressBar>
#include <QPlainTextEdit>
#include <QCompleter>//自动补全 类
#include <QFileSystemModel>
#include <QMenu>
#include <QAction>
#include <QPushButton>
#include <IncludeFile.h>
#define USNREALMONITOR QString("USN日志实时监控")
#define USNHISTORYINFO QString("USN日志历史记录")
#define LOGMAXCOUNT 10000
//日志model
class LogModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit LogModel(QObject *parent = nullptr);

    void init();
    void beData();//更新view
    int rowCount(const QModelIndex& parent) const;  //返回行数
    int columnCount(const QModelIndex& parent) const;   //返回列数
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;  //返回指定表头数据


public slots:
    void SlotAddData(S_USN_LOG);//接收数据

private:
    QList<S_USN_LOG> m_datas;   //显示数据
    QList<S_USN_LOG> m_tempDatas;//更新队列
    quint32 m_addRow;   //显示的行数

    QTimer* timer;
    QElapsedTimer elapsedTimer;//保存经过时间

};
class LogWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LogWidget(QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *event);
    QStringList getAllLogFile();//获取指定文件夹下的所有日志文件
    void setComboBoxData();//设置组合框数据
    static LogWidget * getInstance();//创建单例
    void loadHistoryFileData(QString);//加载log文件数据
signals:
public slots:
    void SlotSwitchHistoryFile(QString);//切换历史文件
    void SlotSwitchWidget(QString);//切换界面
    void SlotRealPlainTextEditAddData(E_LOG_TYPE,QVariant);//添加数据到实时界面
    //void SlotHistoryPlainTextEditAddData();//添加数据到历史界面
    void SlotShowMenu(const QPoint &pos);//显示右键菜单
    //void SlotSlidervalueChanged(const int &value);//根据滑块值更新历史记录界面
    void SlotButtonGetData(bool checked=false);//按钮获取历史数据
    void SlotOpenHistoryFile();//打开历史文件

private:
    //LogWidget() {};//禁用构造函数
    virtual ~LogWidget() {};//禁用析构函数
    LogWidget(const LogWidget &);//禁止拷贝构造函数。
    LogWidget & operator=(const LogWidget &);//禁止赋值拷贝函数。

    //static std::atomic<LogWidget*> m_instance;//单例对象
    static QAtomicPointer<LogWidget> m_instance;//单例对象
    static QMutex m_mutex;//互斥锁
    QString m_path;//历史文件路径
    //QPlainTextEdit *m_realPlainTextEdit;//实时界面
    QTableView * m_realPlainTextEdit;
    QPlainTextEdit *m_historyPlainTextEdit;//历史界面
    QComboBox * m_comboBox ;//保存实时和历史
    QComboBox * m_logComboBox ;//保存日志文件
    QPushButton * m_pushButton;//加载按钮

    //QFutureWatcher<QString>* m_watcher;
    //QByteArray m_data;//保存显示的历史数据
    qint64 m_historyFileSize;//历史文件大小
    QString m_historyFileName;//历史文件名
    QMutex m_mutexSlider;
    QAction *m_openHistoryFile;//打开历史文件
    QProcess* m_proc;//打开外部命名
    QMenu *m_realMenu;
    QMenu *m_historyMenu;

    LogModel *m_usnModel;
    QThread * m_usnThread;

    QList<S_DATA_BODY*> m_monitoringDatas;            //监控列表

};

#endif // LOGWIDGET_H
