#include "mytableview.h"
#include <QMenu>
#include <QMessageBox>
#include <QDialog>
#include <QProcess>
#include <QVBoxLayout>
#include <QUrl>
#include <QTextEdit>
#include <QDebug>
#include <QDesktopServices>
#include <QClipboard>
#include <QFileDialog>
#include <QContextMenuEvent>
#include "ntfsdatarecovery.h"
MyTableView::MyTableView(QWidget *parent) : QTableView(parent)
{
    this->setFocusPolicy(Qt::ClickFocus);//鼠标单击获取焦点
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);//不编辑
    this->setSelectionMode(QAbstractItemView::SingleSelection);//单个选择
    this->setSelectionBehavior(QAbstractItemView::SelectItems);//选择一个项


    setSortingEnabled(true);//启用排序功能

    m_dialog = new QDialog(this);
    // 去掉问号和左上角图标，只保留关闭
    m_dialog->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);



    m_dialogShowInfo = new QTextEdit(m_dialog);
    m_dialogShowInfo->setReadOnly(true);
    m_dialog->setWindowTitle("文件信息");
    m_dialog->setWindowModality(Qt::WindowModal);
    m_dialogLayout = new QVBoxLayout(m_dialog);
    m_dialogLayout->addWidget(m_dialogShowInfo);
    m_dialog->setLayout(m_dialogLayout);
    m_dialog->resize(600,400);
    m_dialog->hide();

    m_menu = new QMenu(this);
    m_actionOpen = new QAction(tr("打开"),this);
    m_actionOpenPath = new QAction(tr("打开路径"),this);
    m_actionCopyFullPath = new QAction(tr("复制完整路径&文件名"),this);
    m_actionCopyPath = new QAction(tr("复制路径"),this);
    m_actionCopy = new QAction(tr("复制选择内容"),this);
    m_actionFileRecover = new QAction(tr("恢复/复制数据"),this);
    m_actionFileInfo  = new QAction(tr("文件/目录信息"),this);

    m_menu->addAction(m_actionOpen);
    m_menu->addAction(m_actionOpenPath);
    m_menu->addAction(m_actionCopyFullPath);
    m_menu->addAction(m_actionCopyPath);
    m_menu->addAction(m_actionCopy);
    m_menu->addSeparator();
    m_menu->addAction(m_actionFileRecover);
    m_menu->addAction(m_actionFileInfo);

    connect(m_actionOpen,&QAction::triggered,this,&MyTableView::SlotActionOpen);
    connect(m_actionOpenPath,&QAction::triggered,this,&MyTableView::SlotActionOpenPath);
    connect(m_actionCopyFullPath,&QAction::triggered,this,&MyTableView::SlotActiCopyFullPath);
    connect(m_actionCopyPath,&QAction::triggered,this,&MyTableView::SlotActiCopyPath);
    connect(m_actionCopy,&QAction::triggered,this,&MyTableView::SlotActiCopy);
    connect(m_actionFileRecover,&QAction::triggered,this,&MyTableView::SlotActiFileRecover);
    connect(m_actionFileInfo,&QAction::triggered,this,&MyTableView::SlotActiFileInfo);
    connect(this,&MyTableView::doubleClicked,this,&MyTableView::SlotDoublenOpen);
    connect(this,&MyTableView::clicked,this,&MyTableView::SlotClicked);


}
//打开文件
void MyTableView::openFile(QString fileName)
{
    qDebug()<<g_OperateFileName<<"打开文件:"<<fileName ;
    QString arg = QDir::fromNativeSeparators(fileName);//  "\\"转为"/"
    //根据系统默认的程序打开文件
    if(!QDesktopServices::openUrl(QUrl::fromLocalFile(arg))){

        QString str="原因:\r\n1:文件不存在!\r\n2:系统文件!\r\n3:没有相关联的应用程序";
        QMessageBox::warning(this,"打开失败",str);
    }
}
//双击打开文件
void MyTableView::SlotDoublenOpen(const QModelIndex &index)
{

    if(index.isValid() && index.column() == T_FILENAME && qApp->mouseButtons() == Qt::LeftButton) {//双击文件名
        if(index.sibling(index.row(),T_MFT).data().toUInt() == ROOT_DIRECTORY_MFT_NUMBER) {//根目录
            QString path = index.sibling(index.row(),T_FILEPATH).data().toString();
            openFile(path);
        }else{
            QString path = index.sibling(index.row(),T_FILEPATH).data().toString();
            path+=index.data().toString();
            openFile(path);
        }
    }
    else if(index.isValid() && index.column() == T_FILEPATH && qApp->mouseButtons() == Qt::LeftButton) {//双击文件路径
        QString path = index.sibling(index.row(),index.column()).data().toString();
        QProcess process;
        process.startDetached(QString("explorer.exe"),QStringList()<<"/n,"<<"/select,"<<path);
    }

}

//打开
void MyTableView::SlotActionOpen()
{
    if(m_openPerIndex.isValid()){
        if(m_openPerIndex.sibling(m_openPerIndex.row(),T_MFT).data().toUInt() == 5) {//根目录
            QString path = m_openPerIndex.sibling(m_openPerIndex.row(),T_FILEPATH).data().toString();
            openFile(path);
        }  else {
            QString path = m_openPerIndex.sibling(m_openPerIndex.row(),T_FILEPATH).data().toString();
            path+=m_openPerIndex.data().toString();
            openFile(path);
        }
    }
}
//打开路径
void MyTableView::SlotActionOpenPath()
{
    if(m_openPerIndex.isValid()){
        QString path = m_openPerIndex.sibling(m_openPerIndex.row(),T_FILEPATH).data().toString()+m_openPerIndex.data().toString();
        QProcess process;
        process.startDetached(QString("explorer.exe"),QStringList()<<"/n,"<<"/select,"<<path);
    }
}
//复制完整路径
void MyTableView::SlotActiCopyFullPath()
{
    if(m_openPerIndex.isValid()) {
        QString path;
        if(m_openPerIndex.sibling(m_openPerIndex.row(),T_MFT).data().toUInt() == ROOT_DIRECTORY_MFT_NUMBER) {//根目录
            path = m_openPerIndex.sibling(m_openPerIndex.row(),T_FILEPATH).data().toString();
        }else{
            path = m_openPerIndex.sibling(m_openPerIndex.row(),T_FILEPATH).data().toString();
            path+=m_openPerIndex.data().toString();
        }
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText(path);
    }
}
//复制路径
void MyTableView::SlotActiCopyPath()
{
    if(m_openPerIndex.isValid()) {
        QString path = m_openPerIndex.sibling(m_openPerIndex.row(),T_FILEPATH).data().toString();
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText(path);
    }
}
//复制选择内容
void  MyTableView::SlotActiCopy()
{
    if(m_openPerIndex.isValid()){
        QString path = m_openPerIndex.data().toString();
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText(path);
    }
}

//文件恢复 -- 针对已删除的文件
void MyTableView::SlotActiFileRecover()
{
    if(m_openPerIndex.isValid()){
        for(int i=0;i<MemoryPool::getInstance()->getInfoList()->count();i++){
            if(MemoryPool::getInstance()->getInfoList()->at(i).partitionSerial == m_openPerIndex.sibling(m_openPerIndex.row(),T_PARTITION).data().toUInt()){
                S_RECOVER_INFO info;
                quint8 flag = m_openPerIndex.sibling(m_openPerIndex.row(),T_FILETYPE).data().toString().toUInt();
                MFTNUM mftNum = m_openPerIndex.sibling(m_openPerIndex.row(),T_MFT).data().toString().toUInt();

                QString extendName = m_openPerIndex.sibling(m_openPerIndex.row(),T_FILENAME).data().toString().section(QChar('.'),-1);

                QString temp = QString("文件(*.%1);;全部文件(*)").arg(extendName);
                QString fileName = QFileDialog::getSaveFileName(this, tr("恢复数据"),"",temp);
                if(!fileName.isEmpty()) {
                    info.pDrive = MemoryPool::getInstance()->getInfoList()->at(i).info.pDrive;
                    info.ntfsOffset = MemoryPool::getInstance()->getInfoList()->at(i).info.bytesOffset;
                    info.mft = mftNum;
                    info.saveFileName = fileName;
                    info.flag = flag;
                    QMetaObject::invokeMethod(CNtfsDataRecovery::getInstance(), "recovery", Qt::AutoConnection,Q_ARG(S_RECOVER_INFO,info));
                }
            }
        }
    }
}


//文件信息
void MyTableView::SlotActiFileInfo()
{
    m_dialogShowInfo->clear();
    if(m_openPerIndex.isValid()){
        for(int i=0;i<MemoryPool::getInstance()->getInfoList()->count();i++) {
            if(MemoryPool::getInstance()->getInfoList()->at(i).partitionSerial == m_openPerIndex.sibling(m_openPerIndex.row(),T_PARTITION).data().toUInt()) {
                m_dialogShowInfo->append(QString("%1:%2").arg("物理设备").arg(MemoryPool::getInstance()->getInfoList()->at(i).info.pDrive));
                m_dialogShowInfo->append(QString("%1:%2").arg("驱动盘").arg(MemoryPool::getInstance()->getInfoList()->at(i).info.partitionFlagName));
                m_dialogShowInfo->append(QString("%1:%2").arg("GUID").arg(QUuid(MemoryPool::getInstance()->getInfoList()->at(i).info.uuid).toString()));
            }
        }
        for(int i=0;i<g_ModelHeadL.count();i++) {
            m_dialogShowInfo->append(QString("%1:%2").arg(g_ModelHeadL.at(i)).arg(m_openPerIndex.sibling(m_openPerIndex.row(),i).data().toString()));
        }
    }
    m_dialog->show();
}

//重新实现菜单事件
void MyTableView::contextMenuEvent(QContextMenuEvent *event)
{
    QModelIndex index = this->indexAt(event->pos());
    m_openPerIndex = index;
    if(index.isValid()){
        if(index.column() == T_FILENAME){
            quint8 flag = index.sibling(index.row(),T_FILETYPE).data().toUInt();
            m_actionOpen->setEnabled(true);
            m_actionOpenPath->setEnabled(true);
            m_actionCopyFullPath->setEnabled(true);
            m_actionCopyPath->setEnabled(true);
            m_actionCopy->setEnabled(true);
            m_actionFileInfo->setEnabled(true);
            if(flag == NOEXISTING_FILE_FLAG) //已删除文件
                m_actionFileRecover->setEnabled(true);
            else if(flag == EXISTING_FILE_FLAG) //正常文件
                m_actionFileRecover->setEnabled(MemoryPool::getInstance()->getSettingArgs().enableDeleteFileFlag);
            else
                m_actionFileRecover->setEnabled(false);
            m_menu->exec(QCursor::pos());//菜单出现位置为鼠标右键点击的位置
        } else {
            m_actionOpen->setEnabled(false);
            m_actionOpenPath->setEnabled(false);
            m_actionCopyFullPath->setEnabled(false);
            m_actionCopyPath->setEnabled(false);
            m_actionCopy->setEnabled(true);
            m_actionFileRecover->setEnabled(false);
            m_actionFileInfo->setEnabled(false);
            m_menu->exec(QCursor::pos());//菜单出现位置为鼠标右键点击的位置
        }
    }
}

//单击获取索引
void MyTableView::SlotClicked(const QModelIndex &index)
{
    m_indexRow = index.row();
    m_indexColumn = index.column();
    if(index.isValid())
        perIndex = index;
}
