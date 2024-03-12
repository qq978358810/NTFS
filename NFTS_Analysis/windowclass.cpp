#include "windowclass.h"
#include "ui_windowclass.h"

#include <QTimer>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QComboBox>
#include <QHBoxLayout>
#include <QDebug>
#include <QThread>
#include <QHeaderView>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>

#include "logwindow.h"
#include "mytablemodel.h"
#include "mytableview.h"
#include "highfinddialog.h"
#include "settingdialog.h"
#include "ntfsdatarecovery.h"
WindowClass::WindowClass(QRect rect,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WindowClass)
{

    ui->setupUi(this);
    setWindowTitle(QCoreApplication::applicationName());
    setWindowIcon(QIcon(":/Images/quickRun.png"));
    setGeometry(rect);//设置窗口位置大小
    //m_elapsedT.start();

    //定时搜索//定时器--防抖函数
    m_timerSerach = new QTimer();//定时器的创建和调用必须在同一个线程
    if(!m_timerSerach->isSingleShot()){
        m_timerSerach->setSingleShot(true);//设置时间后，只触发一次，单次触发
    }
//    //定时排序//定时器--防抖函数
//    m_timerSort = new QTimer();//定时器的创建和调用必须在同一个线程
//    if(!m_timerSort->isSingleShot()){
//        m_timerSort->setSingleShot(true);//设置时间后，只触发一次，单次触发
//    }

    settingDialog = new SettingDialog(this);
    m_highFindDialog = new HighFindDialog(this);
    m_highFindDialog->setModal(true);

    //默认搜索
    m_updateViewDataStruct.RegExp = false;
    m_updateViewDataStruct.Complete = false;
    m_updateViewDataStruct.CaseInsensitive = false;
    m_updateViewDataStruct.ExtendName=DEFAULT_ALL;
    m_updateViewDataStruct.searchName ="";
    m_updateViewDataStruct.sortOrder = Qt::AscendingOrder;
    m_updateViewDataStruct.sortColumn = DAFAULT_SORT_COLUMN;
    /*
     * 添加菜单栏及动作
     */

    fileMenu = this->menuBar()->addMenu(tr("文件(&F)"));//添加文件菜单项
    action_new = new QAction(QIcon(":/Images/newWindow.png"), tr("新建窗口(&N)"), this);
    action_new->setShortcut(QKeySequence(tr("Ctrl+N")));//设置快捷键
    fileMenu->addAction(action_new);
    action_export = new QAction(QIcon(":/Images/export.png"), tr("导出(&E)"), this);
    action_export->setShortcut(QKeySequence(tr("Ctrl+S")));//设置快捷键
    fileMenu->addAction(action_export);
    action_close = new QAction(QIcon(":/Images/closeWindows.png"), tr("关闭窗口(&C)"), this);
    action_close->setShortcut(QKeySequence(tr("Ctrl+W")));//设置快捷键
    fileMenu->addAction(action_close);
    fileMenu->addSeparator();//添加分隔符
    action_exit = new QAction(QIcon(":/Images/exit.png"), tr("退出程序(&X)"), this);
    action_exit->setShortcut(QKeySequence(tr("Ctrl+Q")));//设置快捷键
    fileMenu->addAction(action_exit);

    functionMenu = this->menuBar()->addMenu(tr("日志(&L)"));
    action_usnLog = new QAction(QIcon(":/Images/usnLog_open.png"), tr("UsnJournal(&U)"), this);
    functionMenu->addAction(action_usnLog);
    action_usnLog->setCheckable(true);
    action_runLog = new QAction(QIcon(":/Images/runLog_open.png"), tr("Run日志(&R)"), this);
    functionMenu->addAction(action_runLog);
    action_runLog->setCheckable(true);

    viewMenu = this->menuBar()->addMenu(tr("视图(&V)"));
    action_sortMenu = viewMenu->addMenu(tr("排序(&S)"));
    sortColumnActionGroup = new QActionGroup(this);
    sortOrderActionGroup = new QActionGroup(this);

    searchMenu = this->menuBar()->addMenu(tr("搜索(&S)"));
    action_CaseInsensitive = new QAction(QIcon(":/Images/CaseSensitive.png"), tr("区分大小写(&C)"), this);
    action_CaseInsensitive->setShortcut(QKeySequence(tr("Ctrl+I")));//设置快捷键
    searchMenu->addAction(action_CaseInsensitive);
    action_CaseInsensitive->setCheckable(true);
    action_complete = new QAction(QIcon(":/Images/complete_dis.png"), tr("前置匹配(&M)"), this);
    action_complete->setShortcut(QKeySequence(tr("Ctrl+B")));//设置快捷键
    searchMenu->addAction(action_complete);
    action_complete->setCheckable(true);
    action_highFind = new QAction(QIcon(":/Images/highFind.png"), tr("高级搜索(&A)"), this);
    searchMenu->addAction(action_highFind);
    action_RegExp = new QAction(QIcon(":/Images/regExp_dis.png"), tr("使用正则表达式(&R)"), this);
    action_RegExp->setShortcut(QKeySequence(tr("Ctrl+R")));//设置快捷键
    searchMenu->addAction(action_RegExp);
    action_RegExp->setCheckable(true);
    action_extend = new QAction(QIcon(":/Images/extend_dis.png"), tr("扩展名(&E)"), this);
    searchMenu->addAction(action_extend);
    action_extend->setCheckable(true);



    //添加按钮
    for(int i=0;i<g_ModelHeadL.count();i++) {
        QAction* actTemp = new QAction(g_ModelHeadL.at(i), this);
        actTemp->setShortcut(QKeySequence(tr("Ctrl+"+QByteArray::number(i+1))));//设置快捷键
        if(i==9)
            actTemp->setShortcut(QKeySequence(tr("Ctrl+0")));//设置快捷键
        else if(i==10)
            actTemp->setShortcut(QKeySequence(tr("Ctrl+-")));//设置快捷键
        else if(i==11)
            actTemp->setShortcut(QKeySequence(tr("Ctrl+=")));//设置快捷键
        action_sortMenu->addAction(sortColumnActionGroup->addAction(actTemp));
        sortColumnActionGroup->actions().at(i)->setCheckable(true);
        if(i==0)
            sortColumnActionGroup->actions().at(i)->setChecked(true);
    }
    action_sortMenu->addSeparator();
    action_sortMenu->addAction(sortOrderActionGroup->addAction(new QAction(tr("升序(&A)"), this)));
    action_sortMenu->addAction(sortOrderActionGroup->addAction(new QAction(tr("降序(&D)"), this)));
    QList<QAction *> actionLTempTemp = sortOrderActionGroup->actions();
    for(int i=0;i<actionLTempTemp.count();i++){
        actionLTempTemp.at(i)->setCheckable(true);
        if(i==0)
            actionLTempTemp.at(i)->setChecked(true);
    }


    optionMenu = this->menuBar()->addMenu(tr("选项(&O)"));
    action_setting = new QAction(QIcon(":/Images/setting.png"), tr("设置(&S)"), this);
    action_setting->setShortcut(QKeySequence(tr("Ctrl+P")));//设置快捷键
    action_about = new QAction(QIcon(""), tr("关于(&A)"), this);
    action_about->setShortcut(QKeySequence(tr("Ctrl+F1")));//设置快捷键

    optionMenu->addAction(action_setting);
    optionMenu->addAction(action_about);
    /*
     * 添加工具栏及动作
     */
    ui->toolBar->addAction(action_new);
    ui->toolBar->addAction(action_export);
    ui->toolBar->addAction(action_CaseInsensitive);
    ui->toolBar->addAction(action_complete);
    ui->toolBar->addAction(action_highFind);
    ui->toolBar->addAction(action_RegExp);
    ui->toolBar->addAction(action_extend);
    ui->toolBar->addAction(action_setting);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(action_usnLog);
    ui->toolBar->addAction(action_runLog);
    /*
     * 添加界面
     */
    statuLabelAll =  new QLabel();

    ui->statusbar->addWidget(statuLabelAll);

    progressbar=new QProgressBar();
    progressbar->setFixedWidth(300);
    progressbar->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    progressbar->hide();//初始化隐藏
    ui->statusbar->addWidget(progressbar);

    // createLogUi();
    lineEdit = new InputEdit();
    lineEdit->setFixedHeight(25);
    serachButton = new QPushButton("搜索");
    serachButton->setFixedWidth(100);
    comboBox = new QComboBox();
    comboBox->setFixedWidth(130);
    comboBox->setFixedHeight(25);
    comboBox->hide();


    hBox = new QHBoxLayout();
    hBox->addWidget(lineEdit);
    hBox->addWidget(serachButton);
    hBox->addWidget(comboBox);

    tableView = new MyTableView();
    gridL = new QGridLayout();
    gridL->addLayout(hBox,0,0);
    gridL->addWidget(tableView,1,0);
    ui->centralwidget->setLayout(gridL);




    /*
     * 动作连接信号和槽
     */
    //菜单1
    connect(action_new,&QAction::triggered,this,&WindowClass::SlotNewWindow);
    connect(action_export,&QAction::triggered,this,&WindowClass::SlotExport);
    connect(action_close,&QAction::triggered,this,&WindowClass::SlotCloseWindow);
    connect(action_exit,&QAction::triggered,this,&WindowClass::SlotExitWindow);
    connect(action_usnLog,&QAction::triggered,this,&WindowClass::SlotUsnLog);
    connect(action_runLog,&QAction::triggered,this,&WindowClass::SlotRunLog);
    //菜单4
    connect(action_CaseInsensitive,&QAction::triggered,this,&WindowClass::SlotCaseInsensitive);
    connect(action_complete,&QAction::triggered,this,&WindowClass::SlotComplete);
    connect(action_highFind,&QAction::triggered,this,&WindowClass::SlotHighFind);
    connect(action_RegExp,&QAction::triggered,this,&WindowClass::SlotRegExp);
    connect(action_extend,&QAction::triggered,this,&WindowClass::SlotExtend);




    //菜单5
    connect(action_setting,&QAction::triggered,this,&WindowClass::SlotSetting);
    connect(action_about,&QAction::triggered,this,&WindowClass::SlotAbout);
    SlotRecvInitData();
    SlotUpdateWidget();//更新界面
    statuLabelAll->setText("正在初始化,请稍等 ...");

    connect(sortColumnActionGroup,&QActionGroup::triggered,this,&WindowClass::SlotActionGroup);//按钮组 排序
    connect(sortOrderActionGroup,&QActionGroup::triggered,this,&WindowClass::SlotActionGroup);//按钮组 排序 -升序或降序
    //更新界面
    connect(settingDialog,&SettingDialog::SignalUpdateWidget,this,&WindowClass::SlotUpdateWidget);
    connect(m_timerSerach,&QTimer::timeout,this,[=]()
    {
        SlotSearchData();
    });

//    connect(m_timerSort,&QTimer::timeout,this,[=]()
//    {
//        SlotSortData();
//    });

    connect(tableView->horizontalHeader(),&QHeaderView::sortIndicatorChanged,this,[=](int logicalIndex, Qt::SortOrder order)
    {
        m_logicalIndex = logicalIndex;
        m_order = order;
        SlotSortData();
//        m_timerSort->setInterval(100);//设置时间间隔超出触发timeOut信号，下次执行timer->setInterval(1000);会覆盖当前触发信号，继续等待1000；
//        m_timerSort->start();


    });//点击表头更新排序选项

    connect(m_highFindDialog,&HighFindDialog::SignalHighFind,[=](S_HIGHFIND_UPDATE_VIEW item)
    {
        QMetaObject::invokeMethod(_model, "SlotHighFindData", Qt::QueuedConnection,Q_ARG(S_HIGHFIND_UPDATE_VIEW,item));
    });//高级搜索



    connect(serachButton,&QPushButton::clicked,this,&WindowClass::SlotSearchData);
    //搜索框发生内容改变
    connect(lineEdit,&QLineEdit::textChanged,this,[=](const QString &str)
    {
        m_updateViewDataStruct.searchName = str;
        if(MemoryPool::getInstance()->getSettingArgs().real_timeSearch) {
            m_timerSerach->setInterval(200);//设置时间间隔超出触发timeOut信号，下次执行timer->setInterval(1000);会覆盖当前触发信号，继续等待；
            m_timerSerach->start();
        }

    });

    //扩展框发生内容改变
    connect(comboBox,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,[=](const QString &str)
    {
        m_updateViewDataStruct.ExtendName = str;
        if(MemoryPool::getInstance()->getSettingArgs().real_timeSearch) {
            SlotSearchData();
        }
    });
    connect(lineEdit,&InputEdit::SignalSerch,this,&WindowClass::SlotSearchData);


}

WindowClass::~WindowClass()
{
    if(_modelThread) {
        _modelThread->quit();
        _modelThread->wait();
    }
    qDebug()<<"释放UI内存.";
    delete ui;
}
//搜索按钮是否显示
void WindowClass::SlotUpdateWidget()
{
    //隐藏搜索按钮
    if(MemoryPool::getInstance()->getSettingArgs().showSearchButton)
        serachButton->hide();
    else
        serachButton->show();
    if(MemoryPool::getInstance()->getSettingArgs().windowTop) {
        //设置窗口顶置: 一直在最前面.
        Qt::WindowFlags m_flags = windowFlags();
        setWindowFlags(m_flags | Qt::WindowStaysOnTopHint);
        show();
        settingDialog->setWindowFlags(m_flags | Qt::WindowStaysOnTopHint);
        if(!settingDialog->isHidden())
            settingDialog->show();

    }  else{
        //取消设置窗口顶置: 一直在最前面.
        Qt::WindowFlags m_flags = windowFlags();
        setWindowFlags(m_flags  & ~Qt::WindowStaysOnTopHint);
        show();
        settingDialog->setWindowFlags(m_flags  & ~Qt::WindowStaysOnTopHint);
        if(!settingDialog->isHidden())
            settingDialog->show();
    }

    for(int i=0;i<MemoryPool::getInstance()->getSettingArgs().columns.count();i++)
        tableView->setColumnHidden(i,MemoryPool::getInstance()->getSettingArgs().columns.at(i));//隐藏某一列
}

//处理按钮组的动作
void WindowClass::SlotActionGroup(QAction *action)
{
    QActionGroup* tempGroup = (QActionGroup*)QObject::sender();
    QList<QAction *> actionL = tempGroup->actions();
    for(int i=0;i<actionL.count();i++)
        actionL.at(i)->setIcon(QIcon());
    if(action->isChecked()) {
        if( tempGroup == sortColumnActionGroup) {//以某列排序
            m_updateViewDataStruct.sortColumn = g_ModelHeadL.indexOf(action->text());
            tableView->horizontalHeader()->setSortIndicator(m_updateViewDataStruct.sortColumn, m_updateViewDataStruct.sortOrder);

        }else if( tempGroup == sortOrderActionGroup) {//升序或降序
            if(action->text() == "升序")
                m_updateViewDataStruct.sortOrder = Qt::AscendingOrder;
            else
                m_updateViewDataStruct.sortOrder = Qt::DescendingOrder;
            tableView->horizontalHeader()->setSortIndicator(m_updateViewDataStruct.sortColumn, m_updateViewDataStruct.sortOrder);
        }
        action->setIcon(QIcon(":/Images/darkSpot.png"));
    }else {
        action->setIcon(QIcon());
    }

}

//搜索数据
void WindowClass::SlotSearchData(bool /*isOk=true*/)
{
    QRegExp regExp(m_updateViewDataStruct.searchName,Qt::CaseInsensitive);
    if(m_updateViewDataStruct.RegExp && !regExp.isValid())
        QMessageBox::information(this,"RegExp","无效的正则表达式");
    else{
        QMetaObject::invokeMethod(_model, "SlotSearchData", Qt::QueuedConnection,Q_ARG(S_UPDATE_VIEW,m_updateViewDataStruct));
    }

}
//排序数据
void WindowClass::SlotSortData()
{
    m_updateViewDataStruct.sortOrder = m_order;
    m_updateViewDataStruct.sortColumn = m_logicalIndex;
    sortColumnActionGroup->checkedAction()->setIcon(QIcon());
    sortColumnActionGroup->actions().at(m_logicalIndex)->setChecked(true);
    sortOrderActionGroup->checkedAction()->setIcon(QIcon());
    sortOrderActionGroup->actions().at(m_updateViewDataStruct.sortOrder == Qt::AscendingOrder ? 0 : 1)->setChecked(true);
    QMetaObject::invokeMethod(_model, "SlotSortData", Qt::QueuedConnection,Q_ARG(S_UPDATE_VIEW,m_updateViewDataStruct));
}

//接收初始数据
void WindowClass::SlotRecvInitData()
{
    //创建扩展数据modle
    comboBox->setModel(MemoryPool::getInstance()->getIconModel());
    //comboBox->blockSignals(true);
    comboBox->setCurrentIndex(0);//显示全部
    //comboBox->blockSignals(false);

    //创建数据model
    _model = new MyTableModel(m_updateViewDataStruct);
    _modelThread = new QThread();
    _model->moveToThread(_modelThread);
    tableView->setModel(_model);
    connect(_modelThread,&QThread::started,_model,&MyTableModel::initData,Qt::QueuedConnection);//model初始化
    _modelThread->start();
    connect(_modelThread,&QThread::finished,_model,&MyTableModel::deleteLater,Qt::DirectConnection);//线程完成析构
    connect(_modelThread,&QThread::finished,_modelThread,&QThread::deleteLater,Qt::DirectConnection);//线程完成析构
    connect(_model,&MyTableModel::SignalStatusbarUpdata,this,&WindowClass::SlotStatusbarUpdata,Qt::QueuedConnection);//状态栏更新
    connect(CNtfsDataRecovery::getInstance(),&CNtfsDataRecovery::SignalStatusbarUpdata,this,&WindowClass::SlotStatusbarUpdata,Qt::QueuedConnection);//状态栏更新
    tableView->horizontalHeader()->setSortIndicator(DAFAULT_SORT_COLUMN,Qt::AscendingOrder);//设置默认第0列升序



}
QString WindowClass::toStr(quint64 count)
{
    QString str = QString::number(count);
    int ii = str.size() / 3 ;
    int jj = str.size() % 3 ;
    if(!jj){
        ii--;
        for(int i=0;i<ii;i++)
            str.insert((i+1)*-3 - i,',');
    }else {
        for(int i=0;i<ii;i++)
            str.insert((i+1)*-3 - i,',');
    }
    return str;
}
//状态栏更新
void WindowClass::SlotStatusbarUpdata(E_STATUSBAR_UPDATA type,QVariant var)
{
    if(!MemoryPool::getInstance()->getIsFinish())
        return;
    switch (type){
    case Status_DataCount:
    {
        S_PARTITION_DATA_COUNT countStruct = var.value<S_PARTITION_DATA_COUNT>();
        if(!progressbarIsExe)
            progressbar->hide();
        statuLabelAll->setText(QString("所有文件:%1 个 | 删除文件:%2 个 | 正常文件:%3 个 | 删除目录:%4 个 | 正常目录:%5 个 | 系统文件:%6 个").arg(toStr(countStruct._falg_0_count+countStruct._falg_1_count+countStruct._falg_2_count+countStruct._falg_3_count+countStruct._falg_other_count))
                               .arg(toStr(countStruct._falg_0_count))
                               .arg(toStr(countStruct._falg_1_count))
                               .arg(toStr(countStruct._falg_2_count))
                               .arg(toStr(countStruct._falg_3_count))
                               .arg(toStr(countStruct._falg_other_count)));

    }
        break;
    case Status_ExportDataStart://开始导出文件列表
    {
        progressbarIsExe = true;
        progressbar->show();
        progressbar->setRange(0,var.toULongLong());
    }
        break;
    case Status_ExportDataCount://导出过程
    {
        progressbar->setValue(var.toULongLong());
        double dProgress = progressbar->value() * 100.0  / progressbar->maximum() ;
        // 百分比计算公式
        progressbar->setFormat(QString("正在导出: %1%").arg(QString::number(dProgress, 'f', 3)));
    }
        break;
    case Status_ExportDataEnd://文件列表导出完成
    {
        progressbarIsExe = false;
        if(var.toULongLong() == 0)
        {
            progressbar->setFormat(QString("文件列表导出失败!"));
        }
        else
        {
            progressbar->setValue(progressbar->maximum());
            double dProgress = progressbar->value() * 100.0  / progressbar->maximum() ;
            // 百分比计算公式
            progressbar->setFormat(QString("文件列表导出完成: %1%").arg(QString::number(dProgress, 'f', 3)));
        }
    }
        break;
    case Status_Text:
    {
        statuLabelAll->setText(var.toString());
    }
        break;
    default:
        break;

    }
}

//关闭窗口
void WindowClass::closeEvent(QCloseEvent *event)
{
    //SlotStatusbarUpdata(Status_Text,"窗口即将关闭......");
    //托盘开启的情况
    if(MemoryPool::getInstance()->getSettingArgs().showSystemTray){
        //开启的窗口只有一个，忽略事件
        if(MemoryPool::getInstance()->getWindows()->count()==1) {
            event->ignore();
            this->hide();
            return;
        }
    }

    //emit SignalCloseWindow();
    QEvent _event(CLOSE_WINDOW);
    QApplication::sendEvent(this,&_event);
    event->accept();
}
//新建窗口
void WindowClass::SlotNewWindow()
{
    //    int offsetPoint = 22;//每次生成的窗口相对现在的窗口偏移距离
    //    int windowWidth = this->geometry().width();//获取窗口宽度
    //    int windowHeight = this->geometry().height();//获取窗口高度
    //    int x = this->geometry().x();//获取窗口x坐标
    //    int y = this->geometry().y();//获取窗口y坐标
    //    emit SignalNewWindow(QRect(x+offsetPoint,y+offsetPoint,windowWidth,windowHeight));
    QEvent event(NEW_WINDOW);
    QApplication::sendEvent(qApp,&event);
}
//导出
void WindowClass::SlotExport()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("导出结果列表"),
                                                    EXPORTFILE_LIST,tr("CSV文件(*.csv);;文本文件(*.txt);;XLSX文件[不建议超过10000行数据-耗时&占内存](*.xlsx)"));
    qDebug()<<g_OperateFileName<<"导出文件名:"<<fileName;
    if(!fileName.isEmpty())
        QMetaObject::invokeMethod(_model, "SlotExportFileList", Qt::QueuedConnection,Q_ARG(QString,fileName));
}
//关闭窗口
void WindowClass::SlotCloseWindow()
{
    close();
}

//退出程序
void WindowClass::SlotExitWindow()
{
    //emit SignalExitWindow();
    QEvent event(EXIT_PROGRAM);
    QApplication::sendEvent(qApp,&event);
}


//usn日志
void WindowClass::SlotUsnLog(bool checked)
{
    QAction *action =qobject_cast<QAction *>(sender());
    if(checked){
        action->setIcon(QIcon(":/Images/usnLog_close.png"));
        LogWindow::getInstance()->show();
        //是日志界面显示最前面，Qt::WindowActive 强制获取鼠标焦点
        LogWindow::getInstance()->setWindowState((LogWindow::getInstance()->windowState()&~Qt::WindowMinimized) | Qt::WindowActive);
        LogWindow::getInstance()->raise();
    } else {
        action->setIcon(QIcon(":/Images/usnLog_open.png"));
        if(LogWindow::getInstance()->isHidden())
            return;
        LogWindow::getInstance()->hide();
        //LogWindow::getInstance()->setWindowState((LogWindow::getInstance()->windowState()&~Qt::WindowMinimized) | Qt::WindowActive);
        //LogWindow::getInstance()->raise();
    }
}
//run日志
void WindowClass::SlotRunLog(bool checked)
{
    QAction *action =qobject_cast<QAction *>(sender());

    if(checked) {
        action->setIcon(QIcon(":/Images/runLog_close.png"));
        QEvent event(CREATE_CONSOLE);
        QApplication::sendEvent(qApp,&event);
    } else{
        action->setIcon(QIcon(":/Images/runLog_open.png"));
        QEvent event(CLOSE_CONSOLE);
        QApplication::sendEvent(qApp,&event);
    }
}

//区分大小写
void WindowClass::SlotCaseInsensitive(bool checked)
{
    QAction *action =qobject_cast<QAction *>(sender());

    if(checked) {
        action->setIcon(QIcon(":/Images/CaseInsensitive.png"));
        m_updateViewDataStruct.CaseInsensitive = true;

    } else {
        action->setIcon(QIcon(":/Images/CaseSensitive.png"));
        m_updateViewDataStruct.CaseInsensitive = false;

    }
    if(MemoryPool::getInstance()->getSettingArgs().real_timeSearch) {
        SlotSearchData();
    }

}

//前置匹配  -- 取消使用正则
void WindowClass::SlotComplete(bool checked)
{
    QAction *action =qobject_cast<QAction *>(sender());

    if(checked) {
        action->setIcon(QIcon(":/Images/complete.png"));
        m_updateViewDataStruct.Complete = true;

        action_RegExp->setChecked(false);
        action_RegExp->setIcon(QIcon(":/Images/regExp_dis.png"));
        m_updateViewDataStruct.RegExp = false;

    } else{
        action->setIcon(QIcon(":/Images/complete_dis.png"));
        m_updateViewDataStruct.Complete = false;
    }
    if(MemoryPool::getInstance()->getSettingArgs().real_timeSearch) {
        SlotSearchData();
    }
}

//高级搜索
void WindowClass::SlotHighFind()
{
    m_highFindDialog->show();
}
//使用正则  -- 取消前置匹配
void WindowClass::SlotRegExp(bool checked)
{
    QAction *action =qobject_cast<QAction *>(sender());

    if(checked){
        action->setIcon(QIcon(":/Images/regExp.png"));
        m_updateViewDataStruct.RegExp = true;

        action_complete->setChecked(false);
        action_complete->setIcon(QIcon(":/Images/complete_dis.png"));
        m_updateViewDataStruct.Complete = false;
    }else{
        action->setIcon(QIcon(":/Images/regExp_dis.png"));
        m_updateViewDataStruct.RegExp = false;

    }
    if(MemoryPool::getInstance()->getSettingArgs().real_timeSearch) {
        SlotSearchData();
    }

}
//扩展名
void WindowClass::SlotExtend(bool checked)
{

    QAction *action =qobject_cast<QAction *>(sender());
    if(checked) {
        action->setIcon(QIcon(":/Images/extend.png"));
        comboBox->show();
    } else{
        action->setIcon(QIcon(":/Images/extend_dis.png"));
        comboBox->hide();
    }
//    QList<int> *LL = new QList<int>;
//    int i = LL->at(1000000);
//    qDebug()<<"iiii"<<i;

}
//设置
void WindowClass::SlotSetting()
{
    settingDialog->Show();
}
//关于
void WindowClass::SlotAbout()
{
    QEvent event(ABOUT);
    QApplication::sendEvent(qApp,&event);

}

void InputEdit::keyPressEvent(QKeyEvent *ev)
{
    if(this->hasFocus() && (ev->key() == Qt::Key_Enter || ev->key() == Qt::Key_Return))
        emit SignalSerch(true);
    else
        QLineEdit::keyPressEvent(ev);
}
