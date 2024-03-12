#include "logmodel.h"
#include <QtConcurrent>
#include <QtXlsx>//xlsx
#include "xlsxdocument.h"

//------------------------------------------------------
//-----------------RealLogModel-----------------------------
//------------------------------------------------------



RealLogModel::RealLogModel(QString connectName, QObject *parent) : QAbstractTableModel(parent)
{
    m_connectName = connectName;
}
RealLogModel::~RealLogModel()
{

}
bool RealLogModel::connectDB()
{
    if(MySQLite_db.isOpen()) {
        MySQLite_db.close();
    }

    if(QSqlDatabase::contains(m_connectName)) {
        MySQLite_db = QSqlDatabase::database(m_connectName);
    } else {
        MySQLite_db = QSqlDatabase::addDatabase("QSQLITE",m_connectName);//m_connectName;创建不同的连接名，用于多线程
    }

    static QDir dir;
    static QString path = USN_RECORD_PATH;
    if(dir.mkpath(path)){
        MySQLite_db.setDatabaseName( path + QString("%1_usnRecord.db").arg(QDate::currentDate().toString("yyyy.MM.dd")));
    }

    if (!MySQLite_db.open()){
        return false;
    }
    QSqlQuery query(MySQLite_db);

    query.exec("PRAGMA synchronous=OFF;");//PRAGMA synchronous = OFF 关闭同步模式，减少时间
    query.exec("PRAGMA Journal_Mode=WAL;");
    query.exec("PRAGMA Cache_Size=5000;");
    return true;
}

bool RealLogModel::createTable()
{
    if(g_UsnLogSqlTitle.count()!=10)
        Q_ASSERT(0);
    QSqlQuery query(MySQLite_db);

    //保存文件路径表
    QString ss =QString("CREATE TABLE if not exists '%1' ("//if not exists
                        " '%2'	TEXT NOT NULL,"//
                        " '%3'	TEXT NOT NULL,"//
                        " '%4'	INTEGER NOT NULL,"
                        " '%5'	INTEGER NOT NULL,"//
                        " '%6'	INTEGER NOT NULL,"//
                        " '%7'	INTEGER NOT NULL,"//
                        " '%8'	INTEGER NOT NULL," //
                        " '%9'	INTEGER NOT NULL,"//
                        " '%10'	INTEGER NOT NULL," //
                        " '%11'	TEXT NOT NULL"//
                        " )").arg(TABLENAME).arg(g_UsnLogSqlTitle.at(0)).arg(g_UsnLogSqlTitle.at(1)).arg(g_UsnLogSqlTitle.at(2)).arg(g_UsnLogSqlTitle.at(3))
            .arg(g_UsnLogSqlTitle.at(4)).arg(g_UsnLogSqlTitle.at(5)).arg(g_UsnLogSqlTitle.at(6)).arg(g_UsnLogSqlTitle.at(7)).arg(g_UsnLogSqlTitle.at(8))
            .arg(g_UsnLogSqlTitle.at(9));

    if(!(query.exec(ss))){
        return false;
    }
    return true;

}
void RealLogModel::init()
{
    timer = new QTimer();//定时器的创建和调用必须在同一个线程
    if(!timer->isSingleShot())
        timer->setSingleShot(true);//设置时间后，只触发一次，单次触发
    connect(timer,&QTimer::timeout,this,[&]()mutable
    {
        writeDatas();//写入数据到数据库
        beData();
    });

}
int RealLogModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    } else {
        return m_datas.count();
    }
}
int RealLogModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    } else {
        return g_UsnLogTitle.count();   //表头的大小
    }
}
QVariant RealLogModel::data(const QModelIndex& index, int role) const
{ 
    if(!index.isValid())
        return QVariant();

    const int row=index.row();
    //显示数据
    if(role == Qt::DisplayRole ){
        switch(index.column()){
        case 0: return m_datas.at(row).dataTime.toString("yyyy.MM.dd-hh:mm:ss.zzz");
        case 1: return m_datas.at(row).partition;
        case 2: return m_datas.at(row).usnOffset;
        case 3: return m_datas.at(row).mft;
        case 4: return m_datas.at(row).pMft;
        case 5: return m_datas.at(row).usn;
        case 6: return GlobalFunction::getInstance()->FILETIMEToDateTime(m_datas.at(row).reasonDataTime);
        case 7: return m_datas.at(row).reason;
        case 8: return m_datas.at(row).attributes;
        case 9: return m_datas.at(row).fileName;
        }
    }
    if (role == Qt::TextAlignmentRole) {    //对其方式
        return left_VCenter; //对齐格式为居中
    }
    return QVariant();
    //return  QAbstractTableModel::data(index, role);

}
QVariant RealLogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal){
        //这里我们只设置居中对齐和文本
        if (role == Qt::DisplayRole){
            //这里把横项列表头的文本设计为可以设置的
            if(section>=0 && section<g_UsnLogTitle.count())
                return g_UsnLogTitle.at(section);
            return QString("Col %1").arg(section + 1);
        }else if(role == Qt::TextAlignmentRole){
            return Qt::AlignCenter;
        }
    }else{
        if (role == Qt::DisplayRole)
            return QString("%1").arg(section);
        else if(role == Qt::TextAlignmentRole)
            return Qt::AlignCenter;
    }
    return QVariant();
}

//接收数据
void RealLogModel::SlotAddData(S_USN_LOG item)
{
    m_tempDatas.append(item);
    //同时接收大量数据超过设定值，更新界面
    if(m_tempDatas.count() >= ENDCOUNT){

        writeDatas();//写入数据到数据库
        beData();
        return;
    }
    timer->setInterval(100);//设置时间间隔超出触发timeOut信号，下次执行timer->setInterval(1000);会覆盖当前触发信号，继续等待1000；
    timer->start();

}

//写入数据
void RealLogModel::writeDatas()
{
    if(!connectDB()) {
        qDebug()<<"usnjournal数据库连接失败";
        return;
    }
    if(!createTable()) {
        qDebug()<<"usnjournal创建表失败";
        return;
    }
    QElapsedTimer time;
    time.start();

    QVariantList _1;
    QVariantList _2;
    QVariantList _3;
    QVariantList _4;
    QVariantList _5;
    QVariantList _6;
    QVariantList _7;
    QVariantList _8;
    QVariantList _9;
    QVariantList _10;
    MySQLite_db.transaction();
    QSqlQuery query(MySQLite_db);
    QString insert_sql = QString("INSERT INTO '%1' ('%2','%3','%4','%5','%6','%7','%8','%9','%10','%11')\
                                 VALUES(?,?,?,?,?,?,?,?,?,?)").arg(TABLENAME).arg(g_UsnLogSqlTitle.at(0)).arg(g_UsnLogSqlTitle.at(1)).arg(g_UsnLogSqlTitle.at(2)).arg(g_UsnLogSqlTitle.at(3))
                                 .arg(g_UsnLogSqlTitle.at(4)).arg(g_UsnLogSqlTitle.at(5)).arg(g_UsnLogSqlTitle.at(6)).arg(g_UsnLogSqlTitle.at(7)).arg(g_UsnLogSqlTitle.at(8))
                                 .arg(g_UsnLogSqlTitle.at(9));
            query.prepare(insert_sql);


    for(int i=0;i<m_tempDatas.count();++i){
        _1<<m_tempDatas.at(i).dataTime.toString("yyyy.MM.dd-hh:mm:ss.zzz");
        _2<<m_tempDatas.at(i).partition;
        _3<<m_tempDatas.at(i).usnOffset;
        _4<<m_tempDatas.at(i).mft;
        _5<<m_tempDatas.at(i).pMft;
        _6<<m_tempDatas.at(i).usn;
        _7<<m_tempDatas.at(i).reasonDataTime;
        _8<<m_tempDatas.at(i).reason;
        _9<<m_tempDatas.at(i).attributes;
        _10<<m_tempDatas.at(i).fileName;

    }

    query.addBindValue( _1);
    query.addBindValue( _2);
    query.addBindValue( _3);
    query.addBindValue( _4);
    query.addBindValue( _5);
    query.addBindValue( _6);
    query.addBindValue( _7);
    query.addBindValue( _8);
    query.addBindValue( _9);
    query.addBindValue( _10);
    if(!query.execBatch()){
        MySQLite_db.rollback();
        //return;
    }else{
        MySQLite_db.commit();
        qDebug()<<"数据库表("+TABLENAME+")保存成功耗时:"<<time.elapsed()<<" ms";
    }

    if(MySQLite_db.isOpen()) {
        MySQLite_db.close();
    }
}
//清空数据
void RealLogModel::SlotClear()
{
    QModelIndexList from;
    QModelIndexList to;
    emit layoutAboutToBeChanged();
    m_datas.clear();
    changePersistentIndexList(from,to);
    emit layoutChanged();

}

//更新view
void RealLogModel::beData()
{
    QModelIndexList from;
    QModelIndexList to;
    emit layoutAboutToBeChanged();
    m_datas.append(m_tempDatas);
    changePersistentIndexList(from,to);
    emit layoutChanged();
    m_tempDatas.clear();
}
//导出
void RealLogModel::SlotExportData(const QString &fileName)
{
    //另开一个线程导出
    QtConcurrent::run([=]() {
        QT_TRY {
            QList<S_USN_LOG> exportData;
            exportData.reserve(m_datas.count());
            exportData.append(m_datas);
            QFile file(fileName);
            if(file.open(QIODevice::WriteOnly | QIODevice::Append)){
                QTextStream text_stream(&file);
                emit SignalStatusbarUpdata(Status_Text,"数据正在导出中...");
                qDebug()<<g_OperateFileName<<"导出文件名:"<<fileName<<"  :  数据正在导出中...";
                //导出文件格式xlsx- -最后保存压缩时间超长，不建议保存xlsx格式。可以先保存csv格式，用Excel转换方便
                if(fileName.endsWith(".xlsx",Qt::CaseInsensitive)) {
                    QXlsx::Document xlsx;
                    for(int i=0;i<g_UsnLogTitle.count();i++)
                        xlsx.write(1,i+1,g_UsnLogTitle.at(i));//设置标题
                    for(qint64 i=0;i<exportData.count();i++){
                        xlsx.write(i+2,1,exportData.at(i).dataTime.toString("yyyy.MM.dd-hh:mm:ss.zzz"));
                        xlsx.write(i+2,2,exportData.at(i).partition);
                        xlsx.write(i+2,3,exportData.at(i).usnOffset);
                        xlsx.write(i+2,4,exportData.at(i).mft);
                        xlsx.write(i+2,5,exportData.at(i).pMft);
                        xlsx.write(i+2,6,exportData.at(i).usn);
                        xlsx.write(i+2,7,exportData.at(i).reasonDataTime);
                        xlsx.write(i+2,8,exportData.at(i).reason);
                        xlsx.write(i+2,9,exportData.at(i).attributes);
                        xlsx.write(i+2,10,exportData.at(i).fileName);
                    }
                    xlsx.saveAs(&file);


                } else {
                    /* .csv .text
             * 执行下面
             */
                    //首次写入标题
                    QByteArray array="";
                    for(int i=0;i<g_UsnLogTitle.count();i++){
                        array.append(g_UsnLogTitle.at(i));
                        if((i+1)!= g_UsnLogTitle.count())
                            array.append(',');
                        else
                            array.append('\n');
                    }
                    //file.write(array);
                    text_stream << array;//将任何缓存数据刷新到文件中
                    array.clear();
                    for(qint64 i=0;i<exportData.count();i++) {
                        array.append(exportData.at(i).dataTime.toString("yyyy.MM.dd-hh:mm:ss.zzz"));
                        array.append(',');
                        array.append(exportData.at(i).partition);
                        array.append(',');
                        array.append(exportData.at(i).usnOffset);
                        array.append(',');
                        array.append(exportData.at(i).mft);
                        array.append(',');
                        array.append(exportData.at(i).pMft);
                        array.append(',');
                        array.append(exportData.at(i).usn);
                        array.append(',');
                        array.append(exportData.at(i).reasonDataTime);
                        array.append(',');
                        array.append(exportData.at(i).reason);
                        array.append(',');
                        array.append(exportData.at(i).attributes);
                        array.append(',');
                        array.append(exportData.at(i).fileName);
                        array.append('\n');
                        text_stream << array;//将任何缓存数据刷新到文件中
                        array.clear();
                    }
                    file.close();
                }
                emit SignalStatusbarUpdata(Status_Text,"数据导出成功!");
                qDebug()<<g_OperateFileName<<"导出文件名:"<<fileName<<"  :  数据导出成功!";
            } else {
                emit SignalStatusbarUpdata(Status_Text,"文件打开失败!");//导出失败
                qDebug()<<g_OperateFileName<<"导出文件名:"<<fileName<<"  :  文件打开失败!";
            }
        }
        QT_CATCH(...)
        {
            emit SignalStatusbarUpdata(Status_Text,"文件导出异常!");//导出失败
            qDebug()<<g_OperateFileName<<"导出文件名:"<<fileName<<"  :  文件导出异常!";
        }
    });
}

//------------------------------------------------------
//-----------------RealMonitorModel-----------------------------
//------------------------------------------------------



RealMonitorModel::RealMonitorModel( QObject *parent) : QAbstractTableModel(parent)
{
    m_datas = m_xml.ReadXml();
    m_interval = m_xml.ReadTimeXml();
}
RealMonitorModel::~RealMonitorModel()
{

}

void RealMonitorModel::init()
{
    m_interval = 1000;
    // m_reduce.start();

}
int RealMonitorModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    } else {
        return m_datas.count();
    }
}
int RealMonitorModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    } else {
        return g_UsnMonitorTitle.count();   //表头的大小
    }
}
QVariant RealMonitorModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    const int row=index.row();
    //显示数据
    if(role == Qt::DisplayRole ){
        switch(index.column()){
        case 0: return m_datas.at(row).usnLogItem.partition;
        case 1: return m_datas.at(row).usnLogItem.mft;

        case 2: return m_datas.at(row).usnLogItem.pMft;

        case 3: return m_datas.at(row).usnLogItem.reason;

        case 4: return m_datas.at(row).usnLogItem.attributes;

        case 5: return m_datas.at(row).usnLogItem.fileName;
        case 6: return m_datas.at(row).command;
        case 7: return m_datas.at(row).flag;
        }

    }
    if (role == Qt::TextAlignmentRole) {    //对其方式
        return left_VCenter; //对齐格式为居中
    }
    return QVariant();
    //return  QAbstractTableModel::data(index, role);

}
QVariant RealMonitorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal){
        //这里我们只设置居中对齐和文本
        if (role == Qt::DisplayRole){
            //这里把横项列表头的文本设计为可以设置的
            if(section>=0 && section<g_UsnMonitorTitle.count())
                return g_UsnMonitorTitle.at(section);
            return QString("Col %1").arg(section + 1);
        }else if(role == Qt::TextAlignmentRole){
            return Qt::AlignCenter;
        }
    }else{
        if (role == Qt::DisplayRole)
            return QString("%1").arg(section);
        else if(role == Qt::TextAlignmentRole)
            return Qt::AlignCenter;
    }
    return QVariant();
}
//接收usn数据
void RealMonitorModel::SlotReceiveUsnData(S_USN_LOG item)
{
    foreach(S_MONITOR_ITEM iter, getDataList()) {
        if(iter.flag) {
            if(dataCompare(iter,item)) {
                //节流执行 -- 最低每隔m_interval执行一次
                QThread::msleep(getTime());
                if(!iter.command.isEmpty()) {
                    bool isOk = QProcess::startDetached(iter.command);
                }
            }
        }

    }
}
//接收执行最低时长
void RealMonitorModel::SlotReceiveInterval(quint64 time)
{
    getTime() = time;
    m_xml.UpdateXml(m_datas,m_interval);
}

bool RealMonitorModel::dataCompare(const S_MONITOR_ITEM &local ,const S_USN_LOG &rece)
{
    bool isOk = true;
    if(!local.usnLogItem.partition.isEmpty()) {
        if(QString::compare(local.usnLogItem.partition,rece.partition,Qt::CaseInsensitive)) {
            return false;
        }
    }
    if(local.usnLogItem.mft != -1) {
        if(local.usnLogItem.mft != rece.mft) {
            return false;
        }
    }
    if(local.usnLogItem.pMft!= -1) {
        if(local.usnLogItem.pMft != rece.pMft) {
            return false;
        }
    }
    if(local.usnLogItem.reason!= -1) {
        if(local.usnLogItem.reason != rece.reason) {
            return false;
        }
    }
    if(local.usnLogItem.attributes!= -1) {
        if(local.usnLogItem.attributes != rece.attributes) {
            return false;
        }
    }
    if(!local.usnLogItem.fileName.isEmpty()) {
        //使用正则
        if(local.RegExp){
            QRegExp regExp;
            //区分大小写
            if(local.CaseInsensitive) {
                regExp = QRegExp(local.usnLogItem.fileName,Qt::CaseSensitive);
            } else {
                regExp = QRegExp(local.usnLogItem.fileName,Qt::CaseInsensitive);
            }
            if(!regExp.isValid())
                return false;

            if(!QString(rece.fileName).contains(regExp)) {
                return false;
            }


        }else {
            QByteArray byteTemp;
            if(local.CaseInsensitive)//区分大小写
                byteTemp = local.usnLogItem.fileName;
            else
                byteTemp = local.usnLogItem.fileName.toLower();

            //完全匹配
            if(local.Complete){
                //区分大小写
                if(local.CaseInsensitive){
                    if(QString::compare(QString(byteTemp),QString(rece.fileName),Qt::CaseSensitive)) {
                        return false;
                    }

                }else{
                    if(QString::compare(QString(byteTemp),QString(rece.fileName),Qt::CaseInsensitive)) {
                        return false;
                    }
                }
            }else{
                if(local.CaseInsensitive){//区分大小写
                    if(!QString(rece.fileName).contains(byteTemp,Qt::CaseSensitive)) {
                        return false;
                    }
                } else {
                    if(!QString(rece.fileName).contains(byteTemp,Qt::CaseInsensitive)) {
                        return false;
                    }
                }
            }
        }
    }
    return isOk;
}
//接收数据
void RealMonitorModel::SlotChangeData(E_CHANGE type,S_MONITOR_ITEM item,int row)
{
    if(AppendData == type) {
        beginInsertRows( QModelIndex(), m_datas.count(), m_datas.count());
        getDataList().append(item);
        endInsertRows();
        m_xml.UpdateXml(m_datas,m_interval);
        return;
    }
    if(m_datas.isEmpty()) {
        return;
    }


    //行数大于数据个数直接返回
    if(row > m_datas.count()-1) {
        return;
    }
    switch ((E_CHANGE)type) {
    case InsertData:{

        beginInsertRows( QModelIndex(), row, row);
        getDataList().insert(row,item);
        endInsertRows();
    }
        break;
    case DeleteData:{

        beginRemoveRows(QModelIndex(), row, row);
        getDataList().removeAt(row);
        endRemoveRows();

    }
        break;
    case UpdateData:{

        QModelIndexList from;
        QModelIndexList to;
        emit layoutAboutToBeChanged();
        getDataList().replace(row,item);
        changePersistentIndexList(from,to);
        emit layoutChanged();
    }
        break;
    case UpData:{
        if(row > 0) {
            beginMoveRows(QModelIndex(), row, row, QModelIndex(), row-1);
            getDataList().swap(row,row-1);
            endMoveRows();
        }
    }
        break;
    case DownData:{

        if(row < m_datas.count()-1) {
            beginMoveRows(QModelIndex(), row, row, QModelIndex(), row+2);
            getDataList().swap(row,row+1);
            endMoveRows();
        }

    }
        break;
    default:
        break;
    }
    m_xml.UpdateXml(m_datas,m_interval);

}
QList<S_MONITOR_ITEM> & RealMonitorModel::getDataList()
{
    QMutexLocker __Locker(&m_dataMutex);
    return m_datas;
}
quint64 & RealMonitorModel::getTime()
{
    QMutexLocker __Locker(&m_timeMutex);
    return m_interval;
}

//获取某行数据
S_MONITOR_ITEM RealMonitorModel::SlotGetRowData(int row)
{
    return m_datas.value(row);
}
//获取最低时长执行
quint64 RealMonitorModel::SlotGetElapsed()
{
    return  m_interval;
}
