#ifndef NTFSDATARECOVERY_H
#define NTFSDATARECOVERY_H

#include <QObject>
#include "IncludeFile.h"
#include "ntfs.h"

//保存mft数据 -- 数据恢复结构
typedef struct _STRUCT_RECOVER_DATA
{
    S_NTFS_ATTRIBUTE_HEADER x80_Att; //x80属性头
    QByteArray residentData;//常驻数据
    QList<S_DATARUN> CNonResidentData;//非常驻运行地址
}S_RECOVER_DATA,*PS_RECOVER_DATA;


class CNtfsDataRecovery : public CNTFS
{
    Q_OBJECT
public:
    explicit CNtfsDataRecovery(QObject *parent = nullptr);
    static CNtfsDataRecovery* getInstance();
protected:
    QByteArray getRawData(QFile& file, quint64 offset,qint32 size);
    bool getMFTDataInfo(QFile& file,S_RECOVER_INFO& info,S_RECOVER_DATA& dataInfo,QByteArray byteData);//获取mftx80信息
    bool fileIsNoCover(QFile& file,S_RECOVER_INFO& info,QList<S_DATARUN> CNonResidentData);//判断已删除的文件是否已经被覆盖

signals:
    void SignalStatusbarUpdata(E_STATUSBAR_UPDATA,QVariant);//更新状态栏
public slots:
    void recovery(S_RECOVER_INFO info);//恢复数据

private:
    static QAtomicPointer<CNtfsDataRecovery> m_instance;//单例对象
    static QMutex m_mutex;
    QString m_name;


};

#endif // NTFSDATARECOVERY_H
