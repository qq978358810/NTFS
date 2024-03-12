#ifndef NTFS_H
#define NTFS_H


#include <IncludeFile.h>


class CNTFS : public QObject
{

    Q_OBJECT
public:
    enum NTFS_Error
    {
        NoError=0,//无错误
        WhileError=1,//死循环错误
        NoNTFSNumberErrot=2,//未找到mft号错误
        CopyDataError=3,//拷贝数据错误
        NoMftFileErrot=4,//不是mft文件
        NoNTFSSystemError=5,//不是nfts系统
        BadClusterError=6,//坏簇

        UnknownError=100//未知错误
    };
public:
    explicit CNTFS(QObject *parent = nullptr);
    ~CNTFS();
    bool NTFS_BootHeader(S_NTFS_BOOT&,QByteArray);//获取Boot头
    bool NTFS_isNTFS(S_NTFS_BOOT&);//判断是ntfs系统？
    quint64 NTFS_firstMFTOffset( S_NTFS_BOOT &) ;//返回首个MFT偏移位置
    quint64 NTFS_MTFOffset(MFTNUM,QList<S_MFT0_0X80_DATARUN>&);//获取MTF号的偏移地址
    QList<S_MFT0_0X80_DATARUN> NTFS_MFTTable(QList<S_DATARUN>&);//获取mft表数据运行区
    bool NTFS_MFTFileHeader(S_FILE_RECORD&,QByteArray&);//获取mft文件头
    bool NTFS_isFILE(S_FILE_RECORD&,QByteArray&);//判断是file记录格式？
    bool NTFS_fixup(S_FILE_RECORD&,QByteArray&);//USA和USN的建立称为fixup
    qint8 NTFS_MFTAtt(S_NTFS_ATTRIBUTE_HEADER&,QByteArray);//获取属性 返回0常驻，1非常驻，-1：失败
    qint8 NTFS_MFTUsnAtt(S_USN_JOURNAL_ATTRIBUTE_HEADER&,QByteArray);//获取usn属性 返回0常驻，1非常驻，-1：失败
    bool NTFS_0X10Att(S_ATTRIBUTE_0X10&,QByteArray);//获取0x10属性 常驻
    QList<S_ATTRIBUTE_0X20> NTFS_0X20Att(QByteArray);//获取0x20属性 常驻
    QByteArray NTFS_0X30AttName(S_ATTRIBUTE_0X30 &,QByteArray);//获取0x30属性名字(文件名) 常驻
    QList<QPair<S_INDEX_ROOT_ITEM_ATTRIBUTE_0X90,QString> > NTFS_0X90Att(QByteArray);//获取0x90属性 常驻
    QList<S_DATARUN> NTFS_DatasRun(QByteArray );//获取数据运行块LCN
    bool NTFS_MFTFileInfo(S_MFT_INFO&,QByteArray);//获取一个mft文件所有信息

    NTFS_Error NTFS_error();//获取错误类型
    QString NTFS_errorString();//获取错误类型字符串
    void NTFS_unsetError();//复位错误为NoError
    void NTFS_setErrorType(NTFS_Error);//设置异常类型
protected:
    QString m_name_uuid;

private:

    NTFS_Error m_errorType;
};

//==============================================================================

class CGPT : public QObject
{
    Q_OBJECT
public:
    explicit CGPT(QObject *parent = nullptr);
    QList<S_NTFS_INFO> allDrive();//保存所有GPT物理设备信息->allPDrive

    static CGPT * getInstance();

protected:
    QList<S_GPT_INFO> GPT_Drive();//保存所有GPT物理设备下ntfs信息
    bool GPT_isGPT(S_EFI_GPT_HEADER GPT)const;//判断是否为GPT格式
signals:

private:
    static CGPT *m_instance;
    QString m_name;
};

#endif // NTFS_H
