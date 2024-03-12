#ifndef NTFSSYSINFO_H
#define NTFSSYSINFO_H

#include "IncludeFile.h"
#include "ntfs.h"
//定义mft基本信息
typedef struct _MFT_BASIC_INFO
{
    S_DATA_BODY_ATT attBody;//属性体
    QList<S_DATA_BODY*> fileL;//文件列表--一个mft可能存在多个文件公用一个数据
    MFTNUM ExMFT = 0 ;//扩展mft--默认为0

}MFT_BASIC_INFO,*PMFT_BASIC_INFO;

class CNtfsInfo : public CNTFS
{
    Q_OBJECT
public:
    enum Error
    {
        NoError=0,//无错误
        InitError,//直接返回
        AnewInit//重新初始化

    };
    explicit CNtfsInfo(S_NTFS_INFO ntfsInfo, QObject *parent = nullptr);//物理盘，或逻辑盘
    ~CNtfsInfo();
    bool openNTFS();
    bool initDrive();//初始化ntfs
    QString getmVolumeName();//获取卷名
    QByteArray getRawData(quint64 offset,qint32 size);//从offset位置获取size大小数据
    bool getMFTBasicInfo(MFT_BASIC_INFO& basicInfo,QByteArray byteData);//获取文件的基本信息
    quint64 usedSpaceSize();//通过bitmap获取磁盘已经使用的空间大小
    quint64 getMTFOffset(MFTNUM MTFNumber);//获取mft偏移
    S_MFT_USN_JOURNAL getUsnJournalBasicInfo();//获取usnJournal基本信息
    MFTNUM getSaveUsnLogPMft(QString);//获取保存usn记录文件夹的mft号
    void getAllFilesBasicInfo();//获取所有文件基本信息

    void dataCountAdd(quint8 flag);//统计数据个数
    void updataDataBaseV2( PUSN_RECORD_UNION,quint64 usnRecordOffset=0);//更新数据库v2
    void updataDataBaseV3( PUSN_RECORD_UNION);//更新数据库v3
    void updataDataBaseV4( PUSN_RECORD_UNION);//更新数据库v4
    void usnStartRun(S_MFT_USN_JOURNAL);//开启usn采集
    void usnRecordMonitoring();//监控usn记录
    void initData();
    Error anewInitDrive();//重新初始化


private:
    bool _getUsnJournalBasicInfo(S_MFT_USN_JOURNAL& usnJournalBasicInfo,QByteArray byteData);//获取usnJournal基本信息
signals:
    void SignalDataBase(S_DATA_SUBSET);//记录数据库操作，其他线程写入
    void SignalInitDataBase(E_DATABASE_TYPE,S_INIT_DATAS_MODEL);//初始化数据库
    void SignalUsnRecordLog(E_LOG_TYPE,QVariant);//记录usn日志

    void SignalExitThread(S_NTFS_INFO );//结束线程

public slots:

    void SlotExternalExitThread();//被动结束线程
    void SlotInternalExitThread();//主动结束线程
    void startRun();//开起线程

public:
    QFile * m_fileHandle;//文件指针
    QString m_partitionName;//盘符名
    QUuid m_partitionuuid;             //uuid

    quint64 m_NTFSOffset;//开始偏移
    S_INIT_DATAS_MODEL m_initDatasInfo;//保存初始化NTFS信息
    QString m_volumeName;//卷名
    QList<S_MFT0_0X80_DATARUN>  m_tempMftTable;//初始化代用
    QList<S_MFT0_0X80_DATARUN>  m_mftTable;//mft表
    S_MFT_USN_JOURNAL m_mftUsnJournal;
    QMutex m_mutex;
    PUSN_RECORD_UNION m_usnRecordUnion;
    quint64 m_lastUsnNumber;//最后一个usn记录ID
    bool m_threadRunFlag;//线程运行标志
    bool m_callTempFile; //false :不采集本程序所产生的临时文件；true：采集本程序所产生的临时文件
    bool m_isRootDir;////false :不是根目录；true：是根目录
    bool m_firstAnew;//开启usn记录监控第一次TRUE，第二次（包含第二次）以后为false

    //保存记录文件夹的Pmft号  -1:正常写入日志  其他：判断相等不写入log
    MFTNUM LOG_PATH_PMft;
    MFTNUM USN_RECORD_PATH_PMft;
    MFTNUM DATA_UPDATE_PATH_PMft;
    MFTNUM RUN_LOG_PATH_PMft;

};



#endif // NTFSSYSINFO_H
