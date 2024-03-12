#include "logwidget.h"

//std::atomic<LogWidget*> LogWidget::m_instance = nullptr;
QAtomicPointer<LogWidget> LogWidget::m_instance = nullptr;
QMutex  LogWidget::m_mutex;

LogWidget::LogWidget(QWidget *parent) : QWidget(parent)
{
    resize(800,500);
    m_historyFileSize = 0;
    m_proc = new QProcess(this);
    m_openHistoryFile = new QAction(tr("打开文件"),this);
    connect(m_openHistoryFile,&QAction::triggered,this,&LogWidget::SlotOpenHistoryFile);
    m_pushButton = new QPushButton(this);
    connect(m_pushButton,&QPushButton::clicked,this,&LogWidget::SlotButtonGetData);
    m_realPlainTextEdit = new QTableView(this);
    //m_realPlainTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    //connect(m_realPlainTextEdit,&QPlainTextEdit::customContextMenuRequested,this,&LogWidget::SlotShowMenu);
    m_historyPlainTextEdit = new QPlainTextEdit(this);
    m_historyPlainTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_historyPlainTextEdit,&QPlainTextEdit::customContextMenuRequested,this,&LogWidget::SlotShowMenu);
    //connect(m_historyPlainTextEdit->verticalScrollBar(),&QAbstractSlider::valueChanged,this,&LogWidget::SlotSlidervalueChanged);
    //m_realPlainTextEdit->setReadOnly(true);
    m_historyPlainTextEdit->setReadOnly(true);
    // m_historyPlainTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_logComboBox = new QComboBox(this);
    connect(m_logComboBox,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&LogWidget::SlotSwitchHistoryFile);
    m_comboBox = new QComboBox(this);
    connect(m_comboBox,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&LogWidget::SlotSwitchWidget);
    m_comboBox->addItem(QIcon(":/Images/realMonitor.png"),USNREALMONITOR);
    m_comboBox->addItem(QIcon(":/Images/historyInfo.png"),USNHISTORYINFO);

    //创建标准弹出式菜单
    //m_realMenu = m_realPlainTextEdit->createStandardContextMenu();
    m_historyMenu = m_historyPlainTextEdit->createStandardContextMenu();
    m_historyMenu->addAction(m_openHistoryFile);

    QGridLayout *gridL = new QGridLayout();//网格布局
    gridL->addWidget(m_comboBox,0,0);
    //gridL->setRowStretch(0,0);
    gridL->addWidget(m_logComboBox,0,1);
    gridL->addWidget(m_pushButton,0,2);
    gridL->addWidget(m_realPlainTextEdit,1,0,-1,-1);
    gridL->addWidget(m_historyPlainTextEdit,1,0,-1,-1);
    this->setLayout(gridL);
    this->hide();

    //创建usn日志model
    m_usnModel = new LogModel();
    m_usnThread = new QThread();
    m_usnModel->moveToThread(m_usnThread);
    m_realPlainTextEdit->setModel(m_usnModel);
    connect(m_usnThread,&QThread::started,m_usnModel,&LogModel::init,Qt::QueuedConnection);//Qt::QueuedConnection
    m_usnThread->start();
    connect(m_usnThread,&QThread::finished,m_usnModel,&LogModel::deleteLater,Qt::DirectConnection);
    connect(m_usnThread,&QThread::finished,m_usnThread,&QThread::deleteLater,Qt::DirectConnection);



    //    m_watcher = new QFutureWatcher<QString>();
    //    connect(m_watcher, &QFutureWatcher<QString>::finished,this, &LogWidget::SlotHistoryPlainTextEditAddData);
}
//LogWidget *LogWidget::getInstance()//饿汉模式
//{
//    /* 报错信息
//     * QWidget: Must construct a QApplication before a QWidget
//     * 采用了静态（static）或者全局的QWidget类，静态对象在进入main函数之前就产生了，所以该QWidget在QApplication之前就产生了，产生如图所示的错误。
//     * 解决方案：
//     * 避免在类中采用静态的或者全局的的qWidget。即单例的设计模式不适用于QWidget类及其子类。
//     */

//    return m_instance;
//}

LogWidget *LogWidget::getInstance()//懒汉模式
{
    //1:最初版本，存在cpu顺序优化，没有cpu屏障，存在bug
    //    if (m_instance == nullptr) //双重检测
    //    {
    //        static QMutexLocker mutexLocker(&m_mutex);
    //        if (m_instance == nullptr)
    //        {
    //            m_instance = new LogWidget();
    //            return m_instance;
    //        }
    //    }
    //    return m_instance;
    //

//    //2:c++标准类的原子操作
//            LogWidget *tmp = m_instance.load(std::memory_order_seq_cst);
//            if (!tmp) {
//                QMutexLocker mutexLocker(&m_mutex);
//                tmp = m_instance.load(std::memory_order_seq_cst);
//                if (!tmp) {
//                    tmp = new LogWidget();
//                    m_instance.store(tmp, std::memory_order_seq_cst);
//                }
//            }
//            return (LogWidget*)m_instance;

    //3:qt类原子操作
#ifndef Q_ATOMIC_POINTER_TEST_AND_SET_IS_ALWAYS_NATIVE
    if(!QAtomicPointer::isTestAndSetNative())//运行时检测
        qDebug() << "Error: TestAndSetNative not supported!";
#endif

    //使用双重检测。

    /*! testAndSetOrders操作保证在原子操作前和后的的内存访问
         * 不会被重新排序。
         */
    if(m_instance.testAndSetOrdered(nullptr, nullptr))//第一次检测
    {
        QMutexLocker locker(&m_mutex);//加互斥锁。
        m_instance.testAndSetOrdered(nullptr, new LogWidget);//第二次检测。
    }
    return m_instance;

}

void LogWidget::closeEvent(QCloseEvent *event)
{
    event->accept();
    this->hide();
}
//加载log文件数据
void LogWidget::loadHistoryFileData(QString fileName)
{

    m_pushButton->setText(QString("读取中..."));
    m_pushButton->setEnabled(false);
    QByteArray data="";
    QFile file(USN_RECORD_PATH+fileName);
    if(file.open(QIODevice::ReadOnly))
    {
        if(file.seek(m_historyFileSize) && m_historyFileSize!=file.size())
        {
            if(file.size()-m_historyFileSize<1000000)//剩余文件大小小于，直接读完
            {
                data = file.read(file.size()-m_historyFileSize);
                m_historyFileSize+=data.size();
            }
            else//剩余文件每次读取10000行数据
            {
                for(int i=0;i<10000;i++)
                    data += file.readLine();
                m_historyFileSize+=data.size();
            }
            data.remove(data.size()-2,2);
        }
    }
    else
    {
        qDebug()<<"LogWidget文件打开异常:"<<file.error();
    }

    QElapsedTimer t;
    t.start();
    //m_historyPlainTextEdit->setUndoRedoEnabled(false);
    if(!data.isEmpty())
    {

        m_historyPlainTextEdit->appendPlainText(data);
        //        m_historyPlainTextEdit->textCursor().movePosition(QTextCursor::StartOfLine,QTextCursor::KeepAnchor);
        //        m_historyPlainTextEdit->moveCursor(QTextCursor::Down);
        //        m_historyPlainTextEdit->textCursor().deletePreviousChar();
        m_pushButton->setEnabled(true);
        m_pushButton->setText(QString("继续"));
    }
    else
    {
        m_pushButton->setEnabled(false);
        m_pushButton->setText(QString("读取完成"));
    }
    qDebug()<<"SlotHistoryPlainTextEditAddData 耗时:"<<t.elapsed();
}

//切换历史文件
void LogWidget::SlotSwitchHistoryFile(QString fileName)
{
    m_historyFileSize = 0;
    m_historyFileName = "";
    m_historyPlainTextEdit->clear();
    m_historyFileName = fileName;
    //loadHistoryFileData(m_historyFileName);
    SlotButtonGetData();

}
//切换界面
void LogWidget::SlotSwitchWidget(QString name)
{
    if(name == USNREALMONITOR)//实时监控
    {
        m_historyPlainTextEdit->clear();
        m_historyPlainTextEdit->hide();
        m_logComboBox->hide();
        m_pushButton->hide();

        m_realPlainTextEdit->show();

    }
    else if(name == USNHISTORYINFO)//历史记录
    {
        m_realPlainTextEdit->hide();
        m_historyPlainTextEdit->show();
        m_logComboBox->show();
        m_pushButton->show();
        setComboBoxData();

    }
}

//设置组合框数据
void LogWidget::setComboBoxData()
{
    m_logComboBox->clear();
    m_logComboBox->addItems(getAllLogFile());
}

//获取指定文件夹下的所有日志文件
QStringList LogWidget::getAllLogFile()
{
    QStringList fileList;
    // 获取所有文件夹名
    QDir dir(USN_RECORD_PATH);
    //m_path = dir.fromNativeSeparators(USNRECORDPATH);//  "\\"转为"/"
    if (!dir.exists())
        fileList = QStringList("");
    fileList = dir.entryList(QStringList()<<QString("*.csv"),QDir::Files,QDir::Name);
    return fileList;
}
//添加数据到实时界面
void LogWidget::SlotRealPlainTextEditAddData(E_LOG_TYPE,QVariant data)
{
    QMetaObject::invokeMethod(m_usnModel,"SlotAddData",Qt::QueuedConnection,Q_ARG(S_USN_LOG,data.value<S_USN_LOG>()));
}

//显示右键菜单
void LogWidget::SlotShowMenu(const QPoint &pos)
{

    if((QPlainTextEdit*)QObject::sender() == m_historyPlainTextEdit)
    {
        //显示菜单
        m_historyMenu->exec(QCursor::pos());
    }

}
//根据滑块值更新历史记录界面
void LogWidget::SlotButtonGetData(bool checked)
{
    QMutexLocker _Lock(&m_mutexSlider);
    loadHistoryFileData(m_historyFileName);

}
//打开历史文件
void LogWidget::SlotOpenHistoryFile()
{

    QString arg = QDir::fromNativeSeparators(USN_RECORD_PATH+m_historyFileName);//  "\\"转为"/"
    //m_proc->start("notepad.exe",QStringList(arg));
    QDesktopServices::openUrl(QUrl::fromLocalFile(arg));//根据系统默认的程序打开文件
    //QDesktopServices::openUrl(QUrl("https://blog.csdn.net/weixin_43435307/article/details/117128503"));//根据系统默认的浏览器打开网页

}




//------------------------------------------------------
//-----------------LogModel-----------------------------
//------------------------------------------------------



LogModel::LogModel(QObject *parent) : QAbstractTableModel(parent)
{
    m_addRow = 1;
}

void LogModel::init()
{
    timer = new QTimer();//定时器的创建和调用必须在同一个线程
    if(!timer->isSingleShot())
    {
        timer->setSingleShot(true);//设置时间后，只触发一次，单次触发
    }
    connect(timer,&QTimer::timeout,this,[&]()mutable
    {
        beData();
    });

}
int LogModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    } else {
        return qMin(m_datas.count(),LOGMAXCOUNT);
    }
}
int LogModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    } else {
        return g_UsnLogTitle.count();   //表头的大小
    }
}
QVariant LogModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    const int row=index.row();
    if(role == Qt::DisplayRole )//显示数据
    {
        switch(index.column())
        {
        case 0: return m_datas.at(row).dataTime;
        case 1: return m_datas.at(row).partition;
        case 2: return m_datas.at(row).usnOffset;
        case 3: return m_datas.at(row).mft;
        case 4: return m_datas.at(row).pMft;
        case 5: return m_datas.at(row).usn;
        case 6: return m_datas.at(row).reasonDataTime;
        case 7: return m_datas.at(row).reason;
        case 8: return m_datas.at(row).attributes;
        case 9: return m_datas.at(row).fileName;
        }
    }
    if (role == Qt::TextAlignmentRole) {    //对其方式
        return Qt::AlignCenter; //对齐格式为居中
    }
    return QVariant();

}
QVariant LogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
    {
        //这里我们只设置居中对齐和文本
        if (role == Qt::DisplayRole)
        {
            //这里把横项列表头的文本设计为可以设置的
            if(section>=0 && section<g_UsnLogTitle.count())
                return g_UsnLogTitle.at(section);
            return QString("Col %1").arg(section + 1);
        }
        else if(role == Qt::TextAlignmentRole)
        {
            return Qt::AlignCenter;
        }
    }
    else
    {
        if (role == Qt::DisplayRole)
            return QString("%1").arg(section + m_addRow);
        else if(role == Qt::TextAlignmentRole)
            return Qt::AlignCenter;
    }
    return QVariant();
}
//接收数据
void LogModel::SlotAddData(S_USN_LOG item)
{
    m_tempDatas.append(item);
    //同时接收大量数据超过设定值，更新界面
    if(m_tempDatas.count() >= ENDCOUNT)
    {
        beData();
        return;
    }
    timer->setInterval(100);//设置时间间隔超出触发timeOut信号，下次执行timer->setInterval(1000);会覆盖当前触发信号，继续等待1000；
    timer->start();



}
//更新view
void LogModel::beData()
{
    QModelIndexList from;
    QModelIndexList to;
    emit layoutAboutToBeChanged();
    m_datas.append(m_tempDatas);
    m_tempDatas.clear();
    if(m_datas.count()>LOGMAXCOUNT)
    {
        quint32 outCount = m_datas.count()-LOGMAXCOUNT;
        m_datas.erase(m_datas.begin(),m_datas.begin()+outCount);
        m_addRow += outCount;
    }
    changePersistentIndexList(from,to);
    emit layoutChanged();
}
