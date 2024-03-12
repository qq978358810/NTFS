#include "mainwindow.h"

#include <QDesktopWidget>
#include <QTextStream>
#include <QMessageBox>
#include <QThread>
#include <QtDebug>
#include <QRandomGenerator>
#include "windowclass.h"
#include "datamanage.h"
#include "logwindow.h"
static QTextStream cout(stdout, QIODevice::WriteOnly);//重定向输出
QtMessageHandler gDefaultHandler = NULL;
bool gRedirection = false;//重定向到stdout

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    //没有内容则返回
    if (msg.isEmpty()) {
        return;
    }

    QString logType;
    switch(type){
    case QtDebugMsg:
        logType = QString("DEBUG");
        break;

    case QtInfoMsg:
        logType = QString("INFO");
        break;

    case QtWarningMsg:
        logType = QString("WARNING");
        break;

    case QtCriticalMsg:
        logType = QString("CRITICAL");
        break;

    case QtFatalMsg:
        logType = QString("FATAL");
        break;

    default:
        logType = QString("UNKNOWN");
        break;
    }

    static QMutex mutex;	//添加互斥锁，便于在多线程情况的使用
    static QDir dir;
    QMutexLocker locker(&mutex);		//加锁
    QStringList sL = msg.split(SPLIT_SYMBOLS,QString::SkipEmptyParts);

    if(sL.count() == 2){
        QString message = QString("%1 --- %2 --- Function: %3 --- Line: %4 --- Message: %5").arg(DATETIMS).arg(logType).arg(context.function).arg(context.line).arg(sL.at(1));

        QString LogName = sL.at(0) +".log";
        LogName.remove("\"");
        QFile file(RUN_LOG_PATH+DATE+"___"+LogName);
        if(file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream text_stream(&file);
            text_stream << message<<endl;//将任何缓存数据刷新到文件中
            file.close();
        }


    } else{
        QString message = QString("%1 --- %2 --- Function: %3 --- Line: %4 --- Message: %5").arg(DATETIMS).arg(logType).arg(context.function).arg(context.line).arg(msg);

        QString LogName = "other.log";
        QFile file(RUN_LOG_PATH+DATE+"___"+LogName);
        if(file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            QTextStream text_stream(&file);
            text_stream << message<<endl;//将任何缓存数据刷新到文件中
            file.close();
        }

    }
    if(gRedirection)
        cout<<msg<<endl;
    //    //用系统原来的函数完成原来的功能. 比如输出到调试窗
    //    if(gDefaultHandler)
    //    {
    //        gDefaultHandler( type,context,msg);
    //    }


}
//重启程序
void restartProgram()
{
    HANDLE current_process= GetCurrentProcess();//
    wchar_t filePath[MAX_PATH];
    DWORD size=MAX_PATH;
    QueryFullProcessImageName(current_process,0,filePath,&size);//
    STARTUPINFO startupInfo;
    startupInfo.cb = sizeof(startupInfo);
    PROCESS_INFORMATION processInformation;
    CreateProcess(
                filePath,
                NULL,
                NULL,
                NULL,
                FALSE,
                NORMAL_PRIORITY_CLASS,
                NULL,
                NULL,
                &startupInfo,
                &processInformation

                );
}
BOOL WINAPI console_ctrl_handler(DWORD dwCtrlType){



    switch (dwCtrlType){

    case CTRL_C_EVENT: // Ctrl+C

        break;

    case CTRL_CLOSE_EVENT: // Closing the console window //
    {
        // 遇到关闭事件  重新启动当前重新

        system("cls");//
        restartProgram();
        QMessageBox::warning(NULL,"异常","重新启动当前进程!");
    }
        break; //

    case CTRL_BREAK_EVENT: // Ctrl+Break
        break;

    case CTRL_LOGOFF_EVENT: // User logs off. Passed only to services!
        break;

    case CTRL_SHUTDOWN_EVENT: // System is shutting down. Passed only to services!
        break;
    }//switch

    // Return TRUE if handled this message, further handler functions won't be called.
    // Return FALSE to pass this message to further handlers until default handler calls ExitProcess().
    return TRUE;

}

// 编写回调函数格式 ---
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType){
    // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
    {
        Beep(600, 200);
        return TRUE;
    }
        // CTRL-CLOSE: confirm that the user wants to exit.
    case CTRL_CLOSE_EVENT:
    {
        for(auto  i: *MemoryPool::getInstance()->getWindows())
            i->close();
        Beep(600, 200);
        return TRUE;
    }

        // Pass other signals to the next handler.
    case CTRL_BREAK_EVENT:
        Beep(900, 200);
        printf("Ctrl-Break event\n\n");
        return TRUE;

    case CTRL_LOGOFF_EVENT:
        Beep(1000, 200);
        printf("Ctrl-Logoff event\n\n");
        return TRUE;

    case CTRL_SHUTDOWN_EVENT:
        Beep(750, 500);
        printf("Ctrl-Shutdown event\n\n");
        return TRUE;

    default:
        return TRUE;
    }
}
//创建控制台
void CreateConsole()
{
    ::AllocConsole();// 创建控制台
    Sleep(500);
    SetConsoleTitle(L"RunLog...");//设置控制台标题
    FILE * file;
    freopen_s(&file,"CONOUT$","w+t",stdout);//重定向到控制台打印日志

    BOOL  fSuccess = ::SetConsoleCtrlHandler((PHANDLER_ROUTINE)console_ctrl_handler, TRUE);//    // 第二个参数为TRUE则添加，否则删除,关联控制台回调函数事件
    if (!fSuccess)
        qDebug() << "无法设置控制台处理程序(回调函数)!";

    //禁用控制台关闭按钮
    HWND hwnd = ::GetConsoleWindow();
    if (hwnd != NULL){
        HMENU hMenu = ::GetSystemMenu(hwnd, FALSE);
        if (hMenu != NULL)
            DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
    }
}
//关闭控制台
void CloseConsole()
{
    // fclose(stdout);
    ::FreeConsole();
    Sleep(500);
    FILE * file;
    freopen_s(&file,"___redirectionFile.txt","w",stdout);//重定向到文件 关闭窗口

}
MainWindow::MainWindow(int &argc, char **argv):QApplication(argc,argv)
{
    //QCoreApplication::setApplicationName("QuickRun");
    //在其它地方调用qInstallMessageHandler设置新的输出函数，但保存原来的函数
    gDefaultHandler = qInstallMessageHandler(myMessageHandler);
    //加载配置文件
    MemoryPool::getInstance()->loadSettingArgs();
    QDir dir;
    if(!dir.mkpath(EXPORTFILE_LIST))
        QMessageBox::warning( NULL,"警告","创建FileList文件夹失败!");
    if(!dir.mkpath(LOG_PATH))
        QMessageBox::warning( NULL,"警告","创建LOGS文件夹失败!\n无法写入日志!");
    if(!dir.mkpath(USN_RECORD_PATH))
        QMessageBox::warning( NULL,"警告","创建UsnRecord文件夹失败!\n无法写入UsnRecord日志!");
    if(!dir.mkpath(RUN_LOG_PATH))
        QMessageBox::warning( NULL,"警告","创建RunLog文件夹失败!\n无法写入RunLog日志!");

    //创建数据管理类 -- 逻辑
    m_dataManage = new DataManage();
    m_dataManageThread = new QThread();
    m_dataManage->moveToThread(m_dataManageThread);
    connect(m_dataManageThread,&QThread::started,m_dataManage,&DataManage::SlotInit,Qt::QueuedConnection);
    connect(m_dataManageThread,&QThread::finished,m_dataManage,&DataManage::deleteLater,Qt::DirectConnection);
    connect(m_dataManageThread,&QThread::finished,m_dataManageThread,&QThread::deleteLater,Qt::DirectConnection);
    connect(m_dataManage,&DataManage::SignalUsnRecordLog,LogWindow::getInstance(),&LogWindow::SlotRealPlainTextEditAddData,Qt::QueuedConnection);

    //获取设备失败，退出程序
    connect(m_dataManage,&DataManage::SignalFail,this,[=]()
    {
        QMessageBox::critical(NULL,"失败","获取失败设备列表失为空!\n关闭弹窗,立即关闭程序!");
        emit finishAllThread();
    },Qt::QueuedConnection);
    //获取设备成功，创建界面
    connect(m_dataManage,&DataManage::SignalSucceed,this,[=]()
    {
        //创建窗口 -- UI
        createUi();
    },Qt::QueuedConnection);

    //逻辑开始运行
    m_dataManageThread->start();
}

MainWindow::~MainWindow()
{
    QMetaObject::invokeMethod(m_dataManage, "SlotFinish", Qt::BlockingQueuedConnection);
    qDeleteAll(*MemoryPool::getInstance()->getWindows());
    MemoryPool::getInstance()->getWindows()->clear();
    m_dataManageThread->quit();
    m_dataManageThread->wait();
}

//退出程序
void MainWindow::SlotExitProgram()
{
    for(auto  i: *MemoryPool::getInstance()->getWindows())
        i->close();
    emit finishAllThread();
}
//创建界面
void MainWindow::createUi()
{
    SlotNewWindow(initWindowRect());
}
//初始化窗口大小和位置
QRect MainWindow::initWindowRect()
{
    QDesktopWidget *screenResolution = QApplication::desktop();
    int h = 600;//默认窗口的高度
    int w = 1000;//默认窗口的宽度
    quint32 w_rand =QRandomGenerator::global()->generate()%10;
    quint32 h_rand =QRandomGenerator::global()->generate()%10;
    return QRect((screenResolution->width()-w)/(w_rand == 0 ? 1 : w_rand),(screenResolution->height()- h)/(h_rand == 0 ? 1 : h_rand),w, h);
}
//新建窗口
void MainWindow::SlotNewWindow(QRect rect)
{
    WindowClass *window = new WindowClass(rect);
    window->show();
    connect(m_dataManage,&DataManage::SignalDataChange,window->getModel(),&MyTableModel::SlotDataChange,Qt::QueuedConnection);//数据管理发送到model
    MemoryPool::getInstance()->getWindows()->append(window);
}
//关闭窗口
void MainWindow::closeWindow(QObject *receiver)
{

    WindowClass *window =qobject_cast<WindowClass *>(receiver);
    if(MemoryPool::getInstance()->getWindows()->count() <= 1){
        MemoryPool::getInstance()->getWindows()->removeOne(window);
        SlotExitProgram();
    } else {
        MemoryPool::getInstance()->getWindows()->removeOne(window);
        window->deleteLater();
    }

}
//显示窗口
void MainWindow::showWindows()
{
    if(!MemoryPool::getInstance()->getWindows()->isEmpty()) {
        for(auto i : *MemoryPool::getInstance()->getWindows()) {
            //是隐藏 - 显示
            i->setVisible(true);
            //是最小化 - 显示
            i->showNormal();
            //设置活动窗口
            i->activateWindow();
        }
    }
}
//关于
void MainWindow::about()
{
    //QApplication::aboutQt();
    QString msg = QString("%1<br>版本:V%2<br>使用手册:<a href='https://blog.csdn.net/weixin_43763292/article/details/125361565?csdn_share_tail=%7B%22type%22%3A%22blog%22%2C%22rType%22%3A%22article%22%2C%22rId%22%3A%22125361565%22%2C%22source%22%3A%22weixin_43763292%22%7D&ctrtid=qNxdK'>%1</a><br>E-mail:978358810@qq.com").arg(QCoreApplication::applicationName()).arg(QCoreApplication::applicationVersion());
    //QMessageBox::about(NULL,"关于",msg);
    QMessageBox message(QMessageBox::NoIcon, "关于", msg);
    message.setIconPixmap(QPixmap(":/Images/quickRun.png"));
    if(MemoryPool::getInstance()->getSettingArgs().windowTop) {
        //设置窗口顶置: 一直在最前面.
        Qt::WindowFlags m_flags = message.windowFlags();
        message.setWindowFlags(m_flags | Qt::WindowStaysOnTopHint);
    }
    message.exec();
}

//重写事件
bool MainWindow::notify(QObject *receiver, QEvent *e)
{
    //感兴趣事件返回true；不感兴趣返回false
    //自定义全局事件；新建窗口
    if(e->type() == NEW_WINDOW){
        createUi();
        return true;
    }
    //自定义全局事件；关闭窗口
    if(e->type() == CLOSE_WINDOW){
        closeWindow(receiver);
        return true;
    }
    //自定义全局事件；结束程序
    if(e->type() == EXIT_PROGRAM){
        SlotExitProgram();
        return true;
    }
    //自定义全局事件；创建控制台
    if(e->type() == CREATE_CONSOLE){
        if(!gRedirection){//防止多次运行
            CreateConsole();
            gRedirection = true;
            qDebug()<<g_OperateFileName<<"打开控制台.";
        }
        return true;
    }
    //自定义全局事件；关闭控制台
    if(e->type() == CLOSE_CONSOLE){
        if(gRedirection){//防止多次运行
            gRedirection = false;
            CloseConsole();
            qDebug()<<g_OperateFileName<<"关闭控制台.";
        }
        return true;
    }
    //自定义全局事件；显示全部窗口
    if(e->type() == SHOW_WINDOWS){
        showWindows();
        return true;
    }
    //自定义全局事件；显示关于
    if(e->type() == ABOUT){
        about();
        return true;
    }

    return QApplication::notify(receiver,e);
}





