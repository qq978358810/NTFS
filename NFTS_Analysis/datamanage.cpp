#include "datamanage.h"
#include <QThread>
#include <QTimer>
#include "Database/mysqlite.h"
DataManage::DataManage(QObject *parent) : QObject(parent)
{

    serial = 0;
    deriveCount = 0;
    addCount=0;
    m_clearDelPartitonData =0;
    m_name = "DatasManage";
    m_name += SPLIT_SYMBOLS;
    qDebug()<<m_name<<"构造...";
}

DataManage::~DataManage()
{
    qDebug()<<m_name<<"析构~";
}
//线程开始初始换
void DataManage::SlotInit()
{
    qDebug()<<m_name<<"开启线程:"<<QThread::currentThread();
    m_allPartition = CGPT::getInstance()->allDrive();
    //初始话设备列表失败，数据为空：原因：请以管理员身份运行程序!
    if(m_allPartition.isEmpty()){
        qDebug()<<m_name<<"初始化设备列表失败:列表为空!";
        emit SignalFail();//获取盘符失败
        return;
    }else{
        qDebug()<<m_name<<"初始化设备列表成功:列表个数:"<<m_allPartition.count();
        emit SignalSucceed();//获取盘符成功
    }




    //定时器--节流函数
    m_timerUpdateDrive = new QTimer();
    connect(m_timerUpdateDrive,&QTimer::timeout,this,&DataManage::SlotTimerOut);
    m_timerUpdateDrive->setTimerType(Qt::PreciseTimer);//如果不设置，默认为CoarseTimer类型
    m_timerUpdateDrive->start(5000);

    //定时器--防抖函数
    m_timerUpdateData = new QTimer();//定时器的创建和调用必须在同一个线程
    if(!m_timerUpdateData->isSingleShot()){
        m_timerUpdateData->setSingleShot(true);//设置时间后，只触发一次，单次触发
    }
    connect(m_timerUpdateData,&QTimer::timeout,this,[&]()mutable
    {
        beData();
    });
    elapsedTimer.start();

    //开启多线程采集各个盘数据
    for(int i=0;i<m_allPartition.count();++i)
        addDriver(m_allPartition.at(i));
    GlobalFunction::getInstance()->fileIcons().swap(*MemoryPool::getInstance()->getIconList());//找出所有扩展名的图标icon


}
void DataManage::SlotTimerOut()
{

    bool partitionName = false;//盘符名发生改变为true
    if(!MemoryPool::getInstance()->getIsFinish())
        return;
    m_allPartition = CGPT::getInstance()->allDrive();
    qDebug()<<m_name<<"重新获取设备列表成功:列表个数:"<<m_allPartition.count();
    QList<S_NTFS_INFO> temp_add,temp_remove;
    //筛选增加
    for(int i=0;i<m_allPartition.count();i++){
        bool isOk = true;
        for(int j=0;j<MemoryPool::getInstance()->getInfoList()->count();j++){
            if(m_allPartition.at(i).uuid == MemoryPool::getInstance()->getInfoList()->at(j).info.uuid){
                if(m_allPartition.at(i).partitionFlagName != MemoryPool::getInstance()->getInfoList()->at(j).info.partitionFlagName)
                    partitionName = true;
                (*MemoryPool::getInstance()->getInfoList())[j].info = m_allPartition.at(i);
                isOk = false;
                break;
            }
        }
        if(isOk){
            qDebug()<<m_name<<"筛选增加:"<<m_allPartition.at(i).pDrive<<" --- "<<m_allPartition.at(i).partitionFlagName <<" --- "<<m_allPartition.at(i).uuid;
            addDriver(m_allPartition.at(i));
        }
    }
    //筛选删除
    for(int i=0;i<MemoryPool::getInstance()->getInfoList()->count();i++){
        bool isOk = true;
        for(int j=0;j<m_allPartition.count();j++){
            if(m_allPartition.at(j).uuid == MemoryPool::getInstance()->getInfoList()->at(i).info.uuid){
                isOk = false;
                break;
            }
        }
        if(isOk){
            qDebug()<<m_name<<"筛选删除:"<<MemoryPool::getInstance()->getInfoList()->at(i).info.pDrive<<" --- "<<MemoryPool::getInstance()->getInfoList()->at(i).info.partitionFlagName <<" --- "<<MemoryPool::getInstance()->getInfoList()->at(i).info.uuid;
            SlotExitThread(MemoryPool::getInstance()->getInfoList()->at(i).info);
        }
    }

    //刷新列表model
    if(partitionName)
        MemoryPool::getInstance()->initModelList();//初始化列表

}

//添加设备
void DataManage::addDriver(S_NTFS_INFO item)
{
    qDebug()<<m_name<<"增加一个监控设备:"<<item.pDrive<<" --- "<<item.partitionFlagName <<" --- "<<item.uuid;
    S_COMBINATION combination;
    QThread * driverThread = new QThread();
    CNtfsInfo *NInfo = new CNtfsInfo(item);
    combination.p = NInfo;
    combination.info = item;
    combination.thread = driverThread;
    threadQueue.append(combination);

    connect(NInfo,&CNtfsInfo::SignalInitDataBase,this,&DataManage::initData,Qt::QueuedConnection);//初始化数据到数据库
    connect(NInfo,&CNtfsInfo::SignalDataBase,this,&DataManage::SlotDataBase,Qt::QueuedConnection);//更新
    connect(NInfo,&CNtfsInfo::SignalExitThread,this,&DataManage::SlotExitThread,Qt::QueuedConnection);//结束一个线程
    connect(NInfo,&CNtfsInfo::SignalUsnRecordLog,this,&DataManage::SignalUsnRecordLog,Qt::QueuedConnection);//日志界面更新

    NInfo->moveToThread(driverThread);
    connect(driverThread,&QThread::started,NInfo,&CNtfsInfo::startRun,Qt::QueuedConnection);//Qt::QueuedConnection
    driverThread->start();

    S_INIT_DATAS_MODEL tempModel;
    tempModel.info = item;
    MemoryPool::getInstance()->getInfoList()->append(tempModel);

}
//结束一个线程
void DataManage::SlotExitThread(S_NTFS_INFO  ntfsInfo)
{
    qDebug()<<m_name<<"结束一个监控设备:"<<ntfsInfo.pDrive<<" --- "<<ntfsInfo.partitionFlagName <<" --- "<<ntfsInfo.uuid;
    for(int i=0;i<MemoryPool::getInstance()->getInfoList()->count();i++){
        if(ntfsInfo.uuid == MemoryPool::getInstance()->getInfoList()->at(i).info.uuid){
            m_delPartitonData.append(MemoryPool::getInstance()->getInfoList()->at(i));
            MemoryPool::getInstance()->getInfoList()->removeAt(i);
            MemoryPool::getInstance()->initModelList();//初始化列表
            createDataList();
            emit SignalDataChange(UpdataToDataBase);
            break;
        }
    }

    for(int i=0;i<threadQueue.count();i++){
        if(ntfsInfo.uuid == threadQueue.at(i).info.uuid){
            S_COMBINATION combination = threadQueue.takeAt(i);
            combination.p->disconnect();
            combination.thread->requestInterruption();
            combination.thread->quit();
            combination.thread->wait();
            break;
        }
    }

}
//结束程序
void DataManage::SlotFinish()
{
    //定时器结束
    m_timerUpdateDrive->stop();

    //停止线程
    for(int i=0;i<threadQueue.count();i++){
        S_COMBINATION combination = threadQueue.at(i);
        combination.p->disconnect();
        combination.thread->requestInterruption();
    }
    //数据库保存 -- 单线程
    if(MemoryPool::getInstance()->getSettingArgs().dataBaseLoad){
        sqlite = new MySQLite("DatasManage",this);
        sqlite->initWriteDatas();
    }
    //结束线程
    if(!threadQueue.isEmpty()){
        while(threadQueue.count()>0){
            S_COMBINATION combination = threadQueue.takeLast();
            combination.thread->quit();
            combination.thread->wait();
        }
    }

    qDebug()<<m_name<<"结束所有监控设备!";
}
//初始化
void DataManage::initData(E_DATABASE_TYPE type,S_INIT_DATAS_MODEL initDatas)
{
    if(type == InitDataBase){
        for(int i=0;i<MemoryPool::getInstance()->getInfoList()->count();i++){
            if(QUuid(initDatas.info.uuid) == QUuid(MemoryPool::getInstance()->getInfoList()->at(i).info.uuid)){
                if(!MemoryPool::getInstance()->getInfoList()->at(i).dataBodys.isEmpty())
                    m_delPartitonData.append(MemoryPool::getInstance()->getInfoList()->at(i));
                MemoryPool::getInstance()->getInfoList()->removeAt(i);
                MemoryPool::getInstance()->initModelList();//初始化列表
                createDataList();
                emit SignalDataChange(UpdataToDataBase);
            }
        }
        int tempSerial = 0;
        //找出可用的序号
        for(;;){
            tempSerial++;
            bool isOK = false;
            for(int i=0;i<MemoryPool::getInstance()->getInfoList()->count();i++){
                if(tempSerial == MemoryPool::getInstance()->getInfoList()->at(i).partitionSerial){
                    isOK = true;
                    break;
                }
            }
            if(!isOK)
                break;
        }

        initDatas.partitionSerial = tempSerial;//给这个盘唯一的序号
        QHash<quint32,S_DATA_BODY*>::iterator i =initDatas.dataBodys.begin();
        while (i != initDatas.dataBodys.end()){
            //对每个数据的序号进行赋值，用来标识数据的位置
            i.value()->partitionSerial = initDatas.partitionSerial;
            //获取所有属性
            MemoryPool::getInstance()->getAttList()->insert(QString::number(i.value()->bodyAtt.FileAttributes),QIcon());
            ++i;
        }
        MemoryPool::getInstance()->getInfoList() ->append(initDatas);
        qDebug()<<m_name<<"增加数据:"<<initDatas.dataBodys.count()<<" 个 , 盘符:"<<initDatas.info.partitionFlagName;

        if((++deriveCount) >= m_allPartition.count()){
            MemoryPool::getInstance()->initModelList();//初始化列表
            createDataList();
            emit SignalDataChange(UpdataToDataBase);
            MemoryPool::getInstance()->setIsFinish(true);
            qDebug()<<m_name<<"初始化完成：数据总个数:"<<MemoryPool::getInstance()->getModelData()->count()<<" -- 扩展名Count:"<<MemoryPool::getInstance()->getIconList()->count()<<" --- 初始化时间"<<elapsedTimer.elapsed()<<"ms";

        }
    }

}
//创建数据列表
void DataManage::createDataList()
{

    QList<S_DATA_BODY*> __temp;
    //初始化列表
    quint64 alloc=0;
    for(int i=0;i<MemoryPool::getInstance()->getInfoList()->count();i++){
        alloc += MemoryPool::getInstance()->getInfoList()->at(i).dataBodys.count();
    }
    __temp.reserve(alloc);//分配空间大小
    for(int i=0;i<MemoryPool::getInstance()->getInfoList()->count();++i){
        __temp.append(MemoryPool::getInstance()->getInfoList()->at(i).dataBodys.values());
    }
    MemoryPool::getInstance()->getModelData()->swap(__temp);

    if(!m_delPartitonData.isEmpty()){
        if(++m_clearDelPartitonData>=2){
            S_INIT_DATAS_MODEL temp = m_delPartitonData.takeAt(0);
            if(!temp.dataBodys.isEmpty())
                qDeleteAll(temp.dataBodys);
            qDebug()<<m_name<<temp.info.partitionFlagName<<temp.info.uuid<<" --- 释放内存:"<<temp.dataBodys.count()<<" 个数据";
            temp.dataBodys.clear();
            m_clearDelPartitonData=0;
        }
    }
}

void DataManage::copyData(S_DATA_BODY* dest,S_DATA_BODY* src)
{
    dest->name = src->name;
    dest->MFTNumber = src->MFTNumber;
    dest->PMFTNumber = src->PMFTNumber;
    dest->partitionSerial = src->partitionSerial;
    dest->bodyAtt = src->bodyAtt;
}

//更新数据表
void DataManage::SlotDataBase(S_DATA_SUBSET data)
{

    for(int i=0;i<MemoryPool::getInstance()->getInfoList()->count();i++){
        //寻找同一个数据盘
        if(QUuid(MemoryPool::getInstance()->getInfoList()->at(i).info.uuid) == QUuid(data.guid)){
            quint32 dataBodyLPos = data.dataBodyL.count();
            quint32 hashDataCount = MemoryPool::getInstance()->getInfoList()->at(i).dataBodys.count(data.mft);
            bool once = true;
            for(quint32 j=0;j<dataBodyLPos;j++)
                data.dataBodyL.at(j)->partitionSerial = MemoryPool::getInstance()->getInfoList()->at(i).partitionSerial;//序号赋值
            if(dataBodyLPos){
                //统计数量加
                switch (data.dataBodyL.at(0)->bodyAtt.flag){
                case NOEXISTING_FILE_FLAG:
                    (*MemoryPool::getInstance()->getInfoList())[i].dataBodyCount._falg_0_count += dataBodyLPos;
                    break;
                case EXISTING_FILE_FLAG:
                    (*MemoryPool::getInstance()->getInfoList())[i].dataBodyCount._falg_1_count += dataBodyLPos;
                    break;
                case NOEXISTING_DIRECTORY_FLAG:
                    (*MemoryPool::getInstance()->getInfoList())[i].dataBodyCount._falg_2_count += dataBodyLPos;
                    break;
                case EXISTING_DIRECTORY_FLAG:
                    (*MemoryPool::getInstance()->getInfoList())[i].dataBodyCount._falg_3_count += dataBodyLPos;
                    break;
                default:
                    (*MemoryPool::getInstance()->getInfoList())[i].dataBodyCount._falg_other_count += dataBodyLPos;
                    break;
                }

            }

            QHash<quint32,S_DATA_BODY*>::iterator iterH = (*MemoryPool::getInstance()->getInfoList())[i].dataBodys.find(data.mft);
            //更新数据
            while (iterH != MemoryPool::getInstance()->getInfoList()->at(i).dataBodys.end() && iterH.key() == data.mft){
                if(once){
                    //统计数量--
                    switch (iterH.value()->bodyAtt.flag){
                    case NOEXISTING_FILE_FLAG:
                        (*MemoryPool::getInstance()->getInfoList())[i].dataBodyCount._falg_0_count -= hashDataCount;
                        break;
                    case EXISTING_FILE_FLAG:
                        (*MemoryPool::getInstance()->getInfoList())[i].dataBodyCount._falg_1_count -= hashDataCount;
                        break;
                    case NOEXISTING_DIRECTORY_FLAG:
                        (*MemoryPool::getInstance()->getInfoList())[i].dataBodyCount._falg_2_count -= hashDataCount;
                        break;
                    case EXISTING_DIRECTORY_FLAG:
                        (*MemoryPool::getInstance()->getInfoList())[i].dataBodyCount._falg_3_count -= hashDataCount;
                        break;
                    default:
                        (*MemoryPool::getInstance()->getInfoList())[i].dataBodyCount._falg_other_count -= hashDataCount;
                        break;
                    }
                    once =false;
                }

                //列表中的数据大于0
                if(dataBodyLPos){
                    copyData(iterH.value(),data.dataBodyL.at(--dataBodyLPos));//替换原来的数据
                    delete data.dataBodyL.at(dataBodyLPos);
                    data.dataBodyL[dataBodyLPos] = NULL;
                    iterH++;
                }else{
                    m_delData.append(iterH.value());
                    iterH = (*MemoryPool::getInstance()->getInfoList())[i].dataBodys.erase(iterH);
                }
            }

            //添加未替换的数据
            for(quint32 j=0;j<dataBodyLPos;j++){
                //保存未来删除的数据为空，直接添加
                if(m_delData.isEmpty()){
                    (*MemoryPool::getInstance()->getInfoList())[i].dataBodys.insertMulti(data.mft,data.dataBodyL.at(j));

                }else{
                    //保存未来删除的数据不为空，取出头部的地址，更换要插入的数据，插入hash列表
                    S_DATA_BODY * _temp = m_delData.takeFirst();//取出第一个未删除的数据
                    copyData(_temp,data.dataBodyL.at(j));
                    delete data.dataBodyL.at(j);
                    data.dataBodyL[j] = NULL;
                    (*MemoryPool::getInstance()->getInfoList())[i].dataBodys.insertMulti(data.mft,_temp);
                }

            }
            if(MemoryPool::getInstance()->getIsFinish()){
                //同时接收大量数据超过设定值，更新界面
                if(addCount++ >= ENDCOUNT /*|| duration>=1000*/){
                    beData();
                    return;
                }
                m_timerUpdateData->setInterval(100);//设置时间间隔超出触发timeOut信号，下次执行timer->setInterval(1000);会覆盖当前触发信号，继续等待1000；
                m_timerUpdateData->start();
            }

        }
    }
}
//处理数据
void DataManage::beData()
{
    createDataList();
    addCount = 0;
    //最少一秒更新一次
    if(elapsedTimer.elapsed()>1000){
        emit SignalDataChange(UpdataToDataBase);
        elapsedTimer.restart();
    }
}

