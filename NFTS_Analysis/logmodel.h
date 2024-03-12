#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QObject>
#include <QElapsedTimer>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QDebug>
#include <QTimer>
#include "readwritexml.h"
#include "IncludeFile.h"
#define TABLENAME QString("usnjournal")
//实时日志model
class RealLogModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit RealLogModel(QString connectName, QObject *parent = nullptr);

    ~RealLogModel();
    //开启线程
    void init();
    //更新view
    void beData();

    //返回行数
    int rowCount(const QModelIndex& parent) const;
    //返回列数
    int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;
    //返回指定表头数据
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    //写入数据到数据库
    void writeDatas();
    //连接数据库
    bool connectDB();
    //创建表
    bool createTable();

signals:
    //更新状态栏
    void SignalStatusbarUpdata(E_STATUSBAR_UPDATA,QVariant);
public slots:
    //接收数据
    void SlotAddData(S_USN_LOG);
    //清空数据
    void SlotClear();
    //导出
    void SlotExportData(const QString &fileName);
private:
    QList<S_USN_LOG> m_datas;   //显示数据
    QList<S_USN_LOG> m_tempDatas;//更新队列


    QTimer *timer;
    QSqlDatabase MySQLite_db;
    QString m_connectName;//连接名

    QMutex m_write;
    int left_VCenter = (Qt::AlignLeft | Qt::AlignVCenter); //左对齐 | 垂直居中

};
//实时监控usn日志model
class RealMonitorModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit RealMonitorModel( QObject *parent = nullptr);

    ~RealMonitorModel();
    //开启线程
    void init();

    //返回行数
    int rowCount(const QModelIndex& parent) const;
    //返回列数
    int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;
    //返回指定表头数据
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    bool dataCompare(const S_MONITOR_ITEM &,const S_USN_LOG &);

    QList<S_MONITOR_ITEM> & getDataList();
    quint64 & getTime();
public slots:
    //接收usn数据
    void SlotReceiveUsnData(S_USN_LOG);
    //接收数据
    void SlotChangeData(E_CHANGE,S_MONITOR_ITEM,int);
    //接收执行最低时长
    void SlotReceiveInterval(quint64);
    //获取某行数据
    S_MONITOR_ITEM SlotGetRowData(int);
    //获取最低时长执行
    quint64 SlotGetElapsed();


private:
    QList<S_MONITOR_ITEM> m_datas;   //显示数据
    //QList<S_MONITOR_ITEM> m_tempDatas;//更新队列
    //QElapsedTimer m_reduce;//节流执行
    quint64 m_interval;//间隔时长 单位毫秒
    QMutex m_dataMutex;
    QMutex m_timeMutex;
    ReadWriteXml m_xml;

    int left_VCenter = (Qt::AlignLeft | Qt::AlignVCenter); //左对齐 | 垂直居中
};

#endif // LOGMODEL_H
