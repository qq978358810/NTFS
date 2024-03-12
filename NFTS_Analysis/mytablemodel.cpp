/*
 * model增加一行数据或者删除一行数据--让tableView视图刷新显示：比较速度
 * 1：beginResetModel();  endResetModel();是最快的
 * 2：beginInsertRows(QModelIndex(),pos, pos); m_datas.insert(pos,data); endInsertRows();  速度都比较慢
 * 3：和 beginRemoveRows(QModelIndex(),pos, pos); m_datas.insert(pos,data); endRemoveRows();速度都慢，，view会比较卡顿
 * view的刷新速度：1 > 2 > 3
 *
 */

#include "mytablemodel.h"

#include <QtConcurrent>
#include <QDebug>
#include <QtXlsx>//xlsx
#include "xlsxdocument.h"
#include "pdqSort.h"
MyTableModel::MyTableModel(S_UPDATE_VIEW item,QObject *parent)
    : QAbstractTableModel(parent)
{
    m_datasSearch = new QList<S_DATA_BODY*>;
    m_datasExtend = new QList<S_DATA_BODY*>;
    m_datasTemp = new QList<S_DATA_BODY*>;
    m_datas = new QList<S_DATA_BODY*>;
    m_isHighFind = false;
    m_updateViewDataStruct = item;
}

MyTableModel::~MyTableModel()
{

    delete m_datasSearch;
    delete m_datasExtend;
    delete m_datasTemp;
}

QVariant MyTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    //注意，如果用了sortproxymodel，这个section是实际数据的index，不是界面看到的index
    //区分横表头和竖表头
    if(orientation == Qt::Horizontal){
        //这里我们只设置居中对齐和文本
        if (role == Qt::DisplayRole){
            //这里把横项列表头的文本设计为可以设置的
            if(section>=0 && section<g_ModelHeadL.count())
                return g_ModelHeadL.at(section);
            return QString("Col %1").arg(section + 1);
        }else if(role == Qt::TextAlignmentRole){
            return Qt::AlignCenter;
        }
    }else{
        if (role == Qt::DisplayRole)
            return QString("%1").arg(section + 1);
        else if(role == Qt::TextAlignmentRole)
            return Qt::AlignCenter;
    }
    return QVariant();
}
int MyTableModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    } else {
        return m_datas->count();  //显示数据的大小
    }
}

int MyTableModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    } else {
        return g_ModelHeadL.count();   //表头的大小
    }
}

QVariant MyTableModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    const int row=index.row();
    if(row>=m_datas->count())
        return QVariant();
    if(role == Qt::DisplayRole ){//显示数据
        switch(index.column()){

        case T_MFT: return m_datas->at(row)->MFTNumber;
        case T_PMFT: return m_datas->at(row)->PMFTNumber;
        case T_FILEATTRIBUTE: return m_datas->at(row)->bodyAtt.FileAttributes;
        case T_FILENAME:{
            if(m_datas->at(row)->MFTNumber != ROOT_DIRECTORY_MFT_NUMBER)
                return m_datas->at(row)->name;
            else{
                for(int i=0;i<MemoryPool::getInstance()->getInfoList()->count();++i){
                    if(m_datas->at(row)->partitionSerial == MemoryPool::getInstance()->getInfoList()->at(i).partitionSerial){
                        m_datas->at(row)->name = MemoryPool::getInstance()->getInfoList()->at(i).info.partitionFlagName.toUtf8();
                        break;
                    }
                }
                return m_datas->at(row)->name;
            }
        }
            break;
        case T_FILEPATH:{
            switch (m_datas->at(row)->bodyAtt.flag){
            case NOEXISTING_FILE_FLAG:{
                return "[已删除文件]"+findFilePath(m_datas->at(row)->partitionSerial,m_datas->at(row)->PMFTNumber);
            }
            case EXISTING_FILE_FLAG:{
                return findFilePath(m_datas->at(row)->partitionSerial,m_datas->at(row)->PMFTNumber);
            }
            case NOEXISTING_DIRECTORY_FLAG:{
                return "[已删除目录]"+findFilePath(m_datas->at(row)->partitionSerial,m_datas->at(row)->PMFTNumber);
            }
            case EXISTING_DIRECTORY_FLAG:{
                return findFilePath(m_datas->at(row)->partitionSerial,m_datas->at(row)->PMFTNumber);
            }
            default: {
                return findFilePath(m_datas->at(row)->partitionSerial,m_datas->at(row)->PMFTNumber);
            }
            }
        }
        case T_CREATIONTIME:return GlobalFunction::getInstance()->FILETIMEToDateTime(m_datas->at(row)->bodyAtt.creationTime);
        case T_CHANGETIME:return GlobalFunction::getInstance()->FILETIMEToDateTime(m_datas->at(row)->bodyAtt.changeTime);
        case T_LASTREADTIME:return GlobalFunction::getInstance()->FILETIMEToDateTime(m_datas->at(row)->bodyAtt.lastReadTime);
        case T_REALSIZE: {
            //实际大小
            if(m_datas->at(row)->bodyAtt.flag !=NOEXISTING_DIRECTORY_FLAG && m_datas->at(row)->bodyAtt.flag !=EXISTING_DIRECTORY_FLAG)//目录不显示大小
                return m_datas->at(row)->bodyAtt.fileRealSize;
            else if(m_datas->at(row)->MFTNumber == 0x5)
                return m_datas->at(row)->bodyAtt.fileRealSize;
            else
                return QVariant();
        }
        case T_ALLOCSIZE:{
            //占用空间
            if(m_datas->at(row)->bodyAtt.flag != NOEXISTING_DIRECTORY_FLAG && m_datas->at(row)->bodyAtt.flag != EXISTING_DIRECTORY_FLAG)//目录不显示大小
                return m_datas->at(row)->bodyAtt.fileAiiocSize;
            else if(m_datas->at(row)->MFTNumber == ROOT_DIRECTORY_MFT_NUMBER)
                return m_datas->at(row)->bodyAtt.fileAiiocSize;
            else
                return QVariant();
        }
        case T_FILETYPE:{
            return m_datas->at(row)->bodyAtt.flag;
        }
        case T_PARTITION:{
            return m_datas->at(row)->partitionSerial;
        }
        default:
            break;

        }
    }
    //显示icon
    if (role == Qt::DecorationRole){
        if(index.column() == T_FILENAME){
            if( m_datas->at(row)->MFTNumber == ROOT_DIRECTORY_MFT_NUMBER)
                return GlobalFunction::getInstance()->driveIcon();
            QIcon icon = getFileIcon(m_datas->at(row)->bodyAtt.flag,m_datas->at(row)->name);
            if(icon.isNull()){
                QIcon iconTemp = GlobalFunction::getInstance()->winFileIcon(findFilePath(m_datas->at(row)->partitionSerial,m_datas->at(row)->PMFTNumber)+m_datas->at(row)->name);
                if(iconTemp.isNull())
                    return GlobalFunction::getInstance()->fileIcon();
                return iconTemp;
            }
            return icon;
        }
    }

    if (role == Qt::TextAlignmentRole) {    //对其方式 --- 左对齐
        return left_VCenter;
    }
    return QVariant();

}
//获取文件图标icon
QIcon MyTableModel::getFileIcon(const quint8  flag,const QString  name) const
{
    switch (flag){
    case 0:{//已删除的文件
        if(name.contains(".")){//寻找带后缀文件
            QString temp = '.'+name.section(QChar('.'),-1).toLower();//取出后缀名
            if(!MemoryPool::getInstance()->getIconList()->contains(temp)){//不存在
                return GlobalFunction::getInstance()->fileIcon();
            }else{
                //存在
                QIcon icon =  MemoryPool::getInstance()->getIconList()->value(temp);
                if(icon.isNull())
                    return GlobalFunction::getInstance()->fileIcon();
                return icon;
            }
        }else{
            return GlobalFunction::getInstance()->fileIcon();
        }
    }
        break;
    case 1:{//文件
        if(name.contains(".")){//寻找带后缀文件
            QString temp = '.'+name.section(QChar('.'),-1).toLower();//取出后缀名
            if(!MemoryPool::getInstance()->getIconList()->contains(temp)){//不存在
                return GlobalFunction::getInstance()->fileIcon();
            }else{//存在
                return MemoryPool::getInstance()->getIconList()->value(temp);
            }
        }else{
            return GlobalFunction::getInstance()->fileIcon();
        }
    }
        break;
    case 2://已删除的目录
    {

    }
    case 3://目录
    {
        return GlobalFunction::getInstance()->folderIcon();
    }
        break;
    default:
        return GlobalFunction::getInstance()->fileIcon();
        break;
    }
}
//根据mft号查找路径
QByteArray MyTableModel::findFilePath(const quint8 serial,quint32 pMftNumber)  const
{
    QByteArray path="";
    S_DATA_BODY *body=NULL;
    QList<quint32> tempL;
    for(int i=0;i<MemoryPool::getInstance()->getInfoList()->count();++i){
        if(serial == MemoryPool::getInstance()->getInfoList()->at(i).partitionSerial){
            forever
            {
                if(tempL.contains(pMftNumber))
                    return "[错误文件]";
                if ((*MemoryPool::getInstance()->getInfoList())[i].dataBodys.contains(pMftNumber)){
                    tempL<<pMftNumber;
                    body = (*MemoryPool::getInstance()->getInfoList())[i].dataBodys.value(pMftNumber);
                    path.prepend(body->name + QString(QDir::separator()).toUtf8());
                    if(body->MFTNumber == body->PMFTNumber)
                        return path;
                    pMftNumber = body->PMFTNumber;
                }
                else
                    return "[无效路径]";
            }
        }
    }
    return path;
}

//初始化
void MyTableModel::initData()
{
    if(MemoryPool::getInstance()->getIsFinish())
        SlotDataChange(UpdataToDataBase);
    m_elapsedT.start();
}

//获取数据的数量
S_PARTITION_DATA_COUNT MyTableModel::getDataCount()
{
    S_PARTITION_DATA_COUNT countStruct;
    if(m_datas->count() == MemoryPool::getInstance()->getModelData()->count()){//显示完整数据，直接从更新记录直接读取，速度快些
        for(int i=0;i<MemoryPool::getInstance()->getInfoList()->count();i++){
            countStruct._falg_0_count += (*MemoryPool::getInstance()->getInfoList())[i].dataBodyCount._falg_0_count;
            countStruct._falg_1_count += (*MemoryPool::getInstance()->getInfoList())[i].dataBodyCount._falg_1_count;
            countStruct._falg_2_count += (*MemoryPool::getInstance()->getInfoList())[i].dataBodyCount._falg_2_count;
            countStruct._falg_3_count += (*MemoryPool::getInstance()->getInfoList())[i].dataBodyCount._falg_3_count;
            countStruct._falg_other_count += (*MemoryPool::getInstance()->getInfoList())[i].dataBodyCount._falg_other_count;
        }
    }else{
        for(int i=0;i<m_datas->count();i++){
            switch (m_datas->at(i)->bodyAtt.flag){//统计数量
            case NOEXISTING_FILE_FLAG:
                countStruct._falg_0_count++;
                break;
            case EXISTING_FILE_FLAG:
                countStruct._falg_1_count++;
                break;
            case NOEXISTING_DIRECTORY_FLAG:
                countStruct._falg_2_count++;
                break;
            case EXISTING_DIRECTORY_FLAG:
                countStruct._falg_3_count++;
                break;
            default:
                countStruct._falg_other_count++;

                break;
            }
        }
    }
    return countStruct;
}

//更新数据
void MyTableModel::SlotDataChange(E_DATABASE_TYPE type)
{
    switch (type)
    {
    //    case InitDataBase:
    //    {
    //        //        m_datasTemp->append(*MemoryPool::getInstance()->getModelData());
    //        //        sortData(*m_datasTemp,m_updateViewDataStruct);
    //        //        updataView();
    //        SlotSearchData(m_updateViewDataStruct);
    //    }
    //        break;
    case UpdataToDataBase:{
        if(m_isHighFind)
            SlotHighFindData(m_updateViewDataHighFindStruct);
        else
            SlotSearchData(m_updateViewDataStruct);
    }
        break;
    }

}

//更新界面
void MyTableModel::updataView()
{
    QModelIndexList from;
    QModelIndexList to;
    emit layoutAboutToBeChanged();
    m_datas = m_datasTemp;
    changePersistentIndexList(from,to);
    emit layoutChanged();
    emit SignalStatusbarUpdata(Status_DataCount,QVariant::fromValue(getDataCount()));

}

//按照启用项搜索数据
void MyTableModel::SlotSearchData(S_UPDATE_VIEW item)
{
    if(!MemoryPool::getInstance()->getIsFinish()){
        m_updateViewDataStruct = item;
        return;
    }
    m_isHighFind = false;
    QElapsedTimer t;
    t.start();
    QList<S_DATA_BODY*> tempTemp_1,tempTemp_2;//临时全部数据
    tempTemp_1.append(*MemoryPool::getInstance()->getModelData());

    searchData(tempTemp_1,item);
    sortData(tempTemp_1,item);
    m_datasTemp->swap(tempTemp_1);
    updataView();
    m_updateViewDataStruct = item;
}
void MyTableModel::searchData(QList<S_DATA_BODY*>& tempTemp_1,S_UPDATE_VIEW item)
{
    emit SignalStatusbarUpdata(Status_Text,"数据正在匹配中,请稍等 ...");
    QList<S_DATA_BODY*> tempTemp_2;//临时全部数据
    if(item.ExtendName != DEFAULT_ALL){//扩展名为全部
        QByteArray byteTemp =  item.ExtendName.toUtf8().toLower();
        //获取扩展名数据
        for(int i=0;i<tempTemp_1.count();++i){
            if(tempTemp_1.at(i)->name.toLower().endsWith(byteTemp) && (tempTemp_1.at(i)->bodyAtt.flag == EXISTING_FILE_FLAG || tempTemp_1.at(i)->bodyAtt.flag == NOEXISTING_FILE_FLAG))// 有后缀的只能是文件
                tempTemp_2.append(tempTemp_1.at(i));
        }
        tempTemp_1.swap(tempTemp_2);
        tempTemp_2.clear();
    }
    if(item.RegExp){//使用正则
        if(!item.searchName.isEmpty()){//搜索框非空
            QRegExp regExp;
            if(item.CaseInsensitive)//区分大小写
                regExp = QRegExp(item.searchName,Qt::CaseSensitive);
            else
                regExp = QRegExp(item.searchName,Qt::CaseInsensitive);
            if(!regExp.isValid())
                return;
            //获取搜索框数据
            for(int i=0;i<tempTemp_1.count();++i){
                if(QString( tempTemp_1.at(i)->name).contains(regExp))
                    tempTemp_2.append(tempTemp_1.at(i));
            }
            tempTemp_1.swap(tempTemp_2);
            tempTemp_2.clear();
        }
    }else {
        if(!item.searchName.isEmpty()){//搜索框非空
            QByteArray byteTemp;
            if(item.CaseInsensitive)//区分大小写
                byteTemp = item.searchName.toUtf8();
            else
                byteTemp = item.searchName.toUtf8().toLower();
            //获取搜索框数据
            for(int i=0;i<tempTemp_1.count();++i){
                if(item.Complete){//前置完全匹配
                    if(item.CaseInsensitive){//区分大小写
                        if(tempTemp_1.at(i)->name.startsWith(byteTemp))
                            tempTemp_2.append(tempTemp_1.at(i));
                    }else{
                        if(tempTemp_1.at(i)->name.toLower().startsWith(byteTemp))
                            tempTemp_2.append(tempTemp_1.at(i));
                    }
                }else{
                    if(item.CaseInsensitive){//区分大小写
                        if(tempTemp_1.at(i)->name.contains(byteTemp))
                            tempTemp_2.append(tempTemp_1.at(i));
                    } else {
                        if(tempTemp_1.at(i)->name.toLower().contains(byteTemp))
                            tempTemp_2.append(tempTemp_1.at(i));
                    }
                }
            }
            tempTemp_1.swap(tempTemp_2);
            tempTemp_2.clear();
        }
    }
}
void MyTableModel::sortData(QList<S_DATA_BODY*>& tempTemp_1,S_UPDATE_VIEW item)
{
    emit SignalStatusbarUpdata(Status_Text,"数据正在排序中,请稍等 ...");
    bool reverse = false;
    if(item.sortOrder != m_updateViewDataStruct.sortOrder && item.sortColumn == m_updateViewDataStruct.sortColumn && tempTemp_1.count() == m_datas->count())//和上次的排序的列位置一样，直接翻转数据，
        reverse = true;
    bool sortOrder = item.sortOrder == Qt::AscendingOrder ? true : false; // 升序为true

    switch (item.sortColumn){
    case T_MFT:{

        if(sortOrder){//升序
            if(reverse)
                reverseData(&tempTemp_1);
            else {
                pdqsort(tempTemp_1.begin(),tempTemp_1.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
                {
                    return infoA->MFTNumber < infoB->MFTNumber;//从小到大
                });
            }


        }else{//降序
            if(reverse)
                reverseData(&tempTemp_1);
            else {
                pdqsort(tempTemp_1.begin(),tempTemp_1.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
                {
                    return infoA->MFTNumber > infoB->MFTNumber;//从大到小
                });
            }
        }
    }
        break;
    case T_PMFT:{
        if(sortOrder){//升序
            if(reverse)
                reverseData(&tempTemp_1);
            else{
                pdqsort(tempTemp_1.begin(),tempTemp_1.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
                {
                    return infoA->PMFTNumber < infoB->PMFTNumber;//从小到大
                });
            }

        }else{//降序
            if(reverse)
                reverseData(&tempTemp_1);
            else{
                pdqsort(tempTemp_1.begin(),tempTemp_1.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
                {
                    return infoA->PMFTNumber > infoB->PMFTNumber;//从大到小
                });
            }

        }
    }
        break;
    case T_FILEATTRIBUTE:{
        if(sortOrder){//升序
            if(reverse)
                reverseData(&tempTemp_1);
            else{
                pdqsort(tempTemp_1.begin(),tempTemp_1.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
                {
                    return infoA->bodyAtt.FileAttributes < infoB->bodyAtt.FileAttributes ;//从小到大
                });
            }


        }else{//降序
            if(reverse)
                reverseData(&tempTemp_1);
            else{
                pdqsort(tempTemp_1.begin(),tempTemp_1.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
                {
                    return infoA->bodyAtt.FileAttributes > infoB->bodyAtt.FileAttributes ;//从小到大
                });
            }

        }
    }
        break;
    case T_FILENAME:{
        if(sortOrder){//升序
            if(reverse)
                reverseData(&tempTemp_1);
            else
                GlobalFunction::getInstance()->sort3Ways_wchar_asc(&tempTemp_1);
        }else{//降序
            if(reverse)
                reverseData(&tempTemp_1);
            else
                GlobalFunction::getInstance()->sort3Ways_wchar_des(&tempTemp_1);
        }
    }
        break;
    case T_FILEPATH://路径不支持
    {

        emit SignalStatusbarUpdata(Status_Text,"文件路径不支持排序,请更换其他列.");
        if(sortOrder)//升序
        {
            //            std::sort(tempTemp_1.begin(),tempTemp_1.end(),[=]( DATA_BODY* infoA,  DATA_BODY* infoB)
            //            {
            //                return infoA->MFTNumber < infoB->MFTNumber;//从小到大
            //            });

        }
        else//降序
        {
            //            std::sort(tempTemp_1.begin(),tempTemp_1.end(),[=]( DATA_BODY* infoA,  DATA_BODY* infoB)
            //            {
            //                return infoA->MFTNumber > infoB->MFTNumber;//从大到小
            //            });

        }
    }
        break;
    case T_CREATIONTIME:{
        if(sortOrder){//升序
            if(reverse)
                reverseData(&tempTemp_1);
            else{
                pdqsort(tempTemp_1.begin(),tempTemp_1.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
                {
                    return infoA->bodyAtt.creationTime < infoB->bodyAtt.creationTime;//从小到大
                });
            }

        }else{//降序
            if(reverse)
                reverseData(&tempTemp_1);
            else{
                pdqsort(tempTemp_1.begin(),tempTemp_1.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
                {
                    return infoA->bodyAtt.creationTime > infoB->bodyAtt.creationTime;//从大到小
                });
            }
        }
    }
        break;
    case T_CHANGETIME:{
        if(sortOrder){//升序
            if(reverse)
                reverseData(&tempTemp_1);
            else{
                pdqsort(tempTemp_1.begin(),tempTemp_1.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
                {
                    return infoA->bodyAtt.changeTime < infoB->bodyAtt.changeTime;//从小到大
                });
            }

        }else{//降序
            if(reverse)
                reverseData(&tempTemp_1);
            else{
                pdqsort(tempTemp_1.begin(),tempTemp_1.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
                {
                    return infoA->bodyAtt.changeTime > infoB->bodyAtt.changeTime;//从大到小
                });
            }

        }
    }
        break;
    case T_LASTREADTIME:{
        if(sortOrder){//升序
            if(reverse)
                reverseData(&tempTemp_1);
            else{
                pdqsort(tempTemp_1.begin(),tempTemp_1.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
                {
                    return infoA->bodyAtt.lastReadTime < infoB->bodyAtt.lastReadTime;//从小到大
                });
            }

        }else{//降序
            if(reverse)
                reverseData(&tempTemp_1);
            else{
                pdqsort(tempTemp_1.begin(),tempTemp_1.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
                {
                    return infoA->bodyAtt.lastReadTime > infoB->bodyAtt.lastReadTime;//从大到小
                });
            }
        }
    }
        break;
    case T_REALSIZE:{
        if(sortOrder){//升序
            if(reverse)
                reverseData(&tempTemp_1);
            else{
                pdqsort(tempTemp_1.begin(),tempTemp_1.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
                {
                    return infoA->bodyAtt.fileRealSize < infoB->bodyAtt.fileRealSize;//从小到大
                });
            }

        }else{//降序
            if(reverse)
                reverseData(&tempTemp_1);
            else{
                pdqsort(tempTemp_1.begin(),tempTemp_1.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
                {
                    return infoA->bodyAtt.fileRealSize > infoB->bodyAtt.fileRealSize;//从大到小
                });
            }

        }
    }
        break;
    case T_ALLOCSIZE:{
        if(sortOrder){//升序
            if(reverse)
                reverseData(&tempTemp_1);
            else{
                pdqsort(tempTemp_1.begin(),tempTemp_1.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
                {
                    return infoA->bodyAtt.fileAiiocSize < infoB->bodyAtt.fileAiiocSize;//从小到大
                });
            }

        }else{//降序
            if(reverse)
                reverseData(&tempTemp_1);
            else{
                pdqsort(tempTemp_1.begin(),tempTemp_1.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
                {
                    return infoA->bodyAtt.fileAiiocSize > infoB->bodyAtt.fileAiiocSize;//从大到小
                });
            }

        }
    }
        break;
    case T_FILETYPE:{
        if(sortOrder){//升序
            if(reverse)
                reverseData(&tempTemp_1);
            else{
                pdqsort(tempTemp_1.begin(),tempTemp_1.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
                {
                    return infoA->bodyAtt.flag < infoB->bodyAtt.flag;//从小到大
                });
            }

        }else{//降序
            if(reverse)
                reverseData(&tempTemp_1);
            else{
                pdqsort(tempTemp_1.begin(),tempTemp_1.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
                {
                    return infoA->bodyAtt.flag > infoB->bodyAtt.flag;//从大到小
                });
            }
        }
    }
        break;
    case T_PARTITION:{
        if(sortOrder){//升序
            if(reverse)
                reverseData(&tempTemp_1);
            else{
                pdqsort(tempTemp_1.begin(),tempTemp_1.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
                {
                    return infoA->partitionSerial < infoB->partitionSerial;//从小到大
                });
            }

        }else{//降序
            if(reverse)
                reverseData(&tempTemp_1);
            else{
                pdqsort(tempTemp_1.begin(),tempTemp_1.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
                {
                    return infoA->partitionSerial > infoB->partitionSerial;//从大到小
                });
            }

        }
    }
        break;
    default:
        break;

    }
}
//重写排序
void MyTableModel::SlotSortData(S_UPDATE_VIEW item)//SlotSort(int column, Qt::SortOrder ord)
{

    if(!MemoryPool::getInstance()->getIsFinish()){
        m_updateViewDataStruct = item;
        return;
    }
    QList<S_DATA_BODY*> tempTemp_1;//临时全部数据

    tempTemp_1.clear();
    tempTemp_1.reserve(m_datas->count());
    tempTemp_1.append(*m_datas);
    sortData(tempTemp_1,item);
    m_datasTemp->swap(tempTemp_1);
    updataView();
    m_updateViewDataStruct = item;

}
//高级搜索数据
void MyTableModel::SlotHighFindData(S_HIGHFIND_UPDATE_VIEW item)
{
    if(!MemoryPool::getInstance()->getIsFinish())
        return;
    m_updateViewDataHighFindStruct = item;
    m_isHighFind = true;
    emit SignalStatusbarUpdata(Status_Text,"数据正在匹配中,请稍等 ...");
    QElapsedTimer t;
    t.start();
    QList<S_DATA_BODY*> tempTemp_1,tempTemp_2;//临时全部数据
    tempTemp_1.append(*MemoryPool::getInstance()->getModelData());
    //获取盘符相同所有文件
    if(item.partitionSerial != -1){
        for(int i=0;i<tempTemp_1.count();++i){
            if(tempTemp_1.at(i)->partitionSerial == item.partitionSerial)
                tempTemp_2.append(tempTemp_1.at(i));
        }
        tempTemp_1.swap(tempTemp_2);
        tempTemp_2.clear();
    }
    //获取类型相同所有文件
    if(item.fileFlag != -1){
        for(int i=0;i<tempTemp_1.count();++i){
            if(item.fileFlag == 4){//系统文件
                if(tempTemp_1.at(i)->bodyAtt.flag >= item.fileFlag)
                    tempTemp_2.append(tempTemp_1.at(i));
            }else if(tempTemp_1.at(i)->bodyAtt.flag == item.fileFlag){
                tempTemp_2.append(tempTemp_1.at(i));
            }
        }
        tempTemp_1.swap(tempTemp_2);
        tempTemp_2.clear();
    }
    //获取mft号相同所有文件
    if(item.mft != -1){
        for(int i=0;i<tempTemp_1.count();++i){
            if(tempTemp_1.at(i)->MFTNumber == item.mft)
                tempTemp_2.append(tempTemp_1.at(i));
        }
        tempTemp_1.swap(tempTemp_2);
        tempTemp_2.clear();
    }
    //获取pMft号相同所有文件
    if(item.pMft != -1){
        for(int i=0;i<tempTemp_1.count();++i){
            if(tempTemp_1.at(i)->PMFTNumber == item.pMft)
                tempTemp_2.append(tempTemp_1.at(i));
        }
        tempTemp_1.swap(tempTemp_2);
        tempTemp_2.clear();
    }
    //获取属性相同所有文件
    if(item.fileAtt != -1){
        for(int i=0;i<tempTemp_1.count();++i){
            if(tempTemp_1.at(i)->bodyAtt.FileAttributes == item.fileAtt)
                tempTemp_2.append(tempTemp_1.at(i));
        }
        tempTemp_1.swap(tempTemp_2);
        tempTemp_2.clear();
    }

    //获取文件大小范围
    if(item.fileRealRange.leftValue != 0){
        for(int i=0;i<tempTemp_1.count();++i){
            if(tempTemp_1.at(i)->bodyAtt.fileRealSize >= item.fileRealRange.leftValue)
                tempTemp_2.append(tempTemp_1.at(i));
        }
        tempTemp_1.swap(tempTemp_2);
        tempTemp_2.clear();
    }
    if(item.fileRealRange.rightValue != (quint64)-1){
        for(int i=0;i<tempTemp_1.count();++i){
            if(tempTemp_1.at(i)->bodyAtt.fileRealSize < item.fileRealRange.rightValue)
                tempTemp_2.append(tempTemp_1.at(i));
        }
        tempTemp_1.swap(tempTemp_2);
        tempTemp_2.clear();
    }
    //获取文件创建时间范围
    if(item.fileCreationTimeRange.leftValue != 0){
        for(int i=0;i<tempTemp_1.count();++i){
            if(tempTemp_1.at(i)->bodyAtt.creationTime >= item.fileCreationTimeRange.leftValue)
                tempTemp_2.append(tempTemp_1.at(i));
        }
        tempTemp_1.swap(tempTemp_2);
        tempTemp_2.clear();
    }
    if(item.fileCreationTimeRange.rightValue != (quint64)-1){
        for(int i=0;i<tempTemp_1.count();++i){
            if(tempTemp_1.at(i)->bodyAtt.creationTime < item.fileCreationTimeRange.rightValue)
                tempTemp_2.append(tempTemp_1.at(i));
        }
        tempTemp_1.swap(tempTemp_2);
        tempTemp_2.clear();
    }
    //获取文件修改时间范围
    if(item.fileChangeTimeRange.leftValue != 0){
        for(int i=0;i<tempTemp_1.count();++i){
            if(tempTemp_1.at(i)->bodyAtt.changeTime >= item.fileChangeTimeRange.leftValue)
                tempTemp_2.append(tempTemp_1.at(i));
        }
        tempTemp_1.swap(tempTemp_2);
        tempTemp_2.clear();
    }
    if(item.fileChangeTimeRange.rightValue != (quint64)-1){
        for(int i=0;i<tempTemp_1.count();++i){
            if(tempTemp_1.at(i)->bodyAtt.changeTime < item.fileChangeTimeRange.rightValue)
                tempTemp_2.append(tempTemp_1.at(i));
        }
        tempTemp_1.swap(tempTemp_2);
        tempTemp_2.clear();
    }
    //获取文件访问时间范围
    if(item.fileReadTimeRange.leftValue != 0){
        for(int i=0;i<tempTemp_1.count();++i){
            if(tempTemp_1.at(i)->bodyAtt.lastReadTime >= item.fileReadTimeRange.leftValue)
                tempTemp_2.append(tempTemp_1.at(i));
        }
        tempTemp_1.swap(tempTemp_2);
        tempTemp_2.clear();
    }
    if(item.fileReadTimeRange.rightValue != (quint64)-1){
        for(int i=0;i<tempTemp_1.count();++i) {
            if(tempTemp_1.at(i)->bodyAtt.lastReadTime < item.fileReadTimeRange.rightValue)
                tempTemp_2.append(tempTemp_1.at(i));
        }
        tempTemp_1.swap(tempTemp_2);
        tempTemp_2.clear();
    }


    S_UPDATE_VIEW item_1;
    item_1.ExtendName = item.ExtendName;
    item_1.searchName = item.searchName;
    item_1.CaseInsensitive = item.CaseInsensitive;
    item_1.RegExp = item.RegExp;
    item_1.Complete = item.Complete;

    searchData(tempTemp_1,item_1);
    sortData(tempTemp_1,m_updateViewDataStruct);
    m_datasTemp->swap(tempTemp_1);
    updataView();
    qDebug()<<"高级搜索时间:"<<t.elapsed();
}
//导出文件列表
void MyTableModel::SlotExportFileList(const QString &fileName)
{
    //另开一个线程导出
    QtConcurrent::run([=]()
    {
        QT_TRY
        {
            QList<S_DATA_BODY*> exportData;
            exportData.reserve(m_datas->count());
            exportData.append(*m_datas);
            QFile file(fileName);
            if(file.open(QIODevice::WriteOnly | QIODevice::Append)){
                QTextStream text_stream(&file);
                emit SignalStatusbarUpdata(Status_ExportDataStart,exportData.count());//发送数据总数——初始化进度条
                //导出文件格式xlsx- -最后保存压缩时间超长，不建议保存xlsx格式。可以先保存csv格式，用Excel转换方便
                if(fileName.endsWith(".xlsx",Qt::CaseInsensitive)) {
                    QXlsx::Document xlsx;
                    for(int i=0;i<g_ModelHeadL.count();i++)
                        xlsx.write(1,i+1,g_ModelHeadL.at(i));//设置标题
                    for(qint64 i=0;i<exportData.count();i++){

                        xlsx.write(i+2,T_MFT+1,exportData.at(i)->MFTNumber);
                        xlsx.write(i+2,T_PMFT+1,exportData.at(i)->PMFTNumber);
                        xlsx.write(i+2,T_FILEATTRIBUTE+1,exportData.at(i)->bodyAtt.FileAttributes);
                        xlsx.write(i+2,T_FILENAME+1,QString(exportData.at(i)->name));

                        QString filePath="";
                        switch (exportData.at(i)->bodyAtt.flag){
                        case NOEXISTING_FILE_FLAG:
                        {
                            filePath = QString("[已删除文件]")+findFilePath(exportData.at(i)->partitionSerial,exportData.at(i)->PMFTNumber);
                        }
                            break;
                        case EXISTING_FILE_FLAG:
                        {
                            filePath =  findFilePath(exportData.at(i)->partitionSerial,exportData.at(i)->PMFTNumber);
                        }
                            break;
                        case NOEXISTING_DIRECTORY_FLAG:
                        {
                            filePath = QString("[已删除目录]")+findFilePath(exportData.at(i)->partitionSerial,exportData.at(i)->PMFTNumber);
                        }
                            break;
                        case EXISTING_DIRECTORY_FLAG:
                        {
                            filePath =  findFilePath(exportData.at(i)->partitionSerial,exportData.at(i)->PMFTNumber);
                        }
                            break;
                        default:
                        {
                            filePath =  findFilePath(exportData.at(i)->partitionSerial,exportData.at(i)->PMFTNumber);
                        }
                        }

                        xlsx.write(i+2,T_FILEPATH+1,filePath);
                        xlsx.write(i+2,T_CREATIONTIME+1,GlobalFunction::getInstance()->FILETIMEToDateTime(exportData.at(i)->bodyAtt.creationTime));
                        xlsx.write(i+2,T_CHANGETIME+1,GlobalFunction::getInstance()->FILETIMEToDateTime(exportData.at(i)->bodyAtt.changeTime));
                        xlsx.write(i+2,T_LASTREADTIME+1,GlobalFunction::getInstance()->FILETIMEToDateTime(exportData.at(i)->bodyAtt.lastReadTime));
                        xlsx.write(i+2,T_REALSIZE+1,exportData.at(i)->bodyAtt.fileRealSize);
                        xlsx.write(i+2,T_ALLOCSIZE+1,exportData.at(i)->bodyAtt.fileAiiocSize);
                        xlsx.write(i+2,T_FILETYPE+1,exportData.at(i)->bodyAtt.flag);
                        if(i%100 == 0)
                            emit SignalStatusbarUpdata(Status_ExportDataCount,i);

                    }
                    xlsx.saveAs(&file);

                } else {
                    /* .csv .text
             * 执行下面
             */
                    //首次写入标题
                    QByteArray array="";
                    for(int i=0;i<g_ModelHeadL.count();i++){
                        array.append(g_ModelHeadL.at(i));
                        if((i+1)!= g_ModelHeadL.count())
                            array.append(',');
                        else
                            array.append('\n');
                    }
                    //file.write(array);
                    text_stream << array;//将任何缓存数据刷新到文件中
                    array.clear();
                    for(qint64 i=0;i<exportData.count();i++) {
                        array.append(QByteArray::number(exportData.at(i)->MFTNumber));
                        array.append(',');
                        array.append(QByteArray::number(exportData.at(i)->PMFTNumber));
                        array.append(',');
                        array.append(QByteArray::number(exportData.at(i)->bodyAtt.FileAttributes));
                        array.append(',');
                        array.append(exportData.at(i)->name);
                        array.append(',');
                        QByteArray filePath="";
                        switch (exportData.at(i)->bodyAtt.flag){
                        case NOEXISTING_FILE_FLAG:
                        {
                            filePath = QByteArray("[已删除文件]")+findFilePath(exportData.at(i)->partitionSerial,exportData.at(i)->PMFTNumber);
                        }
                            break;
                        case EXISTING_FILE_FLAG:
                        {
                            filePath =  findFilePath(exportData.at(i)->partitionSerial,exportData.at(i)->PMFTNumber);
                        }
                            break;
                        case NOEXISTING_DIRECTORY_FLAG:
                        {
                            filePath =  QByteArray("[已删除目录]")+findFilePath(exportData.at(i)->partitionSerial,exportData.at(i)->PMFTNumber);
                        }
                            break;
                        case EXISTING_DIRECTORY_FLAG:
                        {
                            filePath =  findFilePath(exportData.at(i)->partitionSerial,exportData.at(i)->PMFTNumber);
                        }
                            break;
                        default:
                        {
                            filePath =  findFilePath(exportData.at(i)->partitionSerial,exportData.at(i)->PMFTNumber);
                        }
                        }
                        array.append(filePath);
                        array.append(',');
                        array.append(GlobalFunction::getInstance()->FILETIMEToDateTime(exportData.at(i)->bodyAtt.creationTime));
                        array.append(',');
                        array.append(GlobalFunction::getInstance()->FILETIMEToDateTime(exportData.at(i)->bodyAtt.changeTime));
                        array.append(',');
                        array.append(GlobalFunction::getInstance()->FILETIMEToDateTime(exportData.at(i)->bodyAtt.lastReadTime));
                        array.append(',');
                        array.append(QByteArray::number(exportData.at(i)->bodyAtt.fileRealSize));
                        array.append(',');
                        array.append(QByteArray::number(exportData.at(i)->bodyAtt.fileAiiocSize));
                        array.append(',');
                        array.append(QByteArray::number(exportData.at(i)->bodyAtt.flag));
                        array.append('\n');
                        text_stream << array;//将任何缓存数据刷新到文件中
                        array.clear();
                        if(i%100 == 0)
                            emit SignalStatusbarUpdata(Status_ExportDataCount,i);

                    }
                    file.close();
                }
                emit SignalStatusbarUpdata(Status_ExportDataEnd,exportData.count());//发送数据总数——初始化进度条
            }
            else
                emit SignalStatusbarUpdata(Status_ExportDataEnd,0);//导出失败
        }
        QT_CATCH(...)
        {
            emit SignalStatusbarUpdata(Status_ExportDataEnd,0);//导出失败
        }
    });
}
//翻转数据
void MyTableModel::reverseData(QList<S_DATA_BODY*>* dataL)
{
    QList<S_DATA_BODY*> temp;
    int j = dataL->count();
    temp.reserve(j);

    for(int i=0;i<dataL->count();i++){
        temp.append(dataL->at(--j));
    }
    dataL->swap(temp);
}
