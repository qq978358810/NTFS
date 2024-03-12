#ifndef MYTABLEMODEL_H
#define MYTABLEMODEL_H

#include <QElapsedTimer>
#include "IncludeFile.h"

class QAbstractTableModel;

class MyTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit MyTableModel(S_UPDATE_VIEW item,QObject *parent = nullptr);

    ~MyTableModel();
    int rowCount(const QModelIndex& parent) const;  //返回行数
    int columnCount(const QModelIndex& parent) const;   //返回列数
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;  //返回指定表头数据
    QByteArray findFilePath(const quint8 ,quint32) const;//根据mft号查找路径
    QIcon getFileIcon(const quint8 ,const QString ) const;//获取文件图标icon
    void getAllExtensionName(QString &); //获得所有扩展名

    S_PARTITION_DATA_COUNT getDataCount();//获取数据的数量
    void updataView();//更新界面
    void reverseData(QList<S_DATA_BODY*>*);//翻转数据


signals:
    void SignalDataBaseSave(QList<S_INIT_DATAS_MODEL>);//数据保存到数据库
    void SignalExtendList(QHash<QString,QIcon>);//发送界面显示扩展名
    void SignalStatusbarUpdata(E_STATUSBAR_UPDATA,QVariant);//更新状态栏

public slots:

    void initData();   //设置表格数据
    void SlotDataChange(E_DATABASE_TYPE);//更新数据
    void SlotExportFileList(const QString &fileName);//导出文件列表

    void SlotSearchData(S_UPDATE_VIEW);//按照启用项搜索数据
    void SlotSortData(S_UPDATE_VIEW);//按照启用项排序数据
    void SlotHighFindData(S_HIGHFIND_UPDATE_VIEW);//高级搜索数据

private:
    void searchData(QList<S_DATA_BODY*>&,S_UPDATE_VIEW);
    void sortData(QList<S_DATA_BODY*>&,S_UPDATE_VIEW);
private:

    QList<S_DATA_BODY*>* m_datas;            //view显示的数据
    QList<S_DATA_BODY*>* m_datasTemp;             //配合m_datas使用
    QList<S_DATA_BODY*>* m_datasSearch;             //保存搜索框数据
    QList<S_DATA_BODY*>* m_datasExtend;             //保存扩展名数据

    QMutex _mutex;
    QMutex _Search;
    QMutex _Extend;
    QTimer* timer;
    QString textChangeValue;//保存搜索框每次改变的值
    QString extendChangeValue;//保存扩展值

    bool isEnableComplete = false;//是否启用全字匹配
    bool isEnableCaseSensitive = false;//是否启用区分大小写
    QElapsedTimer m_elapsedT;
    quint64 addTime=0;
    S_UPDATE_VIEW m_updateViewDataStruct;//搜索-保存当前界面显示参数
    S_HIGHFIND_UPDATE_VIEW m_updateViewDataHighFindStruct;//高级搜索——保存当前界面显示参数
    bool m_isHighFind;//当前搜索是高级搜索为true


    int left_VCenter = (Qt::AlignLeft | Qt::AlignVCenter); //左对齐 | 垂直居中
};

#endif // MYTABLEMODEL_H
