#include "ntfsdatarecovery.h"
#include <QDebug>
#include <QtConcurrent>
QAtomicPointer<CNtfsDataRecovery> CNtfsDataRecovery::m_instance = nullptr;
QMutex  CNtfsDataRecovery::m_mutex;
CNtfsDataRecovery::CNtfsDataRecovery(QObject *parent) : CNTFS(parent)
{
    m_name = "RecoverData";
    m_name += SPLIT_SYMBOLS;
    qDebug()<<m_name<<"构造...";
}

CNtfsDataRecovery* CNtfsDataRecovery::getInstance()
{
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
        m_instance.testAndSetOrdered(nullptr, new CNtfsDataRecovery);//第二次检测。
    }
    return m_instance;
}
//从offset位置获取size大小数据
QByteArray CNtfsDataRecovery::getRawData(QFile& file, quint64 offset,qint32 size)
{
    if(!file.seek(offset))
        return QByteArray();
    QByteArray rawData = file.read(size);
    if(rawData.size() != size)
        return QByteArray();
    return rawData;
}

MFTNUM mft;                             //要恢复用到mft
QString pDrive;                         //存放一个物理硬盘名称
quint64 ntfsOffset;                     //盘符在物理硬盘中的偏移位置
QList<S_MFT0_0X80_DATARUN> mftTables;   //mft数据表
QString saveFileName;                   //保存文件名

//恢复数据
void CNtfsDataRecovery::recovery(S_RECOVER_INFO info)
{
    QtConcurrent::run([=]() mutable
    {
        qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" --- 开始恢复数据...";
        qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" --- 设备名:"<<info.pDrive<<" --- NTFS偏移:"<<info.ntfsOffset<<" --- 保存路径:"<<info.saveFileName;
        emit SignalStatusbarUpdata(Status_Text,"数据正在恢复中 ...");
        bool isSucceed = false;
        QFile readFile,writeFile;
        readFile.setFileName(info.pDrive);
        writeFile.setFileName(info.saveFileName);
        if(!readFile.open(QIODevice::ReadOnly | QIODevice::ExistingOnly))
        {
            emit SignalStatusbarUpdata(Status_Text,"数据恢复失败！");
            qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" -- 数据恢复失败!";
            return;
        }
        S_NTFS_BOOT bootH;
        if(!readFile.seek(info.ntfsOffset))
        {
            qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" -- seek失败!";
            return ;
        }
        QByteArray bootByte = readFile.read(sizeof (S_NTFS_BOOT));
        if(bootByte.size() != sizeof (S_NTFS_BOOT))
        {
            qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" -- 读取数据失败!";
            return ;
        }
        bool _isOk = NTFS_BootHeader(bootH,bootByte);
        if(!_isOk)
        {
            qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" -- 不是ntfs系统!";
            return ;
        }
        quint64 mftStart = NTFS_firstMFTOffset(bootH);
        if(!readFile.seek(info.ntfsOffset + mftStart))
            return ;
        QByteArray mftByte = readFile.read(BYTES_PER_MFT_FILE);
        if(mftByte.size() != BYTES_PER_MFT_FILE)
            return ;
        S_MFT_INFO mftInfo;
        bool mftInfoIsOk = NTFS_MFTFileInfo(mftInfo,mftByte);
        if(!mftInfoIsOk || mftInfo._x80.isEmpty() || mftInfo._x80.at(0).first.NonResidentFlag != 1)
            return ;
        QVariant varData = mftInfo._x80.at(0).second.data;
        if (!varData.canConvert<QList<S_DATARUN>>())
            return ;
        QList<S_DATARUN> dataL = varData.value<QList<S_DATARUN>>();
        if(dataL.count()==0)
            return ;
        QList<S_MFT0_0X80_DATARUN> mftTable = NTFS_MFTTable(dataL);
        info.mftTables = mftTable;

        quint64 readOffset = NTFS_MTFOffset(info.mft,info.mftTables);
        qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" -- mft相对偏移位置:"<<readOffset;
        if(readOffset == 0)
        {
            emit SignalStatusbarUpdata(Status_Text,"数据恢复失败！");
            qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" -- 数据恢复失败!";
            return;
        }
        QByteArray data = getRawData(readFile,info.ntfsOffset + readOffset,BYTES_PER_MFT_FILE);
        if(data.isNull())
        {
            emit SignalStatusbarUpdata(Status_Text,"数据恢复失败！");
            qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" -- 数据恢复失败!";
            return;
        }
        S_RECOVER_DATA dataInfo;
        bool isOk = getMFTDataInfo(readFile,info, dataInfo, data);
        if(!isOk)
        {
            emit SignalStatusbarUpdata(Status_Text,"数据恢复失败！");
            qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" -- 数据恢复失败!";
            return;
        }

        if(dataInfo.x80_Att.NonResidentFlag == 1)
        {
            bool noCover = fileIsNoCover(readFile,info,dataInfo.CNonResidentData);
            if(noCover)
            {
                if(!writeFile.open(QIODevice::WriteOnly | QIODevice::Append))
                {
                    emit SignalStatusbarUpdata(Status_Text,"数据恢复失败！");
                    qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" -- 数据恢复失败!";
                    return;
                }
                quint64 dataRealSize = dataInfo.x80_Att.CCommon.CNonResident.StreamRealSize;//数据的实际大小
                quint64 dataAiiocSize = dataInfo.x80_Att.CCommon.CNonResident.StreamAiiocSize;//空间的分配大小
                if(dataRealSize>dataAiiocSize)
                {
                    emit SignalStatusbarUpdata(Status_Text,"数据恢复失败！");
                    qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" -- 数据恢复失败!";
                    return;
                }
                quint64 readAllCount=0;//总共读的数据大小

                for(int i=0;i<dataInfo.CNonResidentData.count();i++)
                {
                    quint64 startOffset = dataInfo.CNonResidentData.at(i).dataStartCluster * BYTES_PER_CLUSTER;
                    if(startOffset == 0)
                        continue;
                    if(!readFile.seek(info.ntfsOffset +startOffset))
                    {
                        emit SignalStatusbarUpdata(Status_Text,"数据恢复失败！");
                        qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" -- 数据恢复失败!";
                        return;
                    }

                    quint64 readCount=0;//每段数据运行读的大小
                    quint64 runDataSize = dataInfo.CNonResidentData.at(i).datalengthCluster * BYTES_PER_CLUSTER;//每段数据的运行长度
                    qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" -- 要恢复的数据开始地址:"<<info.ntfsOffset +startOffset<<" --- 数据长度:"<<runDataSize;
                    while(readCount<runDataSize)
                    {
                        QByteArray data = readFile.read(BYTES_PER_CLUSTER);
                        if(data.size() != BYTES_PER_CLUSTER)
                        {
                            emit SignalStatusbarUpdata(Status_Text,"数据恢复失败！");
                            qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" -- 数据恢复失败!";
                            return;
                        }

                        readCount += BYTES_PER_CLUSTER;
                        readAllCount += BYTES_PER_CLUSTER;
                        if(readAllCount>=dataRealSize)
                        {
                            data.chop(readAllCount-dataRealSize);
                            writeFile.write(data);
                            writeFile.flush();
                            break;
                        }
                        writeFile.write(data);
                        writeFile.flush();
                        isSucceed = true;

                    }
                    if(readAllCount>=dataRealSize)
                        break;
                }
            }
            else
            {
                emit SignalStatusbarUpdata(Status_Text,"数据区已被覆盖,无法恢复 ...");
                qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" -- 数据区已被覆盖,无法恢复 ...";
                return;
            }
        }
        else if(dataInfo.x80_Att.NonResidentFlag == 0)
        {
            if(!writeFile.open(QIODevice::WriteOnly | QIODevice::Append))
            {
                emit SignalStatusbarUpdata(Status_Text,"数据恢复失败！");
                qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" -- 数据恢复失败!";
                return;
            }
            writeFile.write(dataInfo.residentData);
            writeFile.flush();
            isSucceed = true;
        }
        if(readFile.isOpen())
            readFile.close();
        if(writeFile.isOpen())
            writeFile.close();
        if(isSucceed)
        {
            emit SignalStatusbarUpdata(Status_Text,"数据恢复成功！");
            qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" -- 数据恢复成功!";
            return;
        }
        else
        {
            emit SignalStatusbarUpdata(Status_Text,"数据恢复失败！");
            qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" -- 数据恢复失败!";
            return;
        }
    });
}


//获取mftx80信息
bool CNtfsDataRecovery::getMFTDataInfo(QFile& file,S_RECOVER_INFO& info,S_RECOVER_DATA& dataInfo,QByteArray byteData)
{
    S_FILE_RECORD fileRecordHead;//文件头
    bool isOk = NTFS_MFTFileHeader(fileRecordHead,byteData);
    if(!isOk)
        return false;
    quint32 allLength = fileRecordHead.BytesInUse-4;//所有属性的大小
    quint32 byteOffset = fileRecordHead.AttributeOffset;
    int whileCount = allLength ;

    while(byteOffset<allLength && whileCount--)
    {
        if(whileCount == 0)//死循环异常
        {
            NTFS_setErrorType(CNTFS::WhileError);
            return false;
        }
        if(!(byteData.mid(byteOffset,sizeof (quint32)).toHex()).compare("FFFFFFFF",Qt::CaseInsensitive))
        {
            return true;
        }
        S_NTFS_ATTRIBUTE_HEADER attH;//属性头
        qint8 NonResidentFlag = NTFS_MFTAtt(attH,byteData.mid(byteOffset,sizeof(S_NTFS_ATTRIBUTE_HEADER)));//获取属性 返回0常驻，1非常驻，-1：失败
        if(NonResidentFlag == -1)
            return false;

        switch(attH.AttributeType)
        {
        //0x20
        case AttributeAttributeList:
        {
            byteOffset += attH.NonResidentFlag==0 ? attH.CCommon.CResident.StreamOffset : attH.CCommon.CNonResident.RunListOffset;
            if(attH.NonResidentFlag==0 && (attH.AttributeLength - attH.CCommon.CResident.StreamOffset)>0)
            {
                QList<S_ATTRIBUTE_0X20> x20L = NTFS_0X20Att(byteData.mid(byteOffset,attH.AttributeLength - attH.CCommon.CResident.StreamOffset));
                for(int i=0;i<x20L.count();i++)
                {
                    QList<MFTNUM> _temp;
                    //只获取不同的mft 属性x10和x30和x80
                    if((x20L.at(i).AttributeType == AttributeStandardInformation || x20L.at(i).AttributeType == AttributeFileName || x20L.at(i).AttributeType == AttributeData )
                            &&  x20L.at(i).Fdir.FileReferenceNumber != fileRecordHead.MFTRecordNumber)
                    {
                        if(!_temp.contains(x20L.at(i).Fdir.FileReferenceNumber))//以防多次执行，不同的mft只执行一次
                        {
                            _temp.append(x20L.at(i).Fdir.FileReferenceNumber);
                            quint64 offset = NTFS_MTFOffset(x20L.at(i).Fdir.FileReferenceNumber,info.mftTables);
                            if(offset == 0)
                                return false;
                            QByteArray data = getRawData(file,info.ntfsOffset + offset,BYTES_PER_MFT_FILE);
                            if(data.isNull())
                                return false;
                            bool isOk = getMFTDataInfo(file,info, dataInfo, data);
                            if(!isOk)
                            {
                                return false;
                            }
                        }
                    }
                }
                byteOffset += (attH.AttributeLength-attH.CCommon.CResident.StreamOffset);
            }
            else if(attH.NonResidentFlag==1 && (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset)>0)//预防
            {
                QList<S_DATARUN> DRunList;
                DRunList = NTFS_DatasRun(byteData.mid(byteOffset,attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset));
                for(int i=0;i<DRunList.count();i++)
                {
                    QByteArray data = getRawData(file,info.ntfsOffset + DRunList[i].dataStartCluster * BYTES_PER_CLUSTER,attH.CCommon.CNonResident.StreamRealSize);
                    if(!data.isNull())
                    {
                        QList<S_ATTRIBUTE_0X20> x20L = NTFS_0X20Att(data);
                        for(int i=0;i<x20L.count();i++)
                        {
                            QList<MFTNUM> _temp;
                            //只获取不同的mft 属性x10和x30和x80
                            if((x20L.at(i).AttributeType == AttributeStandardInformation || x20L.at(i).AttributeType == AttributeFileName || x20L.at(i).AttributeType == AttributeData )
                                    &&  x20L.at(i).Fdir.FileReferenceNumber != fileRecordHead.MFTRecordNumber)
                            {
                                if(!_temp.contains(x20L.at(i).Fdir.FileReferenceNumber))//以防多次执行，不同的mft只执行一次
                                {
                                    _temp.append(x20L.at(i).Fdir.FileReferenceNumber);
                                    quint64 offset = NTFS_MTFOffset(x20L.at(i).Fdir.FileReferenceNumber,info.mftTables);
                                    if(offset == 0)
                                        return false;
                                    QByteArray data = getRawData(file,info.ntfsOffset + offset,BYTES_PER_MFT_FILE);
                                    if(data.isNull())
                                        return false;
                                    bool isOk = getMFTDataInfo(file,info, dataInfo, data);
                                    if(!isOk)
                                    {
                                        return false;
                                    }
                                }
                            }
                        }
                    }
                }
                byteOffset += (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset);
            }
        }
            break;
            //0x80--存放文件的位置->获取文件的大小
        case AttributeData:
        {
            byteOffset += attH.NonResidentFlag==0 ? attH.CCommon.CResident.StreamOffset : attH.CCommon.CNonResident.RunListOffset;
            if(attH.NonResidentFlag==0 && (attH.AttributeLength - attH.CCommon.CResident.StreamOffset)>0)
            {
                if(dataInfo.CNonResidentData.isEmpty())//数据大小主要以非常驻为准，非常驻有数据就不获取常驻数据大小
                {
                    dataInfo.x80_Att = attH;
                    dataInfo.residentData = byteData.mid(byteOffset,attH.AttributeLength - attH.CCommon.CResident.StreamOffset);
                }
                byteOffset += (attH.AttributeLength-attH.CCommon.CResident.StreamOffset);
            }
            else if(attH.NonResidentFlag==1 && (attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset)>0)
            {
                QList<S_DATARUN> DRunList;
                DRunList = NTFS_DatasRun(byteData.mid(byteOffset,attH.AttributeLength - attH.CCommon.CNonResident.RunListOffset));
                dataInfo.x80_Att = attH;
                dataInfo.CNonResidentData = DRunList;

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
//判断已删除的文件是否已经被覆盖
bool CNtfsDataRecovery::fileIsNoCover(QFile& file,S_RECOVER_INFO& info,QList<S_DATARUN> CNonResidentData)
{
    if(MemoryPool::getInstance()->getSettingArgs().enableDeleteFileFlag && info.flag != NOEXISTING_FILE_FLAG)//设置启用正常文件可复制，，正常文件已经是被占用
        return true;
    quint64 offset = NTFS_MTFOffset(0x6,info.mftTables);
    if(offset == 0)
        return false;
    QByteArray mftByte = getRawData(file,info.ntfsOffset + offset,BYTES_PER_MFT_FILE);
    if(mftByte.isNull())
        return false;
    S_MFT_INFO mftInfo;
    bool mftInfoIsOk = NTFS_MFTFileInfo(mftInfo,mftByte);
    if(!mftInfoIsOk || mftInfo._x80.isEmpty() || mftInfo._x80.at(0).first.NonResidentFlag != 1)
        return false;
    QVariant varData = mftInfo._x80.at(0).second.data;
    if (!varData.canConvert<QList<S_DATARUN>>())
        return false;
    QList<S_DATARUN> source = varData.value<QList<S_DATARUN>>();
    if(source.count()==0)
        return false;
    if(CNonResidentData.isEmpty())
        return false;
    S_DATARUN tempDataRun;
    //获取数据运行区偏移位置非0的节点
    for(int i=0;i<CNonResidentData.count();i++)
    {
        if(CNonResidentData.at(i).dataStartCluster != 0)
        {
            tempDataRun = CNonResidentData.at(i);
            break;
        }
    }
    if(tempDataRun.dataStartCluster == 0)
        return false;
    quint32 bytesCount = tempDataRun.dataStartCluster / 8;//  求字节个数
    quint32 bitsOffsetCount = tempDataRun.dataStartCluster % 8;//  求位偏移个数


    //    quint32 fileClusterSize= 0;//系统已使用的簇数
    //    quint64 clusterOffset = 0;//bitmap数据区偏移
    //    //quint64 bitOnCount_1 = 0;//未使用的簇数
    //    quint64 byteOffset = 0;//字节偏移

    for(int i=0;i<source.count();i++)//通常只有一个
    {
        if(source.at(i).datalengthCluster * BYTES_PER_CLUSTER  <= bytesCount)//bitmap区大小小于
        {
            bytesCount -= source.at(i).datalengthCluster * BYTES_PER_CLUSTER;
            continue;
        }
        quint32 sectorOffsetCount = bytesCount / BYTES_PER_SECTION;// /8 求字节数量，求扇区偏移个数
        quint32 bytesOffsetCount = bytesCount % BYTES_PER_SECTION;//  求字节偏移个数

        quint64 dataOffset = info.ntfsOffset + source.at(i).dataStartCluster * BYTES_PER_CLUSTER;//获取bitmap数据区偏移地址

        //获取bitmap区数据
        qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" -- 数据在bitmap区占用首字节偏移:"<<dataOffset + sectorOffsetCount * BYTES_PER_SECTION+bytesOffsetCount;
        qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" -- 数据在bitmap区占用首位偏移:"<<bitsOffsetCount;

        if(!file.seek(dataOffset + sectorOffsetCount * BYTES_PER_SECTION))//跳转地址失败-报错
            return false;
        QByteArray tempRawdata = file.read(BYTES_PER_SECTION);
        if(tempRawdata.size() != BYTES_PER_SECTION)//数据读取不完整 -报错
            return false;
        quint8 bit = tempRawdata.at(bytesOffsetCount);
        if(!(bit>>bitsOffsetCount & 1))//判断是否为1
        {
            qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" -- 数据在bitmap区未被其他数据占用(0表示未占用 - 1表示占用):"<<(bit>>bitsOffsetCount);
            return true;
        }
        qDebug()<<m_name<<"信息:Mft:"<<info.mft<<" -- 数据在bitmap区被其他数据占用(0表示未占用 - 1表示占用):"<<(bit>>bitsOffsetCount);
    }

    return false;
}
