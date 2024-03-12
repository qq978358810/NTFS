#include "ntfssysinfo.h"
#include <QDebug>
#include <QThread>
#include <QElapsedTimer>
#include <QProcess>
CNtfsInfo::CNtfsInfo(S_NTFS_INFO ntfsInfo,QObject *parent) : CNTFS(parent)
{
    m_fileHandle = new QFile();
    m_fileHandle->setFileName(ntfsInfo.pDrive);//设置要打开的文件

    m_initDatasInfo.info = ntfsInfo;
    m_NTFSOffset = ntfsInfo.bytesOffset;//开始偏移位置
    m_partitionName = ntfsInfo.partitionFlagName;
    m_partitionuuid = ntfsInfo.uuid;
    if(ntfsInfo.partitionFlagName.count()>0)
        m_name_uuid = ntfsInfo.partitionFlagName.at(0) + ntfsInfo.uuid.toString();
    m_name_uuid += SPLIT_SYMBOLS;
    m_threadRunFlag = false;

    m_usnRecordUnion = (PUSN_RECORD_UNION)(malloc(MAX_USN_REOCRD_SIZE));
    qDebug()<<m_name_uuid<<"构造完成";

}

CNtfsInfo::~CNtfsInfo()
{
    m_fileHandle->close();
    qDebug()<<m_name_uuid<<"析构~";
}

bool CNtfsInfo::openNTFS()
{
    if(!m_fileHandle->isOpen()){
        if(!m_fileHandle->open(QIODevice::ReadOnly | QIODevice::ExistingOnly)){
            qDebug()<<m_name_uuid<<"文件打开失败!";
            return false;
        }
    }
    //qDebug()<<m_name_uuid<<"文件打开成功!";
    return true;
}

//初始化
bool CNtfsInfo::initDrive()
{
    if(!openNTFS())
        return false;
    // _1
    S_NTFS_BOOT bootH;
    QByteArray bootByte = getRawData(m_NTFSOffset,sizeof (S_NTFS_BOOT));
    if(bootByte.isNull()) {
        qDebug()<<m_name_uuid<<"初始化失败!";
        return false;
    }

    bool isOk = NTFS_BootHeader(bootH,bootByte);
    if(!isOk) {
        qDebug()<<m_name_uuid<<"初始化失败!";
        return false;
    }
    quint64 mftStart = NTFS_firstMFTOffset(bootH);
    QByteArray mftByte = getRawData(m_NTFSOffset + mftStart,BYTES_PER_MFT_FILE);
    if(mftByte.isNull()) {
        qDebug()<<m_name_uuid<<"初始化失败!";
        return false;
    }
    S_MFT_INFO mftInfo;
    bool mftInfoIsOk = NTFS_MFTFileInfo(mftInfo,mftByte);
    if(!mftInfoIsOk || mftInfo._x80.isEmpty() || mftInfo._x80.at(0).first.NonResidentFlag != 1){
        qDebug()<<m_name_uuid<<"初始化失败!";
        return false;
    }
    QVariant varData = mftInfo._x80.at(0).second.data;
    if (!varData.canConvert<QList<S_DATARUN>>()){
        qDebug()<<m_name_uuid<<"初始化失败!";
        return false;
    }
    QList<S_DATARUN> dataL = varData.value<QList<S_DATARUN>>();
    if(dataL.count()==0){
        qDebug()<<m_name_uuid<<"初始化失败!";
        return false;
    }
    QList<S_MFT0_0X80_DATARUN> mftTable = NTFS_MFTTable(dataL);
    m_tempMftTable = mftTable;
    //qDebug()<<m_name_uuid<<"初始化成功!";
    return true;
}
//获取卷名
QString CNtfsInfo::getmVolumeName()
{
    quint64 mftOffset = getMTFOffset($Volume);
    QByteArray mftByte_1 = getRawData(m_NTFSOffset + mftOffset,BYTES_PER_MFT_FILE);
    if(mftByte_1.isNull()) {
        qDebug()<<m_name_uuid<<"获取卷名失败!";
        return "";
    }
    S_MFT_INFO mftInfo_1 ;
    bool mftInfo_1IsOk = NTFS_MFTFileInfo(mftInfo_1,mftByte_1);
    if(!mftInfo_1IsOk) {
        qDebug()<<m_name_uuid<<"获取卷名失败!";
        return "";
    }
    if(mftInfo_1._x60.isEmpty()) {
        qDebug()<<m_name_uuid<<"获取卷名失败!";
        return "";
    }
    qDebug()<<m_name_uuid<<"获取卷名成功Name:"+mftInfo_1._x60.at(0).second.name;
    return  mftInfo_1._x60.at(0).second.name;
}
//获取mft偏移
quint64 CNtfsInfo::getMTFOffset(MFTNUM MTFNumber)
{
    quint64 mftOffset = NTFS_MTFOffset(MTFNumber,m_mftTable);
    if(mftOffset == 0) {
        int whileCount = 100;
        qDebug()<<m_name_uuid<<"重新获取Mft偏移...";
        do{
            if(initDrive()) {
                m_mftTable = m_tempMftTable;
                m_volumeName = getmVolumeName();
                mftOffset = NTFS_MTFOffset(MTFNumber,m_mftTable);
                if(mftOffset == 0)
                    QThread::sleep(1);
                else{
                    qDebug()<<m_name_uuid<<"重新获取Mft偏移:"+QString::number(mftOffset);
                    return  mftOffset;
                }
            } else {
                QT_THROW(0);
            }

        }while(whileCount--);
        qDebug()<<m_name_uuid<<"获取mft: ["<<MTFNumber<<"]失败,抛出异常!";
        QT_THROW(0);
    }
    return  mftOffset;
}

//通过bitmap获取磁盘已经使用的空间大小 -- 存在和Windows API获取不一致，偏小
quint64 CNtfsInfo::usedSpaceSize()
{
    quint64 offset = getMTFOffset(0x6);
    QByteArray mftByte = getRawData(m_NTFSOffset + offset,BYTES_PER_MFT_FILE);
    if(mftByte.isNull()) {
        qDebug()<<m_name_uuid<<"获取磁盘已经使用的空间大小失败(bitmap)!";
        return 0;
    }
    S_MFT_INFO mftInfo;
    bool mftInfoIsOk = NTFS_MFTFileInfo(mftInfo,mftByte);
    if(!mftInfoIsOk || mftInfo._x80.isEmpty() || mftInfo._x80.at(0).first.NonResidentFlag != 1){
        qDebug()<<m_name_uuid<<"获取磁盘已经使用的空间大小失败(bitmap)!";
        return 0;
    }
    QVariant varData = mftInfo._x80.at(0).second.data;
    if (!varData.canConvert<QList<S_DATARUN>>()){
        qDebug()<<m_name_uuid<<"获取磁盘已经使用的空间大小失败(bitmap)!";
        return 0;
    }
    QList<S_DATARUN> source = varData.value<QList<S_DATARUN>>();
    if(source.count()==0) {
        qDebug()<<m_name_uuid<<"获取磁盘已经使用的空间大小失败(bitmap)!";
        return 0;
    }

    quint32 fileClusterSize= 0;//系统已使用的簇数
    quint64 clusterOffset = 0;//bitmap数据区偏移

    QScopedPointer<char, QScopedPointerPodDeleter> tempRawdata((char *)(malloc(BYTES_PER_CLUSTER)));//保存每簇bitmap数据区
    if(tempRawdata.isNull()){
        qDebug()<<m_name_uuid<<"获取磁盘已经使用的空间大小失败(bitmap)!";
        return 0;
    }

    for(int i=0;i<source.count();i++){//通常只有一个
        quint64 dataOffset = m_NTFSOffset + source.at(i).dataStartCluster * BYTES_PER_CLUSTER;//获取bitmap数据区偏移地址
        while(clusterOffset<source.at(i).datalengthCluster) {
            //获取bitmap区数据

            if(!m_fileHandle->seek(dataOffset + clusterOffset * BYTES_PER_CLUSTER))//跳转地址失败-报错
                return false;
            qint64 tempRawdataSize = m_fileHandle->read(tempRawdata.data(),BYTES_PER_CLUSTER);
            if(tempRawdataSize != BYTES_PER_CLUSTER)//数据读取不完整 -报错
                return false;
            for(quint32 ii=0;ii<BYTES_PER_CLUSTER;ii++){//从正确的位置读取字节
                quint8 byte = (quint8)tempRawdata.data()[ii];
                for(int j=0;j<8;j++) {
                    if(byte>>j & 1)//判断每一位是否为1
                        fileClusterSize++;
                }
            }
            clusterOffset++;
        }
        clusterOffset=0;
    }
    qDebug()<<m_name_uuid<<"获取磁盘已经使用的空间大小(bitmap):"+QString::number(fileClusterSize * BYTES_PER_CLUSTER);
    return fileClusterSize * BYTES_PER_CLUSTER;
}
//从offset位置获取size大小数据
QByteArray CNtfsInfo::getRawData(quint64 offset,qint32 size)
{
    if(!m_fileHandle->seek(offset))
        return QByteArray();
    QByteArray rawData = m_fileHandle->read(size);
    if(rawData.size() != size)
        return QByteArray();
    return rawData;
}
//获取文件的基本信息
bool CNtfsInfo::getMFTBasicInfo(MFT_BASIC_INFO& basicInfo,QByteArray byteData)
{
    S_FILE_RECORD fileRecordHead;//文件头
    bool isOk = NTFS_MFTFileHeader(fileRecordHead,byteData);
    if(!isOk)
        return false;

    if(basicInfo.ExMFT != fileRecordHead.BaseFileRecord )//初始PMFT等于0；当mft扩展 不为0，直接返回；递归调用PMFT和当前mft扩展相等---遍历时防止多次调用同一个mft扩展
        return false;
    if(basicInfo.ExMFT == 0)//使用同一个mft（扩展mft为0的 mft）
        basicInfo.ExMFT = fileRecordHead.MFTRecordNumber;
    basicInfo.attBody.flag = fileRecordHead.Flags;
    quint32 allLength = fileRecordHead.BytesInUse-4;//所有属性的大小
    quint32 byteOffset = fileRecordHead.AttributeOffset;
    int whileCount = allLength ;

    while(byteOffset<allLength){
        if((--whileCount) == 0){//死循环异常
            NTFS_setErrorType(CNTFS::WhileError);
            return false;
        }
        if(!(byteData.mid(byteOffset,sizeof (quint32)).toHex()).compare("FFFFFFFF",Qt::CaseInsensitive)) {
            return true;
        }
        S_NTFS_ATTRIBUTE_HEADER attH;//属性头
        qint8 NonResidentFlag = NTFS_MFTAtt(attH,byteData.mid(byteOffset,sizeof(S_NTFS_ATTRIBUTE_HEADER)));//获取属性 返回0常驻，1非常驻，-1：失败
        if(NonResidentFlag == -1)
            return false;

        switch(attH.AttributeType){
        //0x10
        case AttributeStandardInformation:
        {
            byteOffset += attH.NonResidentFlag==0 ? attH.CCommon.CResident.StreamOffset : attH.CCommon.CNonResident.RunListOffset;
            //0常驻
            if(attH.NonResidentFlag==0 && (attH.AttributeLength - attH.CCommon.CResident.StreamOffset)>0){
                S_ATTRIBUTE_0X10 x10;
                if(!NTFS_0X10Att(x10,byteData.mid(byteOffset,sizeof(S_ATTRIBUTE_0X10))))
                    return false;
                basicInfo.attBody.creationTime = x10.CreationTime;//获取文件创建时间
                basicInfo.attBody.changeTime = x10.ChangeTime;//获取文件最后修改时间
                basicInfo.attBody.lastReadTime = x10.LastReadTime;//获取文件最后读时间
                basicInfo.attBody.FileAttributes = x10.FileAttribute;//获取文件属性
                byteOffset += (attH.AttributeLength-attH.CCommon.CResident.StreamOffset);

            }
            //1非常驻
            else if(attH.NonResidentFlag==1 && (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset)>0)//预防
            {

                byteOffset += (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset);
            }
        }
            break;
            //0x20
        case AttributeAttributeList:
        {
            byteOffset += attH.NonResidentFlag==0 ? attH.CCommon.CResident.StreamOffset : attH.CCommon.CNonResident.RunListOffset;
            if(attH.NonResidentFlag==0 && (attH.AttributeLength - attH.CCommon.CResident.StreamOffset)>0){
                QList<S_ATTRIBUTE_0X20> x20L = NTFS_0X20Att(byteData.mid(byteOffset,attH.AttributeLength - attH.CCommon.CResident.StreamOffset));
                for(int i=0;i<x20L.count();i++){
                    QList<MFTNUM> _temp;
                    //只获取不同的mft 属性x10和x30和x80
                    if((x20L.at(i).AttributeType == AttributeStandardInformation || x20L.at(i).AttributeType == AttributeFileName || x20L.at(i).AttributeType == AttributeData )
                            &&  x20L.at(i).Fdir.FileReferenceNumber != fileRecordHead.MFTRecordNumber){
                        if(!_temp.contains(x20L.at(i).Fdir.FileReferenceNumber)){//以防多次执行，不同的mft只执行一次
                            _temp.append(x20L.at(i).Fdir.FileReferenceNumber);
                            quint64 offset = getMTFOffset(x20L.at(i).Fdir.FileReferenceNumber);
                            QByteArray data = getRawData(m_NTFSOffset + offset,BYTES_PER_MFT_FILE);
                            if(data.isNull())
                                return false;
                            bool isOk = getMFTBasicInfo(basicInfo,data);
                            if(!isOk)
                                return false;
                        }
                    }
                }
                byteOffset += (attH.AttributeLength-attH.CCommon.CResident.StreamOffset);
            }
            else if(attH.NonResidentFlag==1 && (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset)>0)//预防
            {
                QList<S_DATARUN> DRunList;
                DRunList = NTFS_DatasRun(byteData.mid(byteOffset,attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset));
                for(int i=0;i<DRunList.count();i++) {
                    QByteArray data = getRawData(m_NTFSOffset + DRunList[i].dataStartCluster * BYTES_PER_CLUSTER,attH.CCommon.CNonResident.StreamRealSize);
                    if(!data.isNull()){
                        QList<S_ATTRIBUTE_0X20> x20L = NTFS_0X20Att(data);
                        for(int i=0;i<x20L.count();i++){
                            QList<MFTNUM> _temp;
                            //只获取不同的mft 属性x10和x30和x80
                            if((x20L.at(i).AttributeType == AttributeStandardInformation || x20L.at(i).AttributeType == AttributeFileName || x20L.at(i).AttributeType == AttributeData )
                                    &&  x20L.at(i).Fdir.FileReferenceNumber != fileRecordHead.MFTRecordNumber){
                                if(!_temp.contains(x20L.at(i).Fdir.FileReferenceNumber)){//以防多次执行，不同的mft只执行一次
                                    _temp.append(x20L.at(i).Fdir.FileReferenceNumber);
                                    quint64 offset = getMTFOffset(x20L.at(i).Fdir.FileReferenceNumber);
                                    QByteArray data = getRawData(m_NTFSOffset + offset,BYTES_PER_MFT_FILE);
                                    if(data.isNull())
                                        return false;
                                    bool isOk = getMFTBasicInfo(basicInfo,data);
                                    if(!isOk)
                                        return false;
                                }
                            }
                        }
                    }
                }
                byteOffset += (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset);
            }
        }
            break;
            //x30
        case AttributeFileName:
        {
            byteOffset += attH.NonResidentFlag==0 ? attH.CCommon.CResident.StreamOffset : attH.CCommon.CNonResident.RunListOffset;
            if(attH.NonResidentFlag==0 && (attH.AttributeLength - attH.CCommon.CResident.StreamOffset)>0) {
                S_ATTRIBUTE_0X30 x30;
                QByteArray data = byteData.mid(byteOffset,attH.AttributeLength - attH.CCommon.CResident.StreamOffset);
                QByteArray dataName = NTFS_0X30AttName(x30, data);
                if(!dataName.isNull()) {
                    if(x30.NameType != 2){//忽略DOS命名空间
                        basicInfo.attBody.refCount++;//当前mft，包含refCount个文件
                        S_DATA_BODY* dataBody =new S_DATA_BODY;
                        if(fileRecordHead.MFTRecordNumber == ROOT_DIRECTORY_MFT_NUMBER){//mft == 5 ,根目录（盘符）MFTNumber在一个盘符中是唯一的
                            dataBody->name = m_partitionName.toUtf8();
                            dataBody->MFTNumber = fileRecordHead.MFTRecordNumber;//获取mft
                            dataBody->PMFTNumber = x30.Fdir.DirectoryFile;//获取pMft
                            basicInfo.attBody.fileAiiocSize = m_initDatasInfo.info.bytesTotal;
                            basicInfo.attBody.fileRealSize = usedSpaceSize();
                            basicInfo.fileL.append(dataBody);
                            return true;
                        }

                        dataBody->name = dataName;
                        dataBody->MFTNumber = basicInfo.ExMFT;//获取mft
                        dataBody->PMFTNumber = x30.Fdir.DirectoryFile;//获取pMft
                        basicInfo.fileL.append(dataBody);
                    }
                    byteOffset += (attH.AttributeLength-attH.CCommon.CResident.StreamOffset);
                }

            }
            else if(attH.NonResidentFlag==1 && (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset)>0)//预防
            {
                byteOffset += (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset);
            }

        }
            break;
            //0x80--存放文件的位置->获取文件的大小
        case AttributeData:
        {
            byteOffset += attH.NonResidentFlag==0 ? attH.CCommon.CResident.StreamOffset : attH.CCommon.CNonResident.RunListOffset;
            //常驻大小,不会超过1kb
            if(attH.NonResidentFlag==0 && (attH.AttributeLength - attH.CCommon.CResident.StreamOffset)>=0){
                if(basicInfo.attBody.fileAiiocSize == 0)//数据大小主要以非常驻为准，非常驻有数据就不获取常驻数据大小 -- 非常驻bodyAtt->fileAiiocSize一定是大于0的
                {
                    basicInfo.attBody.fileRealSize = attH.CCommon.CResident.StreamLength;
                    basicInfo.attBody.fileAiiocSize = 0;
                }
                byteOffset += (attH.AttributeLength-attH.CCommon.CResident.StreamOffset);
            }
            //非常驻大小
            else if(attH.NonResidentFlag==1 && (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset)>=0)
            {
                basicInfo.attBody.fileRealSize = attH.CCommon.CNonResident.StreamRealSize;
                basicInfo.attBody.fileAiiocSize = attH.CCommon.CNonResident.StreamAiiocSize;
                byteOffset += (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset);
            }
        }
            break;
        case 0xFFFFFFFF:
        {
            return true;
        }

        default:
            byteOffset += (attH.AttributeLength);
            break;
        }
    }
    return false;
}
//获取所有文件信息
void CNtfsInfo::getAllFilesBasicInfo()
{
    qDebug()<<m_name_uuid<<"获取所有文件信息Start!";
    //恢复默认值
    m_initDatasInfo.dataBodys.clear();
    m_initDatasInfo.dataBodyCount._falg_0_count=0;
    m_initDatasInfo.dataBodyCount._falg_1_count=0;
    m_initDatasInfo.dataBodyCount._falg_2_count=0;
    m_initDatasInfo.dataBodyCount._falg_3_count=0;
    m_initDatasInfo.dataBodyCount._falg_other_count=0;

    QElapsedTimer t;
    t.start();//将此时间设置为当前时间
    for(int i=0;i<m_mftTable.count();++i){
        quint64 byteOffset=0;
        quint64 startOffset = m_mftTable[i].dataStartByte;
        quint64 dataLength = m_mftTable[i].datalengthByte;
        while(byteOffset<dataLength && !QThread::currentThread()->isInterruptionRequested()){

            MFT_BASIC_INFO basicInfo;
            bool isOk=false;
            QByteArray byteData = getRawData(startOffset + m_NTFSOffset,BYTES_PER_MFT_FILE);
            if(!byteData.isNull())
                isOk = getMFTBasicInfo(basicInfo,byteData);
            startOffset += BYTES_PER_MFT_FILE;
            byteOffset += BYTES_PER_MFT_FILE;
            if(!isOk)
                continue;
            if(basicInfo.fileL.count()==1) {
                basicInfo.fileL.at(0)->bodyAtt = basicInfo.attBody;
                dataCountAdd(basicInfo.fileL.at(0)->bodyAtt.flag);
                m_initDatasInfo.dataBodys.insertMulti(basicInfo.fileL.at(0)->MFTNumber,basicInfo.fileL.at(0));
            }else if(basicInfo.fileL.count()>1){
                for(int i=0;i<basicInfo.fileL.count();i++){
                    basicInfo.fileL.at(i)->bodyAtt = basicInfo.attBody;
                    dataCountAdd(basicInfo.fileL.at(i)->bodyAtt.flag);
                    m_initDatasInfo.dataBodys.insertMulti(basicInfo.fileL.at(i)->MFTNumber,basicInfo.fileL.at(i));
                }
            }
        }
    }
    qDebug()<<m_name_uuid<<"获取所有文件信息End:"+QString::number(t.elapsed())+" ms --- Count:"<<m_initDatasInfo.dataBodys.count();

}
//获取保存usn记录文件夹的mft号
MFTNUM CNtfsInfo::getSaveUsnLogPMft(QString path)
{
    path = QDir::toNativeSeparators(path);
    QStringList tempL = path.split(QDir::separator(),QString::SkipEmptyParts);
    //Q_ASSERT_X(tempL.count(),"为空","异常");
    if(tempL.isEmpty())
        return -1;
    if(m_partitionName == tempL.at(0))//判断同一个盘符
    {
        QList<S_DATA_BODY*> tempBodyL;
        QHash<quint32,S_DATA_BODY*>::iterator iter =  m_initDatasInfo.dataBodys.begin();
        while (iter!=m_initDatasInfo.dataBodys.constEnd()) {
            //找出正常文件夹 && 文件名相同的所有项
            if(iter.value()->bodyAtt.flag == EXISTING_DIRECTORY_FLAG && tempL.last().toUtf8() == iter.value()->name)
                tempBodyL.append(iter.value());
            iter++;
        }
        for(int i=0;i<tempBodyL.count();i++) {
            MFTNUM pMft = tempBodyL.at(i)->PMFTNumber;
            //对每个文件夹遍历  找出目录深度相同，文件夹名字对应相同的
            for(int j=tempL.count()-2;j>0;j--){
                QHash<quint32,S_DATA_BODY*>::iterator iterP =  m_initDatasInfo.dataBodys.find(pMft);
                if(iterP == m_initDatasInfo.dataBodys.constEnd() || iterP.value()->name != tempL.at(j).toUtf8())
                    break;
                pMft = iterP.value()->PMFTNumber;
                if(pMft == ROOT_DIRECTORY_MFT_NUMBER && j == 1)//根目录--成立
                {
                    qDebug()<<m_name_uuid<<"获取日志文件夹MFTNumber:"+QString::number(tempBodyL.at(i)->MFTNumber);
                    return tempBodyL.at(i)->MFTNumber;
                }
            }
        }
    }
    qDebug()<<m_name_uuid<<"获取日志文件夹MFTNumber失败!";
    return -1;
}

//统计数据个数
void CNtfsInfo::dataCountAdd(quint8 flag)
{
    switch (flag){//统计数量
    case NOEXISTING_FILE_FLAG:
        m_initDatasInfo.dataBodyCount._falg_0_count++;
        break;
    case EXISTING_FILE_FLAG:
        m_initDatasInfo.dataBodyCount._falg_1_count++;
        break;
    case NOEXISTING_DIRECTORY_FLAG:
        m_initDatasInfo.dataBodyCount._falg_2_count++;
        break;
    case EXISTING_DIRECTORY_FLAG:
        m_initDatasInfo.dataBodyCount._falg_3_count++;
        break;
    default:
        m_initDatasInfo.dataBodyCount._falg_other_count++;
        break;
    }
}
//获取usnJournal基本信息
bool CNtfsInfo::_getUsnJournalBasicInfo(S_MFT_USN_JOURNAL& usnJournalBasicInfo,QByteArray byteData)
{
    S_FILE_RECORD fileRecordHead;//文件头
    bool isOk = NTFS_MFTFileHeader(fileRecordHead,byteData);
    if(!isOk)
        return false;

    quint32 allLength = fileRecordHead.BytesInUse-4;//所有属性的大小
    quint32 byteOffset = fileRecordHead.AttributeOffset;
    int whileCount = allLength ;

    while(byteOffset<allLength ){
        if((--whileCount) == 0){        //死循环异常
            NTFS_setErrorType(CNTFS::WhileError);
            return false;
        }
        if(!(byteData.mid(byteOffset,sizeof (quint32)).toHex()).compare("FFFFFFFF",Qt::CaseInsensitive))
            return true;
        S_USN_JOURNAL_ATTRIBUTE_HEADER attH;//属性头
        qint8 NonResidentFlag = NTFS_MFTUsnAtt(attH,byteData.mid(byteOffset,sizeof(S_USN_JOURNAL_ATTRIBUTE_HEADER)));//获取属性 返回0常驻，1非常驻，-1：失败
        if(NonResidentFlag == -1)
            return false;
        switch(attH.AttributeType) {
        //0x20
        case AttributeAttributeList:
        {
            byteOffset += attH.NonResidentFlag==0 ? attH.CCommon.CResident.StreamOffset : attH.CCommon.CNonResident.RunListOffset;
            if(attH.NonResidentFlag==0 && (attH.AttributeLength - attH.CCommon.CResident.StreamOffset)>0) {
                QList<S_ATTRIBUTE_0X20> x20L = NTFS_0X20Att(byteData.mid(byteOffset,attH.AttributeLength - attH.CCommon.CResident.StreamOffset));
                for(int i=0;i<x20L.count();i++) {
                    QList<MFTNUM> _temp;
                    //只获取不同的mft 属性x80
                    if((x20L.at(i).AttributeType == AttributeData ) && x20L.at(i).Fdir.FileReferenceNumber != fileRecordHead.MFTRecordNumber){
                        if(!_temp.contains(x20L.at(i).Fdir.FileReferenceNumber)) {      //以防多次执行，不同的mft只执行一次
                            _temp.append(x20L.at(i).Fdir.FileReferenceNumber);
                            quint64 offset = getMTFOffset(x20L.at(i).Fdir.FileReferenceNumber);
                            QByteArray data = getRawData(m_NTFSOffset + offset,BYTES_PER_MFT_FILE);
                            if(data.isNull())
                                return false;
                            bool isOk = _getUsnJournalBasicInfo( usnJournalBasicInfo,data);
                            if(!isOk)
                                return false;
                        }
                    }
                }
                byteOffset += (attH.AttributeLength-attH.CCommon.CResident.StreamOffset);
            } else if(attH.NonResidentFlag==1 && (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset)>0) {    //预防
                QList<S_DATARUN> DRunList;
                DRunList = NTFS_DatasRun(byteData.mid(byteOffset,attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset));
                for(int i=0;i<DRunList.count();i++) {
                    QByteArray data = getRawData(m_NTFSOffset + DRunList[i].dataStartCluster * BYTES_PER_CLUSTER,attH.CCommon.CNonResident.StreamRealSize);
                    if(!data.isNull()) {
                        QList<S_ATTRIBUTE_0X20> x20L = NTFS_0X20Att(data);
                        for(int i=0;i<x20L.count();i++){
                            QList<MFTNUM> _temp;
                            //只获取不同的mft 属性x10和x30和x80
                            if((x20L.at(i).AttributeType == AttributeData ) && x20L.at(i).Fdir.FileReferenceNumber != fileRecordHead.MFTRecordNumber) {
                                if(!_temp.contains(x20L.at(i).Fdir.FileReferenceNumber)){//以防多次执行，不同的mft只执行一次
                                    _temp.append(x20L.at(i).Fdir.FileReferenceNumber);
                                    quint64 offset = getMTFOffset(x20L.at(i).Fdir.FileReferenceNumber);
                                    QByteArray data = getRawData(m_NTFSOffset + offset,BYTES_PER_MFT_FILE);
                                    if(data.isNull())
                                        return false;
                                    bool isOk = _getUsnJournalBasicInfo( usnJournalBasicInfo,data);
                                    if(!isOk)
                                        return false;
                                }
                            }
                        }
                    }
                }
                byteOffset += (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset);
            }
        }
            break;
            //x30
        case AttributeFileName:
        {
            byteOffset += attH.NonResidentFlag==0 ? attH.CCommon.CResident.StreamOffset : attH.CCommon.CNonResident.RunListOffset;
            if(attH.NonResidentFlag==0 && (attH.AttributeLength - attH.CCommon.CResident.StreamOffset)>0) {
                S_ATTRIBUTE_0X30 x30;
                QByteArray dataName = NTFS_0X30AttName(x30,byteData.mid(byteOffset,attH.AttributeLength - attH.CCommon.CResident.StreamOffset));
                if(!dataName.isNull()) {
                    if(dataName.compare("$UsnJrnl",Qt::CaseInsensitive)==0)
                        usnJournalBasicInfo.ISOK = true;
                }

                byteOffset += (attH.AttributeLength-attH.CCommon.CResident.StreamOffset);
            } else if(attH.NonResidentFlag==1 && (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset)>0){//预防
                byteOffset += (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset);
            }

        }
            break;
            //0x80
        case AttributeData:
        {
            byteOffset += attH.NonResidentFlag==0 ? attH.CCommon.CResident.StreamOffset : attH.CCommon.CNonResident.RunListOffset;
            if(attH.NonResidentFlag==0 && (attH.AttributeLength - attH.CCommon.CResident.StreamOffset)>0){
                QString name = QString::fromWCharArray(attH.CCommon.CResident.CAttName,attH.AttributeNameLength);//QString ret2 = QString((QChar*)x30.Name, wcslen(x30.Name));
                if(name=="$Max") {
                    errno_t err = memcpy_s(&usnJournalBasicInfo.MaxAtt,16,byteData.mid(byteOffset,16).data(),16);//返回值为0，拷贝成功
                    if(err) {
                        qDebug()<<m_name_uuid<<"memcpy_s拷贝错误码:"<<err;
                        return false;
                    }
                }
                byteOffset += (attH.AttributeLength-attH.CCommon.CResident.StreamOffset);
            } else if(attH.NonResidentFlag==1 && (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset)>0) {
                /*
                 * 获取属性名,可能存在偏移位置不同
                 */
                char dataTemp[100];
                memset(dataTemp,0,sizeof(dataTemp));
                errno_t err = memcpy_s(dataTemp,sizeof(dataTemp), &attH,sizeof (S_USN_JOURNAL_ATTRIBUTE_HEADER));//返回值为0，拷贝成功
                if(err) {
                    qDebug()<<m_name_uuid<<"memcpy_s拷贝错误码:"<<err;
                    return false;
                }
                QString name = QString::fromWCharArray((wchar_t *)&dataTemp[attH.ContentOffset],attH.AttributeNameLength);
                if(name=="$J"){
                    usnJournalBasicInfo.JAtt.append(attH);
                    QByteArray temp = byteData.mid(byteOffset,attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset);

                    QList<S_DATARUN> DRunList;
                    DRunList = NTFS_DatasRun(byteData.mid(byteOffset,attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset));
                    if(!DRunList.isEmpty())
                        usnJournalBasicInfo.lDRun.append(DRunList);
                    byteOffset += (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset);
                }
            }
        }
            break;

        case 0xFFFFFFFF:
            return true;

        default:
            byteOffset += (attH.AttributeLength);
            break;
        }
    }
    return false;
}


//获取usnJournal MFT信息
S_MFT_USN_JOURNAL CNtfsInfo::getUsnJournalBasicInfo()
{

    S_MFT_USN_JOURNAL usnJournalBasicInfo;
    usnJournalBasicInfo.ISOK = false;
    quint64 mftOffset = getMTFOffset($Extend$UsnJrnl);
    QByteArray mftByte_1 = getRawData(m_NTFSOffset + mftOffset,BYTES_PER_MFT_FILE);
    if(mftByte_1.isNull())
        return usnJournalBasicInfo;
    S_MFT_INFO mftInfo_1;
    bool mftInfo_1IsOk= NTFS_MFTFileInfo(mftInfo_1,mftByte_1);
    if(!mftInfo_1IsOk)
        return usnJournalBasicInfo;
    if(mftInfo_1._x90.isEmpty())
        return usnJournalBasicInfo;
    if(mftInfo_1._x90.at(0).first.NonResidentFlag != 0)
        return usnJournalBasicInfo;
    QList<QPair<S_INDEX_ROOT_ITEM_ATTRIBUTE_0X90,QString>> varData = mftInfo_1._x90.at(0).second._x90Body;
    if (varData.isEmpty())
        return usnJournalBasicInfo;
    for(int i=0;i<varData.count();i++){
        if(!varData.at(i).second.compare(QString("$UsnJrnl"),Qt::CaseInsensitive)) {
            quint64 mftOffset = getMTFOffset(varData.at(i).first.dir.MFTDirectoryFile);
            QByteArray mftByte_1 = getRawData(m_NTFSOffset + mftOffset,BYTES_PER_MFT_FILE);
            if(mftByte_1.isNull())
                return usnJournalBasicInfo;
            bool isOk = _getUsnJournalBasicInfo(usnJournalBasicInfo,mftByte_1);
            if(isOk)
                usnJournalBasicInfo.ISOK = true;
            else{
                qDebug()<<m_name_uuid<<"获取usnJournalMFT信息失败!";
                usnJournalBasicInfo.ISOK = false;
                return usnJournalBasicInfo;
            }
            for(int i=0;i<usnJournalBasicInfo.JAtt.count();){//获取有用的属性头
                if(QString::compare(QString::fromWCharArray(usnJournalBasicInfo.JAtt.at(i).CCommon.CNonResident.CNonAttName,usnJournalBasicInfo.JAtt.at(i).AttributeNameLength),"$J")){
                    usnJournalBasicInfo.JAtt.removeAt(i);
                }else{
                    i++;
                }
            }
            if(usnJournalBasicInfo.JAtt.isEmpty())
                return usnJournalBasicInfo;
        }
    }
    return usnJournalBasicInfo;
}

//更新数据库
void CNtfsInfo::updataDataBaseV2(PUSN_RECORD_UNION usnRecordUnion,quint64 usnRecordOffset /*=0*/)
{
    //qDebug()<<m_name_uuid<<"Usn_V2";
    QString fileName = QString::fromWCharArray(usnRecordUnion->V2.FileName,usnRecordUnion->V2.FileNameLength/2);

    QScopedPointer<S_DATA_BODY> dataBody(new S_DATA_BODY);
    if(!dataBody)
        return;
    dataBody.data()->MFTNumber =  usnRecordUnion->V2.FileReferenceNumber;
    dataBody.data()->PMFTNumber =  usnRecordUnion->V2.ParentFileReferenceNumber;

    S_DATA_SUBSET aD;
    aD.guid = m_initDatasInfo.info.uuid;
    aD.reason = usnRecordUnion->V2.Reason;
    aD.usnOffset = usnRecordOffset;
    aD.mft = usnRecordUnion->V2.FileReferenceNumber;

    /*//0x00000100：捕捉创建文件或文件夹；0x80000200：捕捉删除文件；0x8000200：捕捉重命名文件 或者本盘剪切
                 *
                 *
                 * 0x01   -> 0x80000001  文件中覆盖数据 （文件字节大小不变）
                 * 0x02   -> 0x80000002  文件中增加数据（从无到有）
                 * 0x03   -> 0x80000003  文件中增加数据 （在原来的存在数据中增加数据）
                 * 0x05   -> 0x80000005  文件中删除部分数据
                 * 0x04   -> 0x80000004  文件中删除全部数据
                 *
                 * 0x04   -> 0x8004  -> 0x80008004  粘贴一个空文件替换掉原来的文件
                 * 0x04   -> 0x6 -> 0x7 -> 0x8007  -> 0x80008007  粘贴一个非空文件替换掉原来的文件
                 */

    if(!(LOG_PATH_PMft == dataBody.data()->PMFTNumber  || USN_RECORD_PATH_PMft == dataBody.data()->PMFTNumber ||
         DATA_UPDATE_PATH_PMft == dataBody.data()->PMFTNumber || RUN_LOG_PATH_PMft == dataBody.data()->PMFTNumber)){//log文件夹下的所有文件发生改变不采集
        S_USN_LOG array;
        array.dataTime = QDateTime::currentDateTime();
        array.partition = m_partitionName;
        array.usnOffset = usnRecordOffset;
        array.mft = (MFTNUM)usnRecordUnion->V2.FileReferenceNumber;
        array.pMft = (MFTNUM)usnRecordUnion->V2.ParentFileReferenceNumber;
        array.usn = usnRecordUnion->V2.Usn;
        array.reasonDataTime = usnRecordUnion->V2.TimeStamp.QuadPart;
        array.reason = usnRecordUnion->V2.Reason;
        array.attributes = usnRecordUnion->V2.FileAttributes;
        array.fileName = fileName.toUtf8();

        emit SignalUsnRecordLog(UsnRecordLog,QVariant::fromValue(array));//记录usn日志
    }

    switch (usnRecordUnion->V2.Reason) {
    case 0x00000100://捕捉创建 复制

    case 0x80002000://重命名文件  本盘剪切也是0x00002000

    case 0x80000200://捕捉删除文件

    case 0x80000001://文件中覆盖数据 （文件字节大小不变）

    case 0x80000002://文件中增加数据（从无到有）

        // break;
    case 0x80000003://文件中增加数据 （在原来的存在数据中增加数据）

        // break;
    case 0x80000004://文件中删除全部数据


        // break;
    case 0x80000005://文件中删除部分数据

        // break;
    case 0x80008004://粘贴一个空文件替换掉原来的文件

        // break;
    case 0x80008007://粘贴一个非空文件替换掉原来的文件
    {
        MFT_BASIC_INFO basicInfo;
        quint64 mftOffset = getMTFOffset(dataBody.data()->MFTNumber);
        QByteArray byteData = getRawData(m_NTFSOffset + mftOffset,BYTES_PER_MFT_FILE);
        if(byteData.isNull())
            return ;
        if(!getMFTBasicInfo(basicInfo,byteData))
            return;
        for(int i=0;i<basicInfo.fileL.count();i++)
            basicInfo.fileL.at(i)->bodyAtt = basicInfo.attBody;

        aD.dataBodyL = basicInfo.fileL;
        emit SignalDataBase(aD);//更新数据
    }
        return;
    default:
        break;
    }

}
void CNtfsInfo::updataDataBaseV3(PUSN_RECORD_UNION usnRecordUnion)
{
    qDebug()<<m_name_uuid<<"Usn_V3";
}
void CNtfsInfo::updataDataBaseV4(PUSN_RECORD_UNION usnRecordUnion)
{
    qDebug()<<m_name_uuid<<"Usn_V4";
}
//监控usn记录
void CNtfsInfo::usnRecordMonitoring()
{
    qDebug()<<m_name_uuid<<"开启usn记录监控!";
    while (!QThread::currentThread()->isInterruptionRequested()) {
        if(m_mftUsnJournal.ISOK)
            usnStartRun(m_mftUsnJournal);
        while (!QThread::currentThread()->isInterruptionRequested() && MemoryPool::getInstance()->getSettingArgs().usnRecordNoMonitoring)
            QThread::sleep(1);
        QThread::sleep(1);
        CNtfsInfo::Error type = anewInitDrive();
        switch (type){
        case NoError:
        {
            qDebug()<<m_name_uuid<<"NoError:重新获取usnJournalMFT信息!";
            while (!QThread::currentThread()->isInterruptionRequested()){
                QThread::sleep(1);
                m_mftUsnJournal = getUsnJournalBasicInfo();
                if(!m_mftUsnJournal.ISOK)
                    break;
                //判断后来的StreamAiiocSize和DataSize和类中的比较是否相等，相等表示，数据块未发生变化，继续采集
                //不相等表示，数据块发生变化，覆盖类中的usnjournal MFT信息，重新采集usnjournal记录
                if(m_mftUsnJournal.JAtt.isEmpty())
                    continue;
                //必须大于lastUSNNumber 说明才有数据，不然一直循环获取usn，直到有数据。 等于说明，最后（0x228最大usnjournal日志大小）存在数据，需要重新获取最后
                if(m_mftUsnJournal.JAtt[0].CCommon.CNonResident.StreamRealSize >= m_lastUsnNumber) {
                    m_firstAnew = false;
                    break;
                }
            }
        }
            break;
        case InitError:
        {
            qDebug()<<m_name_uuid<<"InitError:重新初始化失败,结束线程.";
            return;
        }
            break;
        case AnewInit:
        {
            qDebug()<<m_name_uuid<<"AnewInit:重新初始化,获取所有数据,重新采集...";
            initData();

        }
            break;

        }
    }
}

//开启usn采集
void CNtfsInfo::usnStartRun(S_MFT_USN_JOURNAL usnJour)
{
    qDebug()<<m_name_uuid<<"开始采集usn记录...";
    quint32 count_i = 0;
    quint64 USNoffset=0;//实际偏移地址lcn
    quint64 dataRunSize=0;//当前使用的数据块大小
    quint64 dataRunOffset=0;//当前使用的数据块的偏移地址vcn  （不能大于dataRunSize）
    quint64 tempSize = 0;//记录数据块总共大小
    //firstAnew为TRUE是第开启usn
    if(m_firstAnew)
        m_lastUsnNumber = usnJour.JAtt[0].CCommon.CNonResident.StreamRealSize;
    //获取要读取usn记录的位置
    for(int i=0;i<usnJour.lDRun.count();++i) {
        //累加数据块大小
        tempSize += usnJour.lDRun[i].datalengthCluster * BYTES_PER_CLUSTER;
        //数据块大小和最后一个usn相等，从下一个块读取
        if(tempSize == m_lastUsnNumber){
            //当相加的数据块大小等于usn号时，此时usn号时下一个块的开头，所以要验证下一个数据块是否存在
            if((++i)>=usnJour.lDRun.count())
                return ;

            //要读取的数据块偏移vcn
            dataRunOffset = 0;
            //要读取的数据块大小vcn
            dataRunSize = usnJour.lDRun[i].datalengthCluster * BYTES_PER_CLUSTER;
            //要读取的物理偏移地址lcn
            USNoffset =  m_NTFSOffset + usnJour.lDRun[i].dataStartCluster * BYTES_PER_CLUSTER;
            break;
        } else if(tempSize > m_lastUsnNumber){   //数据块大小大于最后一个usn，从当前数据块读取

            //要读取的数据块偏移vcn
            dataRunOffset = usnJour.lDRun[i].datalengthCluster * BYTES_PER_CLUSTER +  m_lastUsnNumber - tempSize;
            //要读取的数据块大小vcn
            dataRunSize = usnJour.lDRun[i].datalengthCluster * BYTES_PER_CLUSTER;
            //要读取的物理偏移地址lcn  取扇区首地址才能正常读取数据
            USNoffset =  m_NTFSOffset + usnJour.lDRun[i].dataStartCluster * BYTES_PER_CLUSTER + dataRunOffset - (dataRunOffset % PAGE_USN_RECORD_SIZE);

            break;
        }
    }

    qDebug()<<m_name_uuid<<"UsnNumber:"+QString::number(m_lastUsnNumber)<<" --- Usn记录偏移:"+QString::number(USNoffset - m_NTFSOffset);
    while (dataRunOffset<dataRunSize && !QThread::currentThread()->isInterruptionRequested()){
        //不监控usn
        if(MemoryPool::getInstance()->getSettingArgs().usnRecordNoMonitoring)
            return;
        if(!m_fileHandle->seek(USNoffset))
            return ;
        QByteArray rawdata = m_fileHandle->read(PAGE_USN_RECORD_SIZE+1);//读取
        if(rawdata.size()!=PAGE_USN_RECORD_SIZE+1)
            return ;
        quint32 readSize=0;
        if((PAGE_USN_RECORD_SIZE - dataRunOffset % PAGE_USN_RECORD_SIZE) >= MAX_USN_REOCRD_SIZE)//判断要copy的字节大小
            readSize = MAX_USN_REOCRD_SIZE;
        else
            readSize = (PAGE_USN_RECORD_SIZE - dataRunOffset % PAGE_USN_RECORD_SIZE);

        //memcpy(m_usnRecordUnion,rawdata.mid(dataRunOffset % PAGE_USN_RECORD_SIZE,readSize).data(),readSize);

        errno_t err = memcpy_s(m_usnRecordUnion,MAX_USN_REOCRD_SIZE,rawdata.mid(dataRunOffset % PAGE_USN_RECORD_SIZE,readSize).data(),readSize);//返回值为0，拷贝成功
        if(err){
            qDebug()<<m_name_uuid<<"memcpy_s拷贝错误码:"<<err;
            return ;
        }
        if(m_usnRecordUnion->V2.Usn && !m_firstAnew && (quint64)m_usnRecordUnion->V2.Usn != m_lastUsnNumber)
            return ;
        if(!m_usnRecordUnion->Header.RecordLength){//当记录都为0的时候，表示没有数据，
            count_i++;
            if(count_i%50==0){  //连续n次数据为空给cpu腾出时间，进入休眠，给cpu腾出时间片，缓解cpu压力
                //qDebug()<<m_name_uuid<<"连续N次采集数据为空,重新初始化!";
                CNtfsInfo::Error error = anewInitDrive();
                if(error != NoError)
                    return ;

                QThread::sleep(1);
                count_i =0;
            }
            //QThread::msleep(500);//源码调用的也是Windows下Sleep();
            //QThread::msleep(0);//给cpu腾出时间片，
        }

        /*//0x00000100：捕捉创建文件或文件夹；0x80000200：捕捉删除文件；0x8000200：捕捉重命名文件 或者本盘剪切
                     *
                     *
                     * 0x01   -> 0x80000001  文件中覆盖数据 （文件字节大小不变）
                     * 0x02   -> 0x80000002  文件中增加数据（从无到有）
                     * 0x03   -> 0x80000003  文件中增加数据 （在原来的存在数据中增加数据）
                     * 0x05   -> 0x80000005  文件中删除部分数据
                     * 0x04   -> 0x80000004  文件中删除全部数据
                     *
                     * 0x04   -> 0x8004  -> 0x80008004  粘贴一个空文件替换掉原来的文件
                     * 0x04   -> 0x6 -> 0x7 -> 0x8007  -> 0x80008007  粘贴一个非空文件替换掉原来的文件
                     */
        switch (m_usnRecordUnion->Header.MajorVersion) {
        case 0x2:
        {

            if(m_usnRecordUnion->V2.Reason == 0)//等于0读取的数据不是完整的，重新读取
                continue;
            updataDataBaseV2(m_usnRecordUnion,USNoffset + dataRunOffset % PAGE_USN_RECORD_SIZE - m_NTFSOffset);
            m_lastUsnNumber = m_usnRecordUnion->V2.Usn + m_usnRecordUnion->Header.RecordLength;//记录最后一个usn号

        }
            break;
        case 0x3:
        {

            updataDataBaseV3(m_usnRecordUnion);
        }
            break;
        case 0x4:
        {
            updataDataBaseV4(m_usnRecordUnion);
        }
            break;
        default:
        {
            //continue;
        }
            break;

        }

        dataRunOffset += m_usnRecordUnion->Header.RecordLength;

        quint32 temp = PAGE_USN_RECORD_SIZE - (dataRunOffset % PAGE_USN_RECORD_SIZE);
        //剩余的数据小于0x40，丢弃//0x40 == sizeof(USN_RECORD_V2) + (8 - sizeof(USN_RECORD_V2)%8) //8字节是一个硬盘一块数据的最小占用
        if(temp < 0x40) {
            USNoffset += PAGE_USN_RECORD_SIZE;
            dataRunOffset += temp;
            m_lastUsnNumber += temp;//记录最后一个usn号
            if(dataRunOffset  == dataRunSize)//数据读到最后了，存在丢弃最后的小于0x228个字节
                break;
            else if(dataRunOffset  > dataRunSize)//大于就抛出异常
                return ;
        }else if(!m_usnRecordUnion->Header.RecordLength  && temp<MAX_USN_REOCRD_SIZE){   //0x228最大usnjournal日志大小
            if(dataRunOffset + temp == dataRunSize){    //数据读到最后了，存在丢弃最后的小于0x228个字节
                S_MFT_USN_JOURNAL mftUsnJournal = getUsnJournalBasicInfo();//先获取usn记录管理 1
                if(!mftUsnJournal.ISOK){    //usnjournal 错误:  卷更改日志处于非活动状态。
                    m_lastUsnNumber += temp;//记录最后一个usn号
                    break;
                }
                if(mftUsnJournal.JAtt[0].CCommon.CNonResident.StreamRealSize >= m_lastUsnNumber ){
                    m_firstAnew = false;
                    m_lastUsnNumber += temp;//记录最后一个usn号
                    break;

                }
            } else if(dataRunOffset + temp > dataRunSize){  //大于就抛出异常
                return ;
            }else if((quint8)rawdata.back()>0){
                m_lastUsnNumber += temp;//记录最后一个usn号
                dataRunOffset += temp;
                USNoffset += PAGE_USN_RECORD_SIZE;
            }
        }else if(temp == PAGE_USN_RECORD_SIZE && m_usnRecordUnion->Header.RecordLength != 0){
            USNoffset += PAGE_USN_RECORD_SIZE;
        }
    }
    return ;
}

//主动结束线程
void CNtfsInfo::SlotInternalExitThread()
{

}

//被动结束线程
void CNtfsInfo::SlotExternalExitThread()
{
    QMutexLocker is___(&m_mutex);
    m_threadRunFlag = true;

}
//重新初始化
CNtfsInfo::Error CNtfsInfo::anewInitDrive()
{

    //qDebug()<<m_name_uuid<<"重新初始化!";
    if(QThread::currentThread()->isInterruptionRequested())
        return InitError;
    if(!initDrive())
    {
        return InitError;
    }

    //获取的运行列表不能小于当前列表，
    if(m_tempMftTable.count()<m_mftTable.count())
        return AnewInit;
    for(int i=0;i<m_mftTable.count();i++){
        if(m_mftTable[i] != m_tempMftTable[i])
            return AnewInit;
    }
    S_MFT_USN_JOURNAL mftUsnJournal = getUsnJournalBasicInfo();//先获取usn记录管理 1
    if(!mftUsnJournal.ISOK)//usnjournal 错误:  卷更改日志处于非活动状态。
        return AnewInit;
    return NoError;

}
void CNtfsInfo::initData()
{
    m_lastUsnNumber=0;
    m_firstAnew = true;
    if(!initDrive()){
        emit SignalExitThread(m_initDatasInfo.info);
        return;
    }
    m_mftTable = m_tempMftTable;
    m_volumeName = getmVolumeName();
    QT_TRY
    {
        //bool isWhile = false;
        for(;;){
            m_mftUsnJournal = getUsnJournalBasicInfo();//先获取usn记录管理 1
            if(!m_mftUsnJournal.ISOK)//usnjournal 错误:  卷更改日志处于非活动状态。
            {

                if(QThread::currentThread()->isInterruptionRequested()) {
                    return;
                }
                //创建usnjournal mft 阻塞执行
                int exitCode =  QProcess::execute("cmd", QStringList() << "/c" << QString("fsutil usn createjournal m=33554432 a=8388608 %1:").arg(m_partitionName.at(0)));
                qDebug()<<m_name_uuid<<"创建usnjournal MFT文件:fsutil usn createjournal 退出代码 ： "<<exitCode;
                QThread::sleep(3);

            }
            else
            {
                break;
            }
        }
        getAllFilesBasicInfo();//获取 文件名 2
        emit SignalInitDataBase(InitDataBase,m_initDatasInfo);
        LOG_PATH_PMft = getSaveUsnLogPMft(LOG_PATH);
        USN_RECORD_PATH_PMft = getSaveUsnLogPMft(USN_RECORD_PATH);
        RUN_LOG_PATH_PMft = getSaveUsnLogPMft(RUN_LOG_PATH);

    }
    QT_CATCH(...)
    {
        emit SignalExitThread(m_initDatasInfo.info);
        qDebug()<<m_name_uuid<<"异常1结束线程:"<<QThread::currentThread()<<" --- 错误类型:"<<NTFS_errorString();
        return ;
    }
}
//开起线程
void CNtfsInfo::startRun()
{
    qDebug()<<m_name_uuid<<"开启线程:"<<QThread::currentThread();
    QT_TRY
    {
        initData();
        usnRecordMonitoring();// 开始执行usn 3
    }
    QT_CATCH(...)
    {
        emit SignalExitThread(m_initDatasInfo.info);
        qDebug()<<m_name_uuid<<"异常2结束线程:"<<QThread::currentThread()<<" --- 错误类型:"<<NTFS_errorString();
        return;
    }
    emit SignalExitThread(m_initDatasInfo.info);

}
