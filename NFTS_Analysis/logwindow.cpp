#include <QProcess>
#include <QMenu>
#include <QAction>
#include <QThread>
#include <QTextCodec>
#include <QDebug>
#include <QDesktopServices>
#include <QTimer>
#include <QClipboard>
#include <QCloseEvent>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include "logwindow.h"
#include "ui_logwindow.h"
#include "readwritexml.h"
#include "usnmonitordialog.h"
//std::atomic<LogWindow*> LogWindow::m_instance = nullptr;
QAtomicPointer<LogWindow> LogWindow::m_instance = nullptr;
QMutex  LogWindow::m_mutex;
LogWindow::LogWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LogWindow)
{
    ui->setupUi(this);
    setWindowTitle("UsnJournal");
    setWindowIcon(QIcon(":/Images/quickRun.png"));
    resize(800,500);
    m_statuLabe =  new QLabel();
    ui->statusbar->addWidget(m_statuLabe);

    usnMonotorDialog = new UsnMonitorDialog;
    usnMonotorDialog->hide();
    m_historyFileSize = 0;
    m_proc = new QProcess(this);
    m_realMenu = new QMenu(this);
    m_copyData = new QAction(tr("复制"),this);
    connect(m_copyData,&QAction::triggered,this,&LogWindow::SlotCopyData);
    m_realMenu->addAction(m_copyData);


    ui->m_realTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->m_realTableView,&QTableView::customContextMenuRequested,this,&LogWindow::SlotShowMenu);
    //ui->m_historyTableView->setReadOnly(true);
    // ui->m_historyPlainTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);//设置不自动换行


    //创建usn实时日志model
    m_usnRealLogModel = new RealLogModel(QString("realUsnDatas"));
    m_usnRealLogModelThread = new QThread();
    m_usnRealLogModel->moveToThread(m_usnRealLogModelThread);
    ui->m_realTableView->setModel(m_usnRealLogModel);
    connect(m_usnRealLogModelThread,&QThread::started,m_usnRealLogModel,&RealLogModel::init,Qt::QueuedConnection);//Qt::QueuedConnection
    m_usnRealLogModelThread->start();
    connect(m_usnRealLogModelThread,&QThread::finished,m_usnRealLogModel,&RealLogModel::deleteLater,Qt::DirectConnection);
    connect(m_usnRealLogModelThread,&QThread::finished,m_usnRealLogModelThread,&QThread::deleteLater,Qt::DirectConnection);
    connect(m_usnRealLogModel,&RealLogModel::SignalStatusbarUpdata,this,&LogWindow::SlotStatusbarUpdata,Qt::QueuedConnection);

    //实时监控执行
    m_usnRealMonitorModel = new RealMonitorModel();
    m_usnRealMonitorModelThread = new QThread();
    m_usnRealMonitorModel->moveToThread(m_usnRealMonitorModelThread);
    ui->monitorTableView->setModel(m_usnRealMonitorModel);
    connect(m_usnRealMonitorModelThread,&QThread::started,m_usnRealMonitorModel,&RealMonitorModel::init,Qt::QueuedConnection);//Qt::QueuedConnection
    m_usnRealMonitorModelThread->start();
    connect(m_usnRealMonitorModelThread,&QThread::finished,m_usnRealMonitorModel,&RealMonitorModel::deleteLater,Qt::DirectConnection);
    connect(m_usnRealMonitorModelThread,&QThread::finished,m_usnRealMonitorModelThread,&QThread::deleteLater,Qt::DirectConnection);

    quint64 elapsed;
    QMetaObject::invokeMethod(m_usnRealMonitorModel,"SlotGetElapsed",Qt::DirectConnection,Q_RETURN_ARG(quint64,elapsed));
    ui->doubleSpinBox->setValue((double)elapsed / 1000.0);

}

LogWindow::~LogWindow()
{
    delete ui;
}

//状态栏更新
void LogWindow::SlotStatusbarUpdata(E_STATUSBAR_UPDATA type,QVariant var)
{
    switch (type){

    case Status_Text:
    {

        m_statuLabe->setText(var.toString());
    }
        break;
    default:
        break;

    }
}
LogWindow *LogWindow::getInstance()//懒汉模式
{
    //1:最初版本，存在cpu顺序优化，没有cpu屏障，存在bug
    //    if (m_instance == nullptr) //双重检测
    //    {
    //        static QMutexLocker mutexLocker(&m_mutex);
    //        if (m_instance == nullptr)
    //        {
    //            m_instance = new LogWindow();
    //            return m_instance;
    //        }
    //    }
    //    return m_instance;
    //

    //    //2:c++标准类的原子操作
    //            LogWindow *tmp = m_instance.load(std::memory_order_seq_cst);
    //            if (!tmp) {
    //                QMutexLocker mutexLocker(&m_mutex);
    //                tmp = m_instance.load(std::memory_order_seq_cst);
    //                if (!tmp) {
    //                    tmp = new LogWindow();
    //                    m_instance.store(tmp, std::memory_order_seq_cst);
    //                }
    //            }
    //            return (LogWindow*)m_instance;

    //3:qt类原子操作
#ifndef Q_ATOMIC_POINTER_TEST_AND_SET_IS_ALWAYS_NATIVE
    if(!QAtomicPointer::isTestAndSetNative())//运行时检测
        qDebug() << "Error: TestAndSetNative not supported!";
#endif

    //使用双重检测。

    /*! testAndSetOrders操作保证在原子操作前和后的的内存访问
         * 不会被重新排序。
         */
    //第一次检测
    if(m_instance.testAndSetOrdered(nullptr, nullptr)){
        QMutexLocker locker(&m_mutex);//加互斥锁。
        m_instance.testAndSetOrdered(nullptr, new LogWindow);//第二次检测。
    }
    return m_instance;

}

void LogWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
    this->hide();
}


//添加数据到实时界面
void LogWindow::SlotRealPlainTextEditAddData(E_LOG_TYPE,QVariant data)
{
    QMetaObject::invokeMethod(m_usnRealLogModel,"SlotAddData",Qt::QueuedConnection,Q_ARG(S_USN_LOG,data.value<S_USN_LOG>()));
    QMetaObject::invokeMethod(m_usnRealMonitorModel,"SlotReceiveUsnData",Qt::QueuedConnection,Q_ARG(S_USN_LOG,data.value<S_USN_LOG>()));
}

//显示右键菜单
void LogWindow::SlotShowMenu(const QPoint &pos)
{

    if((QTableView*)QObject::sender() == ui->m_realTableView)
        //显示菜单
        m_realMenu->exec(QCursor::pos());

}

//复制数据
void LogWindow::SlotCopyData()
{
    if(ui->m_realTableView->isVisible()){
        QModelIndexList indexL = ui->m_realTableView->selectionModel()->selectedIndexes();
        QString str;
        qint32 row=0;
        if(indexL.isEmpty())
            return;
        else
            row = indexL.at(0).row();
        for(auto &i :indexL){
            if(i.row()!=row)
                str += "\r\n"+i.data().toString()+",";
            else
                str += i.data().toString()+",";

            row = i.row();
        }
        str.chop(1);
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText(str);
    }

}

//清空实时数据
void LogWindow::on_clearButton_clicked()
{
    QMetaObject::invokeMethod(m_usnRealLogModel,"SlotClear",Qt::QueuedConnection);
}

//导出实时数据
void LogWindow::on_exportButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("导出"),
                                                    EXPORTFILE_LIST,tr("CSV文件(*.csv);;文本文件(*.txt);;XLSX文件[不建议超过10000行数据-耗时&占内存](*.xlsx)"));
    qDebug()<<g_OperateFileName<<"导出文件名:"<<fileName;
    if(!fileName.isEmpty()) {
        QMetaObject::invokeMethod(m_usnRealLogModel, "SlotExportData", Qt::QueuedConnection,Q_ARG(QString,fileName));
    }
}

//追加
void LogWindow::on_appendButton_clicked()
{
    usnMonotorDialog->exec();
    if(usnMonotorDialog->isOk()) {
        S_MONITOR_ITEM args = usnMonotorDialog->getArgs();
        QMetaObject::invokeMethod(m_usnRealMonitorModel,"SlotChangeData",Qt::DirectConnection,Q_ARG(E_CHANGE,AppendData),Q_ARG(S_MONITOR_ITEM,args),Q_ARG(int,-1));
    }
    usnMonotorDialog->clearArgs();
}
//插入
void LogWindow::on_insertButton_clicked()
{
    QModelIndex index = ui->monitorTableView->selectionModel()->currentIndex();
    if(!index.isValid()) {
        return;
    }
    int row = index.row();
    usnMonotorDialog->exec();
    if(usnMonotorDialog->isOk()) {
        S_MONITOR_ITEM args = usnMonotorDialog->getArgs();
        QMetaObject::invokeMethod(m_usnRealMonitorModel,"SlotChangeData",Qt::DirectConnection,Q_ARG(E_CHANGE,InsertData),Q_ARG(S_MONITOR_ITEM,args),Q_ARG(int,row));
    }
    usnMonotorDialog->clearArgs();
}

//删除
void LogWindow::on_deleteButton_clicked()
{
    QModelIndex index = ui->monitorTableView->selectionModel()->currentIndex();
    if(!index.isValid()) {
        return;
    }
    int row = index.row();
    QMessageBox::StandardButton button = QMessageBox::information(this,"删除",QString("正在删除 %1 行!").arg(row),QMessageBox::Ok,QMessageBox::Cancel);
    if(QMessageBox::Ok == button) {
        QMetaObject::invokeMethod(m_usnRealMonitorModel,"SlotChangeData",Qt::DirectConnection,Q_ARG(E_CHANGE,DeleteData),Q_ARG(S_MONITOR_ITEM,S_MONITOR_ITEM()),Q_ARG(int,row));
    }
}
//修改
void LogWindow::on_updateButton_clicked()
{
    QModelIndex index = ui->monitorTableView->selectionModel()->currentIndex();
    if(!index.isValid()) {
        return;
    }
    int row = index.row();

    S_MONITOR_ITEM item;
    QMetaObject::invokeMethod(m_usnRealMonitorModel,"SlotGetRowData",Qt::DirectConnection,Q_RETURN_ARG(S_MONITOR_ITEM, item),Q_ARG(int,row));

    usnMonotorDialog->setArgs(item);
    usnMonotorDialog->exec();
    if(usnMonotorDialog->isOk()) {
        S_MONITOR_ITEM args = usnMonotorDialog->getArgs();
        QMetaObject::invokeMethod(m_usnRealMonitorModel,"SlotChangeData",Qt::DirectConnection,Q_ARG(E_CHANGE,UpdateData),Q_ARG(S_MONITOR_ITEM,args),Q_ARG(int,row));
    }
    usnMonotorDialog->clearArgs();
}

//上移
void LogWindow::on_upButton_clicked()
{
    QModelIndex index = ui->monitorTableView->selectionModel()->currentIndex();
    if(!index.isValid()) {
        return;
    }
    int row = index.row();
    QMetaObject::invokeMethod(m_usnRealMonitorModel,"SlotChangeData",Qt::DirectConnection,Q_ARG(E_CHANGE,UpData),Q_ARG(S_MONITOR_ITEM,S_MONITOR_ITEM()),Q_ARG(int,row));

}
//下移
void LogWindow::on_downButton_clicked()
{
    QModelIndex index = ui->monitorTableView->selectionModel()->currentIndex();
    if(!index.isValid()) {
        return;
    }
    int row = index.row();
    QMetaObject::invokeMethod(m_usnRealMonitorModel,"SlotChangeData",Qt::DirectConnection,Q_ARG(E_CHANGE,DownData),Q_ARG(S_MONITOR_ITEM,S_MONITOR_ITEM()),Q_ARG(int,row));

}


void LogWindow::on_doubleSpinBox_valueChanged(double arg1)
{
    QMetaObject::invokeMethod(m_usnRealMonitorModel,"SlotReceiveInterval",Qt::DirectConnection,Q_ARG(quint64,arg1*1000));

}
