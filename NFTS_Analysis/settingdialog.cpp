#include "settingdialog.h"
#include "ui_settingdialog.h"

#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QProcess>
#include <QDebug>
#include <QMenu>
#include "mycheckboxcombo.h"

QProcess* SettingDialog::AutoRunprocess = new  QProcess();
SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("选项");
    this->setWindowIcon(QIcon(":/Images/quickRun.png"));
    // this->setModal(true); // 对话框特有
    //阻塞父窗口
    this->setWindowModality(Qt::WindowModal);
    // 去掉问号和左上角图标，只保留关闭
    this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    ui->buttonBox->addButton(QDialogButtonBox::Apply);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
    ui->buttonBox->button(QDialogButtonBox::Apply)->setText("应用");


    ui->columnsComboBox->AddItems(g_ModelHeadL);
    //创建系统托盘
    SystemTrayProtected();
    //系统托盘是否显示
    SystemTray(MemoryPool::getInstance()->getSettingArgs().showSystemTray);
}

SettingDialog::~SettingDialog()
{
    delete ui;
}
void SettingDialog::SystemTray(bool checked)
{
    m_sysTrayIcon->setVisible(checked);
}
//开机自起
void SettingDialog::AppAutoRun(bool checked)
{
    QString application_name = QApplication::applicationName();//获取应用名称
    QString application_path = QApplication::applicationFilePath();//找到应用的目录

    //只创建一次
    static bool once = true;
    if(once) {
        connect(AutoRunprocess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                [=](int exitCode, QProcess::ExitStatus exitStatus)
        {
            if(exitCode == 0 && exitStatus == QProcess::NormalExit){
                qDebug()<<"创建任务计划来完成开机登录自启 ---设置成功";
                //QMessageBox::information(NULL,"开机登录自启设置结果","设置成功!");
            } else {
                qDebug()<<"创建任务计划来完成开机登录自启 --- 设置失败";
                //QMessageBox::warning(NULL,"开机登录自启设置结果","设置失败!");
            }
        });
        once = false;
    }
    if(checked){
        //采用创建任务计划来完成开机登录自启
        AutoRunprocess->start(QString("schtasks.exe"),QStringList()<<"/create"<<"/tn"<<application_name<<"/rl"<<"highest"<<"/tr"<<application_path<<"/sc"<<"onlogon"<<"/f");

    } else {
        AutoRunprocess->start(QString("schtasks.exe"),QStringList()<<"/delete"<<"/tn"<<application_name<<"/f");
    }

    //schtasks /create /tn 2222 /tr "C:\Program Files\QuickRun2\bin\QuickRun.exe" /sc ONLOGON /rl highest /f
    //    QSettings settings(AUTO_RUN_KEY, QSettings::NativeFormat);//创建QSetting, 需要添加QSetting头文件
    //    if(checked)
    //    {
    //        QString application_path = QApplication::applicationFilePath();//找到应用的目录
    //        settings.setValue(application_name, application_path.replace("/", "\\"));//写入注册表
    //    }
    //    else
    //    {
    //        settings.remove(application_name);		//从注册表中删除
    //    }
    //    settings.sync();
}
//以管理员身份运行 --- 待
void SettingDialog::SystemAdminRun(bool checked)
{
    QString application_path = QApplication::applicationFilePath();//找到应用的目录
    QSettings settings(ADMIN_RUN_KEY, QSettings::NativeFormat);//创建QSetting, 需要添加QSetting头文件
    if(checked)
        settings.setValue(QDir::fromNativeSeparators(application_path),"RUNASADMIN" );//写入注册表
    else
        settings.remove(application_path);		//从注册表中删除
    settings.sync();
}

//系统托盘
void SettingDialog::SystemTrayProtected()
{
    //新建QSystemTrayIcon对象
    static QSystemTrayIcon systemTray;
    m_sysTrayIcon = &systemTray;
    createMenu();
    //设置托盘图标
    QIcon icon = QIcon(":/Images/quickRun.png");    //资源文件添加的图标
    m_sysTrayIcon->setIcon(icon);

    //当鼠标移动到托盘上的图标时，会显示此处设置的内容
    m_sysTrayIcon->setToolTip("托盘提示信息");

    //给QSystemTrayIcon添加槽函数
    connect(m_sysTrayIcon, &QSystemTrayIcon::activated,[=](QSystemTrayIcon::ActivationReason reason)
    {
        switch(reason){
        case QSystemTrayIcon::DoubleClick:
            SlotShowWindow();
            //            //单击托盘图标
            //            m_sysTrayIcon->showMessage(QObject::trUtf8(""),
            //                                       QObject::trUtf8("欢迎使用此程序"),
            //                                       QSystemTrayIcon::Information,
            //                                       1000);
            break;
        default:
            break;
        }
    });

}

//创建托盘菜单
void SettingDialog::createMenu()
{
    QMenu * m_menu = new QMenu(this);
    QAction *action_showWindow = new QAction(tr("显示全部搜索窗口(&S)"), m_menu);
    connect(action_showWindow,&QAction::triggered,this,&SettingDialog::SlotShowWindow);
    QAction *action_newWindow = new QAction(tr("新建搜索窗口(&N)"), m_menu);
    connect(action_newWindow,&QAction::triggered,this,&SettingDialog::SlotNewWindow);
    QAction *action_option = new QAction(tr("选项(&O)"), m_menu);
    connect(action_option,&QAction::triggered,this,&SettingDialog::SlotOption);
    QAction *action_exit = new QAction(tr("退出(&X)"), m_menu);
    connect(action_exit,&QAction::triggered,this,&SettingDialog::SlotExit);
    QAction *action_about = new QAction(tr("关于(&A)"), m_menu);
    connect(action_about,&QAction::triggered,this,&SettingDialog::SlotAbout);
    //新增菜单项---显示主界面
    m_menu->addAction(action_showWindow);
    m_menu->addAction(action_newWindow);
    //增加分隔符
    m_menu->addSeparator();
    m_menu->addAction(action_option);
    //增加分隔符
    m_menu->addSeparator();
    m_menu->addAction(action_exit);
    //增加分隔符
    m_menu->addSeparator();
    m_menu->addAction(action_about);
    //把QMenu赋给QSystemTrayIcon对象
    m_sysTrayIcon->setContextMenu(m_menu);
}

//显示窗口
void SettingDialog::SlotShowWindow()
{
    QEvent event(SHOW_WINDOWS);
    QApplication::sendEvent(qApp,&event);
}

//新建窗口
void SettingDialog::SlotNewWindow()
{
    QEvent event(NEW_WINDOW);
    QApplication::sendEvent(qApp,&event);
}

//选项
void SettingDialog::SlotOption()
{
    this->Show();
}

//退出
void SettingDialog::SlotExit()
{
    //m_sysTrayIcon->setVisible(false);
    QEvent event(EXIT_PROGRAM);
    QApplication::sendEvent(qApp,&event);
}
//关于
void SettingDialog::SlotAbout()
{
    QEvent event(ABOUT);
    QApplication::sendEvent(qApp,&event);
}

//设置/取消自启动
//: true(开机启动)    false(开机不启动)

void SettingDialog::on_AppAutoRuncheckBox_clicked(bool checked)
{
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
}

//系统托盘
void SettingDialog::on_SystemTrayCheckBox_clicked(bool checked)
{
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
}
void SettingDialog::on_real_timeSearchCheckBox_clicked(bool checked)
{
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
}

void SettingDialog::on_enableDelFileFlagCheckBox_clicked(bool checked)
{
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
}

void SettingDialog::on_dataBaseLoadCheckBox_clicked(bool checked)
{
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
}
void SettingDialog::on_hideSearchButton_clicked(bool checked)
{
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
}

void SettingDialog::on_windowTopCheckBox_clicked(bool checked)
{
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
}
void SettingDialog::on_usnRecordNoMonitoringCheckBox_clicked(bool checked)
{
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
}

void SettingDialog::on_columnsComboBox_activated(int index)
{
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
}


//设置参数
S_SETTING_ARGS SettingDialog::setArgs()
{
    S_SETTING_ARGS settingArgs;
    settingArgs.autoStaringUp =  ui->AppAutoRuncheckBox->checkState() == Qt::Checked ? true : false;
    settingArgs.showSystemTray =  ui->SystemTrayCheckBox->checkState() == Qt::Checked ? true : false;
    settingArgs.real_timeSearch =  ui->real_timeSearchCheckBox->checkState() == Qt::Checked ? true : false;
    settingArgs.enableDeleteFileFlag =  ui->enableDelFileFlagCheckBox->checkState() == Qt::Checked ? true : false;
    settingArgs.showSearchButton =  ui->hideSearchButton->checkState() == Qt::Checked ? true : false;
    settingArgs.dataBaseLoad =  ui->dataBaseLoadCheckBox->checkState() == Qt::Checked ? true : false;
    settingArgs.windowTop =  ui->windowTopCheckBox->checkState() == Qt::Checked ? true : false;
    settingArgs.usnRecordNoMonitoring =  ui->usnRecordNoMonitoringCheckBox->checkState() == Qt::Checked ? true : false;
    settingArgs.columns = ui->columnsComboBox->GetShowColumns();

    AppAutoRun(settingArgs.autoStaringUp);
    //SystemAdminRun(settingArgs.autoStaringUp);
    SystemTray(settingArgs.showSystemTray);
    return settingArgs;
}

void SettingDialog::on_buttonBox_clicked(QAbstractButton *button)
{

    if(button == (QAbstractButton *)(ui->buttonBox->button(QDialogButtonBox::Ok))) {
        if(ui->buttonBox->button(QDialogButtonBox::Apply)->isEnabled()) {
            MemoryPool::getInstance()->setSettingArgs(setArgs());
            emit SignalUpdateWidget();//更新界面
        }
    }  else if(button == (QAbstractButton *)(ui->buttonBox->button(QDialogButtonBox::Apply))){
        button->setEnabled(false);
        MemoryPool::getInstance()->setSettingArgs(setArgs());
        emit SignalUpdateWidget();//更新界面
    }
}

//恢复默认值
void SettingDialog::on_pushButton_clicked()
{
    ui->AppAutoRuncheckBox->setCheckState(Qt::Unchecked);
    ui->SystemTrayCheckBox->setCheckState(Qt::Unchecked);
    ui->real_timeSearchCheckBox->setCheckState(Qt::Unchecked);
    ui->enableDelFileFlagCheckBox->setCheckState(Qt::Unchecked);
    ui->hideSearchButton->setCheckState(Qt::Unchecked);
    ui->dataBaseLoadCheckBox->setCheckState(Qt::Unchecked);
    ui->windowTopCheckBox->setCheckState(Qt::Unchecked);
    ui->usnRecordNoMonitoringCheckBox->setCheckState(Qt::Unchecked);
    ui->columnsComboBox->resetItems(Qt::Unchecked);

    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
}
//显示读取
void SettingDialog::Show()
{
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
    S_SETTING_ARGS settingArgs = MemoryPool::getInstance()->getSettingArgs();

    ui->AppAutoRuncheckBox->setCheckState(settingArgs.autoStaringUp == false ? Qt::Unchecked : Qt::Checked );
    ui->SystemTrayCheckBox->setCheckState(settingArgs.showSystemTray == false ? Qt::Unchecked : Qt::Checked );
    ui->real_timeSearchCheckBox->setCheckState(settingArgs.real_timeSearch == false ? Qt::Unchecked : Qt::Checked );
    ui->enableDelFileFlagCheckBox->setCheckState(settingArgs.enableDeleteFileFlag == false ? Qt::Unchecked : Qt::Checked );
    ui->hideSearchButton->setCheckState(settingArgs.showSearchButton == false ? Qt::Unchecked : Qt::Checked );
    ui->dataBaseLoadCheckBox->setCheckState(settingArgs.dataBaseLoad == false ? Qt::Unchecked : Qt::Checked );
    ui->windowTopCheckBox->setCheckState(settingArgs.windowTop == false ? Qt::Unchecked : Qt::Checked );
    ui->usnRecordNoMonitoringCheckBox->setCheckState(settingArgs.usnRecordNoMonitoring == false ? Qt::Unchecked : Qt::Checked );
    ui->columnsComboBox->resetItems(settingArgs.columns);
    show();
}



