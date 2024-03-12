#ifndef DATAMANAGE_H
#define DATAMANAGE_H

#include <QObject>
#include <QElapsedTimer>
#include "IncludeFile.h"
#include "ntfs.h"
#include "ntfssysinfo.h"

QT_BEGIN_NAMESPACE
class QTimer;
class QThread;
class MySQLite;
QT_END_NAMESPACE
typedef struct _STRUCT_COMBINATION
{
    QThread* thread;
    CNtfsInfo *   p;
    S_NTFS_INFO info;

}S_COMBINATION;
class DataManage : public QObject
{
    Q_OBJECT
public:
    explicit DataManage(QObject *parent = nullptr);
    ~DataManage();


    void addDriver(S_NTFS_INFO);//添加设备
    QString findFilePath(const quint8 ,quint32) ;//根据mft号查找路径
    QIcon getFileIcon(const quint8 ,const QString ) ;//获取文件图标icon
    void getAllExtensionName(QString &); //获得所有扩展名
    void resetData();
    void beData();
    void copyData(S_DATA_BODY* dest,S_DATA_BODY* src);
    void createDataList();//创建数据列表

signals:
    void SignalDataUpdateLog(E_LOG_TYPE,QByteArray);//记录数据改变日志
    void SignalDataChange(E_DATABASE_TYPE);
    void SignalUsnRecordLog(E_LOG_TYPE,QVariant);
    void SignalFail();//获取盘符失败
    void SignalSucceed();//获取盘符成功
public slots:
    void initData(E_DATABASE_TYPE,S_INIT_DATAS_MODEL);   //设置表格数据
    void SlotDataBase(S_DATA_SUBSET);//数据库操作
    void SlotInit();//线程开始初始换
    void SlotTimerOut();
    void SlotExitThread(S_NTFS_INFO  ntfsInfo);
    void SlotFinish();

private:

    QList<S_INIT_DATAS_MODEL> m_delPartitonData;//保存要删除的盘符数据
    int m_clearDelPartitonData;
    QList<S_DATA_BODY*> m_delData;//保存要删除的数据
    QList<S_DATA_BODY*> m_tempDelData;//
    quint8 serial;//最大255个序号，表示255个盘符
    quint16 deriveCount;//需要接收的盘符个数，确保接收完整
    quint32 addCount;//累计接收改变数据的个数

    QTimer* m_timerUpdateData;
    QTimer* m_timerUpdateDrive;
    QElapsedTimer elapsedTimer;//保存经过时间

    MySQLite *sqlite;

    QList<S_NTFS_INFO> m_allPartition;
    QList<S_COMBINATION> threadQueue;//保存所有线程//管理内存

    QString m_name;
};

#endif // DATAMANAGE_H
