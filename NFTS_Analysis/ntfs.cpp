#include "ntfs.h"
#include <QDebug>
CGPT *CGPT::m_instance = new CGPT;
CGPT::CGPT(QObject *parent) : QObject(parent)
{
    m_name = "CGPT";
    m_name += SPLIT_SYMBOLS;
    qDebug()<<m_name<<"构造...";
}
CGPT * CGPT::getInstance()
{
    return m_instance;
}
//获取所有NTFS系统的盘符
QList<S_NTFS_INFO> CGPT::allDrive()
{
    /* 1：由于U盘是ntfs系统情况下，没有Gpt头，无法分析，这里采用先QStorageInfo::mountedVolumes() 获取系统下所有显示的盘符
     * 2：CGPT::GPT_Drive() 获取系统下所有硬盘的ntfs盘符
     * 3：通过对比找出 2 下的是否有隐藏盘符
     */
    QList<S_NTFS_INFO>   allPartition,tempAllPartition;
    QList<QStorageInfo> volumesInfos = QStorageInfo::mountedVolumes();

    for(auto & volume : volumesInfos){
        S_NTFS_INFO S_info;
        if(volume.isValid() && volume.fileSystemType() == "NTFS") {
            S_info.uuid = QUuid(QString(QString(volume.device()).section('{',1,1)).section('}',0,0));
            S_info.partitionFlagName = volume.rootPath().left(2);
            S_info.pDrive = QString("\\\\.\\%1").arg(S_info.partitionFlagName);
            S_info.bytesOffset = 0;
            S_info.bytesTotal = volume.bytesTotal();
            S_info.BytesPerSector = BYTES_PER_SECTION;
            allPartition.append(S_info);
            qDebug()<<m_name<<"结果:"<<volume;
        }
    }
    tempAllPartition.append(allPartition);

    QList<S_GPT_INFO> gptInfos = GPT_Drive();
//    //从物理字节区获取设备为空，没有以管理员身份打开范围空设备，提示
//    if(gptInfos.isEmpty()){
//        allPartition.clear();
//        return allPartition;
//    }
    for(int i=0;i<gptInfos.count();i++){
        bool isOk = true;
        for(int j=0;j<tempAllPartition.count();j++){
            if(gptInfos.at(i).pDrivePartitionItem.GUID == tempAllPartition.at(j).uuid){
                isOk = false;
                break;
            }
        }
        if(isOk){

            S_NTFS_INFO S_info;
            S_info.uuid = QUuid(gptInfos.at(i).pDrivePartitionItem.GUID);
            S_info.partitionFlagName = "*";
            S_info.pDrive = gptInfos.at(i).pDrive;
            S_info.bytesOffset = gptInfos.at(i).pDrivePartitionItem.StartSection * BYTES_PER_SECTION;//开始偏移位置;
            S_info.bytesTotal = (gptInfos.at(i).pDrivePartitionItem.EndSection - gptInfos.at(i).pDrivePartitionItem.StartSection ) * BYTES_PER_SECTION;
            S_info.BytesPerSector = BYTES_PER_SECTION;
            qDebug()<<m_name<<"结果:"<<" 隐藏盘符:"<<" --- uuid:"<<S_info.uuid<<" --- partitionFlagName:"<<S_info.partitionFlagName
                   <<" --- pDrive:"<<S_info.pDrive<<" --- bytesOffset:"<<S_info.bytesOffset<<" --- bytesTotal:"<<S_info.bytesTotal<<" --- BytesPerSector:"<<S_info.BytesPerSector;
            allPartition.append(S_info);
        }
    }
    return allPartition;

}

//保存所有GPT物理设备信息
QList<S_GPT_INFO>  CGPT::GPT_Drive()
{
    QList<S_GPT_INFO>   allPartition;
    int i =0;
    //判断前n个物理设备，没有自动停止
    forever {
        QString driveName = QString("\\\\.\\PhysicalDrive%1").arg(i++);
        QFile driveHandle(driveName);
        if(!driveHandle.open(QIODevice::ReadWrite | QIODevice::ExistingOnly))
            break;
        if(!driveHandle.seek(0)){
            driveHandle.close();
            continue;
        }
        QByteArray protectMBR = driveHandle.read(BYTES_PER_SECTION);
        if(protectMBR.size() != BYTES_PER_SECTION){
            driveHandle.close();
            continue;
        }

        //GPT
        S_EFI_GPT_HEADER GPT;
        if(!driveHandle.seek(BYTES_PER_SECTION)) {
            driveHandle.close();
            continue;
        }
        QByteArray gptArray = driveHandle.read(N_EFI_GPT_SIZE);
        if(N_EFI_GPT_SIZE != gptArray.size()){
            driveHandle.close();
            continue;
        }
        errno_t err = memcpy_s(&GPT,N_EFI_GPT_SIZE,gptArray.data(),N_EFI_GPT_SIZE);
        if(err){
            driveHandle.close();
            continue;
        }

        //保护MBR包含一个DOS分区表(LBA0)，只包含一个类型值为0xEE的分区项，在小于2TB的磁盘上，大小为整个磁盘；在更大的磁盘上，它的大小固定为2TB。它的作用是阻止不能识别GPT分区的磁盘工具试图对其进行分区或格式化等操作，所以该扇区被称为“保护MBR”。实际上，EFI根本不使用这个分区表。
        if((quint8)protectMBR.at(0x1c2) == 0xEE && GPT_isGPT(GPT))  {//Gpt
            //一个硬盘的分区占用的最大字节
            int partitionTablesSize = GPT.PartitionTablesAllNum * GPT.PartitionTableByteNum;

            //循环读取某个分区表每个分区信息
            if(!driveHandle.seek(GPT.PartitionTableStartSection * BYTES_PER_SECTION)){
                driveHandle.close();
                continue;
            }
            QByteArray partitionTables = driveHandle.read(partitionTablesSize);
            if(partitionTables.size() != partitionTablesSize){
                driveHandle.close();
                continue;
            }
            int byteOffset = 0;
            while(byteOffset < partitionTablesSize){
                S_PARTITION_ITEM partitionItem;
                errno_t err = memcpy_s(&partitionItem,GPT.PartitionTableByteNum,partitionTables.mid(byteOffset,GPT.PartitionTableByteNum).data(),GPT.PartitionTableByteNum);
                if(err)
                    break;

                if(QUuid(partitionItem.GUIDType) == QUuid("{EBD0A0A2-B9E5-4433-87C0-68B6B72699C7}")) {
                    S_GPT_INFO S_info;
                    S_info.pDrivePartitionItem = partitionItem;
                    S_info.pDrive = driveName;
                    QString str;
                    quint64 strINT64=0;

                    str += QString("    物理设备:%1 ;").arg(driveName);
                    memcpy(&strINT64,partitionItem.GUIDType.Data4,8);
                    str += QString("    分区类型:%1-%2-%3-%4 ;").arg(QString::number(partitionItem.GUIDType.Data1,16)).arg(QString::number(partitionItem.GUIDType.Data2,16))
                            .arg(QString::number(partitionItem.GUIDType.Data3,16)).arg(QString::number(strINT64,16));
                    memcpy(&strINT64,partitionItem.GUID.Data4,8);
                    str += QString("    分区唯一标示符:%1-%2-%3-%4 ;").arg(QString::number(partitionItem.GUID.Data1,16)).arg(QString::number(partitionItem.GUID.Data2,16))
                            .arg(QString::number(partitionItem.GUID.Data3,16)).arg(QString::number(strINT64,16));
                    str += QString("    分区的起始扇区:0x%1 ;").arg(QString::number(partitionItem.StartSection,16));
                    str += QString("    分区的结束扇区:0x%1 ;").arg(QString::number(partitionItem.EndSection,16));
                    qDebug()<<m_name<<str;
                    allPartition.append(S_info);

                }else if(QUuid(partitionItem.GUIDType) == QUuid("{00000000-0000-0000-0000-000000000000}")) {
                    break;
                }
                byteOffset+=GPT.PartitionTableByteNum;
            }
            driveHandle.close();
        }
    }
    return allPartition;
}

//判断是否为GPT格式
bool CGPT::GPT_isGPT(S_EFI_GPT_HEADER GPT)const
{
    quint64 EFI = 0x5452415020494645;
    if(GPT.GPTHeader == EFI )
        return true;
    return false;
}

//=====================================================================================

CNTFS::CNTFS(QObject *parent) : QObject(parent)
{
    NTFS_unsetError();
}
CNTFS::~CNTFS()
{

}

//判断是ntfs系统？
bool CNTFS::NTFS_isNTFS(S_NTFS_BOOT& bootHead)
{
    //NTFS_unsetError();
    uchar ntfs[8]={0x4e,0x54,0x46,0x53,0x20,0x20,0x20,0x20};
    int isOk = memcmp(bootHead.OemID,ntfs,sizeof(ntfs));
    if(isOk) {
        NTFS_setErrorType(NoNTFSSystemError);
        return false;
    }
    return true;

}
//判断是file记录格式？

bool CNTFS::NTFS_isFILE(S_FILE_RECORD& fileRecordHead,QByteArray &rawData)
{
    //NTFS_unsetError();
    if(fileRecordHead.Ntfs.Type != 0x454c4946){
        NTFS_setErrorType(NoMftFileErrot);
        return false;
    }
    return NTFS_fixup( fileRecordHead, rawData);

}
//USA和USN的建立称为fixup。
bool CNTFS::NTFS_fixup(S_FILE_RECORD& fileRecordHead,QByteArray& rawData)
{
    //NTFS_unsetError();
    quint16 USN_1 =0;
    quint16 USN_2 =0;
    errno_t err = memcpy_s(&USN_1,2,rawData.mid(0x1FE,2).data(),2);//返回值为0，拷贝成功
    errno_t err2 = memcpy_s(&USN_2,2,rawData.mid(0x3FE,2).data(),2);//返回值为0，拷贝成功
    if(err || err2){
        NTFS_setErrorType(CopyDataError);
        return false;
    }


    /*这两项的作用在于对记录（文件记录，索引记录，RCRD记录，RSTR记录等等）进行扇区检查。
         * 其中，USA中的内容是该记录的每一个扇区的最后两个字节。
         * 同时，用USN的值取代每一个扇区的最后两个字节。
         * 这样，当记录被读取的时候，从文件头部分读取USN，并将其与该记录的每一个扇区的最后两位进行比较，如果相等，说明扇区无误，
         * 然后，将USA按顺序读入记录的各个扇区的最后一个字中。USA和USN的建立称为fixup。
         * 更新序号（USN）的值是一个记载被保护记录写入硬盘次数的循环数。
         * 其中，0和0XFFFF不被使用。
         */

    if((fileRecordHead.MFTUSN & USN_1) != USN_2 ){//三个数相等比较
        NTFS_setErrorType(CopyDataError);
        return false;
    }

    rawData.replace(0x1FE,2,fileRecordHead.MFTUSA_0,2);
    rawData.replace(0x3FE,2,fileRecordHead.MFTUSA_1,2);
    return true;
}

//获取Boot头
bool CNTFS::NTFS_BootHeader(S_NTFS_BOOT& boot,QByteArray rawData)
{
    //NTFS_unsetError();
    errno_t err = memcpy_s(&boot,sizeof(S_NTFS_BOOT),rawData.data(),sizeof(S_NTFS_BOOT));//返回值为0，拷贝成功
    if(err){
        NTFS_setErrorType(CopyDataError);
        return false;
    }
    if(!NTFS_isNTFS(boot))
        return false;
    return true;
}


//返回首个MFT偏移位置
quint64 CNTFS::NTFS_firstMFTOffset(S_NTFS_BOOT &boot)
{
    return boot.MftStartLcn * boot.PackedBpb.SectorsPerCluster * boot.PackedBpb.BytesPerSector;
}

//获取MTF号的偏移地址
quint64 CNTFS::NTFS_MTFOffset(MFTNUM MTFNumber,QList<S_MFT0_0X80_DATARUN> &MFTL)
{
    if(MFTL.isEmpty()){
        qDebug()<<m_name_uuid<<"MFT列表为空,无法获取MTFNumber: ["<<MTFNumber<<"],抛出异常!";
        QT_THROW(0);
    }
    if(MTFNumber<=MFTL[MFTL.count()-1].highNumber){
        for(int i=0;i<MFTL.count();++i) {
            if(MTFNumber>=MFTL[i].lowNumber && MTFNumber<=MFTL[i].highNumber){
                quint64 offset = (MTFNumber - MFTL[i].lowNumber ) * BYTES_PER_MFT_FILE + MFTL[i].dataStartByte;
                return offset;
            }
        }
    }
    m_errorType = NoNTFSNumberErrot;
    return 0;
}

//获取mft表数据运行
QList<S_MFT0_0X80_DATARUN> CNTFS::NTFS_MFTTable(QList<S_DATARUN>& dataL)
{
    //NTFS_unsetError();
    QList<S_MFT0_0X80_DATARUN> MFTL;
    if(dataL.count()==0)
        return MFTL;

    quint64 allLength=0;
    for(int i=0;i<dataL.count();++i){
        if(dataL[i].dataStartCluster == 0)
            continue;
        S_MFT0_0X80_DATARUN x80_data;
        if(i>0)
            x80_data.lowNumber = MFTL[i-1].highNumber+1;
        if(i==0)
            x80_data.lowNumber=0;
        x80_data.highNumber = x80_data.lowNumber + dataL[i].datalengthCluster * BYTES_PER_CLUSTER / BYTES_PER_MFT_FILE - 1;
        x80_data.datalengthByte = dataL[i].datalengthCluster * BYTES_PER_CLUSTER ;
        allLength += x80_data.datalengthByte;
        x80_data.dataStartByte = dataL[i].dataStartCluster * BYTES_PER_CLUSTER ;
        MFTL.append(x80_data);
    }
    return MFTL;

}


//获取mft文件头
bool CNTFS::NTFS_MFTFileHeader(S_FILE_RECORD& fileRecordHead,QByteArray& rawData)
{
    //NTFS_unsetError();
    errno_t err = memcpy_s(&fileRecordHead,sizeof(S_FILE_RECORD),rawData.data(),sizeof(S_FILE_RECORD));//返回值为0，拷贝成功
    if(err){
        NTFS_setErrorType(CopyDataError);
        return false;
    }
    if(!NTFS_isFILE(fileRecordHead,rawData))
        return false;
    return true;

}
//获取属性 返回0常驻，1非常驻，-1：失败
qint8 CNTFS::NTFS_MFTAtt(S_NTFS_ATTRIBUTE_HEADER& att,QByteArray rawData)
{
    //NTFS_unsetError();
    errno_t err = memcpy_s(&att,sizeof(S_NTFS_ATTRIBUTE_HEADER),rawData.data(),sizeof(S_NTFS_ATTRIBUTE_HEADER));//返回值为0，拷贝成功
    if(err){
        NTFS_setErrorType(CopyDataError);
        return -1;
    }
    return att.NonResidentFlag;
}
//获取usn属性 返回0常驻，1非常驻，-1：失败
qint8 CNTFS::NTFS_MFTUsnAtt(S_USN_JOURNAL_ATTRIBUTE_HEADER& att,QByteArray rawData)
{
    //NTFS_unsetError();
    errno_t err = memcpy_s(&att,sizeof(S_USN_JOURNAL_ATTRIBUTE_HEADER),rawData.data(),sizeof(S_USN_JOURNAL_ATTRIBUTE_HEADER));//返回值为0，拷贝成功
    if(err){
        NTFS_setErrorType(CopyDataError);
        return -1;
    }
    return att.NonResidentFlag;
}
//获取0x10属性 常驻
bool CNTFS::NTFS_0X10Att(S_ATTRIBUTE_0X10& x10,QByteArray rawData)
{
    //NTFS_unsetError();
    errno_t err = memcpy_s(&x10,sizeof(S_ATTRIBUTE_0X10),rawData.data(),sizeof(S_ATTRIBUTE_0X10));//返回值为0，拷贝成功
    if(err) {
        NTFS_setErrorType(CopyDataError);
        return false;
    }
    return true;
}
//获取0x20属性 常驻
QList<S_ATTRIBUTE_0X20> CNTFS::NTFS_0X20Att(QByteArray rawData)
{
    //NTFS_unsetError();
    QList<S_ATTRIBUTE_0X20> x20L;

    qint32 total=0;
    qint32 x20Size = sizeof(S_ATTRIBUTE_0X20);
    int whileCount = rawData.size();
    qint32 dataSize = rawData.size();
    while(total < dataSize) {
        if((--whileCount) == 0) {
            NTFS_setErrorType(WhileError);
            break;
        }
        S_ATTRIBUTE_0X20 x20;
        QByteArray tempData = rawData.mid(total,x20Size);
        errno_t err = memcpy_s(&x20,x20Size,tempData.data(),tempData.size());//返回值为0，拷贝成功
        if(!err){
            x20L.append(x20);
            total+=x20.Length;
        }
    }
    return x20L;
}
//获取0x30属性名字(文件名) 常驻
QByteArray CNTFS::NTFS_0X30AttName(S_ATTRIBUTE_0X30 & x30,QByteArray rawData)
{
    //NTFS_unsetError();
    qint32 x30Size = sizeof(S_ATTRIBUTE_0X30);
    errno_t err = memcpy_s(&x30,x30Size,rawData.data(),x30Size);//返回值为0，拷贝成功
    if(err) {
        NTFS_setErrorType(CopyDataError);
        return QByteArray();
    }
    //QString ret2 = QString((QChar*)x30.Name, wcslen(x30.Name));
    return QString::fromWCharArray((wchar_t *)(rawData.mid(x30Size-sizeof(wchar_t),x30.NameLength*sizeof(wchar_t)).data()),x30.NameLength).toUtf8();

}
//获取0x90属性 常驻
QList<QPair<S_INDEX_ROOT_ITEM_ATTRIBUTE_0X90,QString> > CNTFS::NTFS_0X90Att(QByteArray rawData)
{
    //NTFS_unsetError();
    QList<QPair<S_INDEX_ROOT_ITEM_ATTRIBUTE_0X90,QString> > x90L;

    qint32 total=0;
    qint32 x90Size = sizeof(S_INDEX_ROOT_ITEM_ATTRIBUTE_0X90);
    int whileCount = rawData.size();
    qint32 dataSize = rawData.size();
    while(total < dataSize){
        if((--whileCount) == 0) {
            NTFS_setErrorType(WhileError);
            break;
        }
        S_INDEX_ROOT_ITEM_ATTRIBUTE_0X90 x90;
        QByteArray tempData = rawData.mid(total,x90Size);
        errno_t err = memcpy_s(&x90,x90Size,tempData.data(),tempData.size());//返回值为0，拷贝成功
        if(!err){
            if(x90.dir.MFTDirectoryFile==0)
                break;
            QString x90name = QString::fromWCharArray((wchar_t*)(rawData.mid(total+x90Size-sizeof(wchar_t),x90.FileNameLen*sizeof (wchar_t)).data()),x90.FileNameLen);
            x90L.append(qMakePair(x90,x90name));
            total+=x90.ItemLen;
        }
    }
    return x90L;
}

//获取数据运行块LCN
QList<S_DATARUN> CNTFS::NTFS_DatasRun(QByteArray rawdata)
{
    //NTFS_unsetError();
    QList<S_DATARUN> DRunList;
    qint32 byteOffset=0;
    qint32 dataSize = rawdata.size();
    int whileCount = dataSize ;

    while(byteOffset<dataSize){
        if((--whileCount) == 0){
            NTFS_setErrorType(WhileError);
            break;
        }
        S_DATARUN run;
        quint8 dataH = (quint8)rawdata[byteOffset++];
        if(!dataH)
            return DRunList;
        //取出一个字节的高四位和低四位
        quint8 high_four = (dataH & 0xf0) >> 4;
        quint8 low_four = dataH & 0x0f;
        //bool isOk;
        quint64 length =0;
        if(low_four){//低位是否为0
            errno_t err = memcpy_s(&length,8,rawdata.mid(byteOffset,low_four).leftJustified(8,0).data(),8);
            if(err){
                NTFS_setErrorType(CopyDataError);
                break;
            }
        }
        byteOffset +=low_four;
        qint64 dataStart =0;
        if(high_four){//高位是否为0：一般在usnjournal中数据run大于$Max属性中的最大值，第一个DataRun才为0
            errno_t err = memcpy_s(&dataStart,8,rawdata.mid(byteOffset,high_four).leftJustified(8,0).data(),8);
            if(err){
                NTFS_setErrorType(CopyDataError);
                break;
            }
            byteOffset+=high_four;
            if((quint8)rawdata[byteOffset-1]>=0x80){ // 负数
                quint64 tt = 0xFFFFFFFFFFFFFFFF;
                tt = tt<<(high_four)*8;
                dataStart |= tt;
            }
        }
        if(!DRunList.isEmpty()){//每增加一项获取最后的一项 把相对偏移转换实际偏移
            S_DATARUN temp = DRunList.last();
            dataStart = temp.dataStartCluster+dataStart;
        }
        run.datalengthCluster =length;
        run.dataStartCluster = dataStart;
        DRunList.append(run);
    }
    return  DRunList;

}
//获取一个mft文件所有信息
bool CNTFS::NTFS_MFTFileInfo(S_MFT_INFO& mftInfo,QByteArray rawData)
{
    mftInfo.MFTNumber=0;
    mftInfo.PMFTNumber=0;
    S_FILE_RECORD fileH;//文件头
    if(!NTFS_MFTFileHeader(fileH,rawData))
        return false;

    mftInfo.MFTNumber = fileH.MFTRecordNumber;
    mftInfo.flag = fileH.Flags;
    quint32 allLength = fileH.BytesInUse-4;//所有属性的大小
    quint32 byteOffset = fileH.AttributeOffset;
    int whileCount = rawData.size();//循环次数，以防死循环，一般只有几次
    while(byteOffset<allLength){
        if((--whileCount) == 0) {
            NTFS_setErrorType(WhileError);
            return false;
        }
        if(!(rawData.mid(byteOffset,sizeof (quint32)).toHex()).compare("FFFFFFFF",Qt::CaseInsensitive)){
            return true;
        }
        S_NTFS_ATTRIBUTE_HEADER attH;//属性头
        qint8 attflag = NTFS_MFTAtt(attH,rawData.mid(byteOffset,sizeof(S_NTFS_ATTRIBUTE_HEADER)));
        if(attflag == -1)
            return false;
        switch(attH.AttributeType){
        //0x10
        case AttributeStandardInformation:
        {
            S_ATTRIBUTE_BODY attBody;
            byteOffset += attH.NonResidentFlag==0 ? attH.CCommon.CResident.StreamOffset : attH.CCommon.CNonResident.RunListOffset;
            //0常驻
            if(!attH.NonResidentFlag && (attH.AttributeLength - attH.CCommon.CResident.StreamOffset)>0) {
                S_ATTRIBUTE_0X10 x10;
                if(!NTFS_0X10Att(x10,rawData.mid(byteOffset,sizeof(S_ATTRIBUTE_0X10))))
                    return false;
                byteOffset += (attH.AttributeLength-attH.CCommon.CResident.StreamOffset);
                attBody.data = QVariant::fromValue(x10);
                mftInfo._x10.append(qMakePair(attH,attBody));
            }
            //1非常驻
            else if(attH.NonResidentFlag && (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset)>0){//预防
                byteOffset += (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset);
            }
        }
            break;
            //0x20
        case AttributeAttributeList:
        {
            S_ATTRIBUTE_BODY attBody;

            byteOffset += attH.NonResidentFlag==0 ? attH.CCommon.CResident.StreamOffset : attH.CCommon.CNonResident.RunListOffset;
            if(attH.NonResidentFlag==0 && (attH.AttributeLength - attH.CCommon.CResident.StreamOffset)>0){
                //S_ATTRIBUTE_0X20 x20;
                attBody._x20Body.append(NTFS_0X20Att(rawData.mid(byteOffset,attH.AttributeLength - attH.CCommon.CResident.StreamOffset)));
                mftInfo._x20.append(qMakePair(attH,attBody));
                byteOffset += (attH.AttributeLength-attH.CCommon.CResident.StreamOffset);
            }
            else if(attH.NonResidentFlag==1 && (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset)>0)//预防
            {
                QList<S_DATARUN> DRunList;

                DRunList = NTFS_DatasRun(rawData.mid(byteOffset,attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset));
                attBody.data = QVariant::fromValue(DRunList);
                mftInfo._x20.append(qMakePair(attH,attBody));
                byteOffset += (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset);
            }
        }
            break;
            //x30
        case AttributeFileName:
        {
            S_ATTRIBUTE_BODY attBody;
            byteOffset += attH.NonResidentFlag==0 ? attH.CCommon.CResident.StreamOffset : attH.CCommon.CNonResident.RunListOffset;
            if(attH.NonResidentFlag==0 && (attH.AttributeLength - attH.CCommon.CResident.StreamOffset)>0){
                S_ATTRIBUTE_0X30 x30;
                QByteArray name =  NTFS_0X30AttName(x30,rawData.mid(byteOffset,attH.AttributeLength - attH.CCommon.CResident.StreamOffset));
                if(name.isNull())
                {
                    return false;
                }
                attBody.name = name;
                mftInfo.PMFTNumber = x30.Fdir.DirectoryFile;
                attBody.data = QVariant::fromValue(x30);
                mftInfo._x30.append(qMakePair(attH,attBody));
                byteOffset += (attH.AttributeLength-attH.CCommon.CResident.StreamOffset);
            }
            else if(attH.NonResidentFlag==1 && (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset)>0)//预防
            {
                byteOffset += (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset);
            }
        }
            break;
            //0x60
        case AttributeVolumeName:
        {
            S_ATTRIBUTE_BODY attBody;
            byteOffset += attH.NonResidentFlag==0 ? attH.CCommon.CResident.StreamOffset : attH.CCommon.CNonResident.RunListOffset;
            if(attH.NonResidentFlag==0 && (attH.AttributeLength - attH.CCommon.CResident.StreamOffset)>0) {
                attBody.name = QString::fromWCharArray((wchar_t*)rawData.mid(byteOffset,attH.AttributeLength - attH.CCommon.CResident.StreamOffset).data(),attH.AttributeLength - attH.CCommon.CResident.StreamOffset);
                mftInfo._x60.append(qMakePair(attH,attBody));
                byteOffset += (attH.AttributeLength-attH.CCommon.CResident.StreamOffset);
            }
            else if(attH.NonResidentFlag==1 && (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset)>0)//预防
            {
                byteOffset += (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset);
            }
        }
            break;
            //0x80
        case AttributeData:
        {
            S_ATTRIBUTE_BODY attBody;
            byteOffset += attH.NonResidentFlag==0 ? attH.CCommon.CResident.StreamOffset : attH.CCommon.CNonResident.RunListOffset;
            if(attH.NonResidentFlag==0 && (attH.AttributeLength - attH.CCommon.CResident.StreamOffset)>0) {
                //attBody.name = rawData.mid(byteOffset,attH.AttributeLength - attH.CCommon.CResident.StreamOffset);
                byteOffset += (attH.AttributeLength-attH.CCommon.CResident.StreamOffset);
            }
            else if(attH.NonResidentFlag==1 && (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset)>0)
            {
                QList<S_DATARUN> DRunList;
                DRunList = NTFS_DatasRun(rawData.mid(byteOffset,attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset));
                attBody.data = QVariant::fromValue(DRunList);
                mftInfo._x80.append(qMakePair(attH,attBody));
                byteOffset += (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset);
            }
        }
            break;
            //0x90常驻
        case AttributeIndexRoot:
        {
            S_ATTRIBUTE_BODY attBody;
            byteOffset += attH.NonResidentFlag==0 ? attH.CCommon.CResident.StreamOffset : attH.CCommon.CNonResident.RunListOffset;
            if(attH.NonResidentFlag==0 && (attH.AttributeLength - attH.CCommon.CResident.StreamOffset)>0){
                //byteOffset += (attH.AttributeLength-attH.CCommon.CResident.StreamOffset);
                quint32 rootAttX90Size = sizeof(S_INDEX_ROOT_ATTRIBUTE_0X90);
                byteOffset += rootAttX90Size;
                quint32 dataSize = attH.AttributeLength - attH.CCommon.CResident.StreamOffset-rootAttX90Size;

                QList<QPair<S_INDEX_ROOT_ITEM_ATTRIBUTE_0X90,QString>> x90L = NTFS_0X90Att(rawData.mid(byteOffset,dataSize));
                attBody._x90Body.append(x90L);
                mftInfo._x90.append(qMakePair(attH,attBody));
                byteOffset += dataSize;

            }
            else if(attH.NonResidentFlag==1 && (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset)>0)//预防
            {
                byteOffset += (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset);
            }
        }
            break;
            //0xA0非常驻
        case AttributeIndexAllocation:
        {
            S_ATTRIBUTE_BODY attBody;
            byteOffset += attH.NonResidentFlag==0 ? attH.CCommon.CResident.StreamOffset : attH.CCommon.CNonResident.RunListOffset;
            if(attH.NonResidentFlag==0 && (attH.AttributeLength - attH.CCommon.CResident.StreamOffset)>0) {
                byteOffset += (attH.AttributeLength-attH.CCommon.CResident.StreamOffset);

            }else if(attH.NonResidentFlag==1 && (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset)>0) {
                QList<S_DATARUN> DRunList;
                DRunList = NTFS_DatasRun(rawData.mid(byteOffset,attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset));
                attBody.data = QVariant::fromValue(DRunList);
                mftInfo._x80.append(qMakePair(attH,attBody));
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

//获取错误类型
CNTFS::NTFS_Error CNTFS::NTFS_error()
{
    return  m_errorType;
}
//获取错误类型字符串
QString CNTFS::NTFS_errorString()
{
    switch (m_errorType) {
    case NoError:
        return "无错误";
    case WhileError:
        return "死循环错误";
    case NoNTFSNumberErrot:
        return "未找到mft号错误";
    case CopyDataError:
        return "拷贝数据错误";
    case NoMftFileErrot:
        return "不是mft文件";
    case NoNTFSSystemError:
        return "不是nfts系统";
    case BadClusterError:
        return "坏簇";
    default:
        return "未知错误";
    }
}

//复位错误为NoError
void CNTFS::NTFS_unsetError()
{
    m_errorType = NoError;
}
//设置异常类型
void CNTFS::NTFS_setErrorType(CNTFS::NTFS_Error errorType)
{
    m_errorType = errorType;
}
