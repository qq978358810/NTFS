/*
 * http://ftp.kolibrios.org/users/Asper/docs/NTFS/ntfsdoc.html#id4757298  //NTFS资料
 * https://www.dgxue.com/huifu/luoji/windwos-huifu/ntfs/                  //NTFS数据恢复
 *
 * LCN号为簇号  从0开始  -- 比如：LCN0就是  boot头
 */


#ifndef INCLUDEFILE_H
#define INCLUDEFILE_H

//#include <QMainWindow>
#include <Windows.h>
#include <QUuid>
#include <QHash>
#include <QStorageInfo>
#include <QSettings>
#include <QtWin>
#include <QFileIconProvider>//获取文件图标icon
#include <QTemporaryFile>//用于创造临时文件
#include <QAbstractListModel>
#include <QMutex>
#include <QGuiApplication>
#include <QSet>
#include <QDateTime>
#include <shellapi.h>
#include <ole2.h>


QT_BEGIN_NAMESPACE
class WindowClass;
QT_END_NAMESPACE


//#define ； ;  //中文分号预定义英文分号
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"Shell32.lib")   //引入库解决这个问题:  (mytablemodel.obj:-1: error: LNK2019: 无法解析的外部符号 __imp__SHGetFileInfoW@20，                                          
//该符号在函数 "public: static class QIcon __cdecl GlobalFunction::fileIcon(class QString)" (?fileIcon@GlobalFunction@@SA?AVQIcon@@VQString@@@Z) 中被引用))

#define DATE qPrintable(QDate::currentDate().toString("yyyy.MM.dd"))
#define DATETIMS qPrintable(QDateTime::currentDateTime().toString("yyyy.MM.dd-hh:mm:ss.zzz"))

#define N_BOOT_SIZE         sizeof(_STRUCT_NTFS_BOOT)
#define N_FILEHEAD_SIZE     sizeof(_STRUCT_FILE_RECORD)
#define N_EFI_GPT_SIZE      sizeof(_STRUCT_EFI_GPT_HEADER) //GPT头分区大小
#define N_PAR_ITEM_SIZE     sizeof(_STRUCT_PARTITION_ITEM) //分区项大小
#define N_ATTRIBUTEH_SIZE     sizeof(_STRUCT_NTFS_ATTRIBUTE_HEADER) //属性头大小

//对应g_ModelHeadL 位置
typedef enum _ENUM_HEAD_TYPE{
    T_MFT=0,T_PMFT,T_FILEATTRIBUTE,T_FILENAME,T_FILEPATH,T_CREATIONTIME,T_CHANGETIME,T_LASTREADTIME,T_REALSIZE,T_ALLOCSIZE,T_FILETYPE,T_PARTITION
} E_HEAD_TYPE;
//数据库表头定义
static QStringList g_SqliteHeadL = QStringList()<<"MftNumber"<<"PMftNumber"<<"FileAttribute"<<"FileName"<<"CreationTime"<<"ChangeTime"<<"LastReadTime"<<"RealSize(Byte)"<<"AllocSize(Byte)"<<"FileType"<<"PartitionFlag(non)";
//数据界面view表头定义
static QStringList g_ModelHeadL = QStringList()<<"Mft号"<<"PMft号"<<"文件属性"<<"文件名"<<"文件路径"<<"创建时间"<<"修改时间"<<"访问时间"<<"实际大小(Byte)"<<"占用空间(Byte)"<<"文件类型"<<"盘符标识(non)";



//文件类型定义
static QStringList g_FileFlagL = QStringList()<<"删除文件"<<"正常文件"<<"删除目录"<<"正常目录"<<"系统文件";
//usn日志界面头定义
static QStringList g_UsnLogTitle = QStringList()<<"采集时间"<<"盘符"<<"Usn记录偏移(相对盘符)"<<"MFT"<<"PMFT"<<"Usn号"<<"文件变化时间"<<"文件变化原因"<<"文件属性"<<"文件名";
//usn日志数据库头定义
static QStringList g_UsnLogSqlTitle = QStringList()<<"AcquisitionTime"<<"Partition"<<"UsnOffset"<<"MFT"<<"PMFT"<<"UsnNumber"<<"FileChangeTime"<<"FileChangeReason"<<"FileAttribute"<<"FileName";

//usn监控日志头
static QStringList g_UsnMonitorTitle = QStringList()<<"盘符"<<"MFT"<<"PMFT"<<"文件变化原因"<<"文件属性"<<"文件名"<<"命令"<<"命令状态";


//定义mft号类型
typedef  quint32 MFTNUM;

#define NOEXISTING_FILE_FLAG (0x0) //不存在文件的标识
#define EXISTING_FILE_FLAG (0x1) //存在文件的标识
#define NOEXISTING_DIRECTORY_FLAG (0x2) //不存在目录的标识
#define EXISTING_DIRECTORY_FLAG (0x3) //存在目录的标识

#define ROOT_DIRECTORY_MFT_NUMBER (0x5)  //根目录mft号


#define NEW_WINDOW ((QEvent::Type)(QEvent::User+1))//自定义全局事件；新建窗口
#define CLOSE_WINDOW ((QEvent::Type)(QEvent::User+2))//自定义全局事件；关闭窗口
#define EXIT_PROGRAM ((QEvent::Type)(QEvent::User+3))//自定义全局事件；结束程序
#define CREATE_CONSOLE ((QEvent::Type)(QEvent::User+4))//自定义全局事件；创建控制台
#define CLOSE_CONSOLE ((QEvent::Type)(QEvent::User+5))//自定义全局事件；关闭控制台
#define SHOW_WINDOWS ((QEvent::Type)(QEvent::User+6))//自定义全局事件；显示全部窗口
#define ABOUT ((QEvent::Type)(QEvent::User+7))//自定义全局事件；显示关于




#define MAX_USN_REOCRD_SIZE (0x228) //0x228最大usnjournal日志大小
#define MAXIMUM_VOLUME_LABEL_LENGTH (32*sizeof(wchar_t))
#define PAGE_USN_RECORD_SIZE (0x1000) //usnjournal一个簇的大小  保存日志一个簇最大保存
#define DAFAULT_SORT_COLUMN (0)       //程序开始默认排序第n列

#define BYTES_PER_SECTION (0x200) //每扇区的字节数
#define BYTES_PER_CLUSTER (8*BYTES_PER_SECTION) //每簇的字节数
#define BYTES_PER_MFT_FILE (2*BYTES_PER_SECTION) //mft 文件占用大小

#define ENDCOUNT (0x1000)     //数据更新超过当前值就更新
#define SPLIT_SYMBOLS "####"    //分割符  运行日志以当前字符分割保存
#define DEFAULT_ALL ""          //默认空字符
#define LOG_PATH        QCoreApplication::applicationDirPath()+QDir::separator()+QString("LOGS")+QDir::separator() //日志路径

#define EXPORTFILE_LIST QCoreApplication::applicationDirPath()+QDir::separator()+QString("FileList")+QDir::separator() //文件列表路径
#define USN_RECORD_PATH  LOG_PATH+QString("UsnRecord")+QDir::separator() //usn日志路径
#define RUN_LOG_PATH LOG_PATH+QString("RunLog")+QDir::separator()       //运行日志路径

#define CONFIGFILE_PATH QCoreApplication::applicationDirPath()+QDir::separator()+QString("config.ini") //配置文件路径

static QString g_OperateFileName  = QString("Operate")+SPLIT_SYMBOLS; //定义全局的操作文件名
/*
 *使用#pragma pack(push,1) 来表示内存对齐为1字节
 *#pragma pack(pop) 来表示恢复内存原本结构对齐n字节
 */

;/*分号不能去掉(解决报警---IncludeFile.h:15:9: warning: unterminated '#pragma pack (push, ...)' at end of file)*/
#pragma pack(push,1)
//保存UUID序列号
//typedef struct _UUID{
//    quint32 _32 ;
//    quint16 _16w1;
//    quint16 _16w2;
//    quint8 temp[8];
//} ;
/********************************************************************************/
/*
 * EFI信息区结构,也称为GPT头
 */

typedef struct _STRUCT_EFI_GPT_HEADER
{
    /*+0x00*/   quint64 GPTHeader;                  //GPT头签名“45 46 49 20 50 41 52 54”(ASCII码为“EFI PART”)
    /*+0x08*/   quint32 Version;                    //版本号，目前是1.0版，其值是“00 00 01 00”
    /*+0x0C*/   quint32 GPTSize;                    //GPT头的大小(字节数)，通常为“5C 00 00 00”(0x5C)，也就是92字节
    /*+0x10*/   quint32 GPT_CRC;                    //GPT头CRC校验和(计算时把这个字段本身看做零值)
    /*+0x14*/   quint32 Reserved;                   //保留，必须为“00 00 00 00”
    /*+0x18*/   quint64 GPTHeaderStartSection;      //EFI信息区(GPT头)的起始扇区号，通常为“01 00 00 00 00 00 00 00”，也就是LBA1。
    /*+0x20*/   quint64 GPTHeaderEndSection;        //EFI信息区(GPT头)备份位置的扇区号，也就是EFI区域结束扇区号。通常是整个磁盘最末一个扇区。
    /*+0x28*/   quint64 GPTStartSection;            //GPT分区区域的起始扇区号，通常为“22 00 00 00 00 00 00 00”(0x22)，也即是LBA34。
    /*+0x30*/   quint64 GPTEndSection;              //GPT分区区域的结束扇区号，通常是倒数第34扇区。
    /*+0x38*/   GUID  GUID;                   //磁盘GUID(全球唯一标识符,与UUID是同义词)
    /*+0x48*/   quint64 PartitionTableStartSection; //分区表起始扇区号，通常为“02 00 00 00 00 00 00 00”(0x02)，也就是LBA2。
    /*+0x50*/   quint32 PartitionTablesAllNum;      //分区表总项数，通常限定为“80 00 00 00”(0x80)，也就是128个。
    /*+0x54*/   quint32 PartitionTableByteNum;      //每个分区表项占用字节数，通常限定为“80 00 00 00”(0x80)，也就是128字节。
    /*+0x58*/   quint32 PartitionTableCRC;          //分区表CRC校验和
    /*+0x5C*/   quint8  Reserved_1[420];            //保留，通常是全零填充


}S_EFI_GPT_HEADER,*PS_EFI_GPT_HEADER;

/*
 * GPT分区项结构
 */
typedef struct _STRUCT_PARTITION_ITEM
{
    /*+0x00*/   GUID  GUIDType; //用GUID表示的分区类型
    /*+0x10*/   GUID  GUID;     //用GUID表示的分区唯一标示符
    /*+0x20*/   quint64 StartSection;   //分区的起始扇区，用LBA值表示。
    /*+0x28*/   quint64 EndSection;     //该分区的结束扇区(包含)，用LBA值表示，通常是奇数。
    /*+0x30*/   quint64 Flag;           //该分区的属性标志
    /*+0x38*/   quint8  PartitionName[72];  //UTF-16LE编码的人类可读的分区名称，最大32个字符
}S_PARTITION_ITEM,*PS_PARTITION_ITEM;






/********************************************************************************/
/*
 *  NTFS 的DBR 数据结构
 */

typedef struct _STRUCT_BIOS_PARAMETER_BLOCK
{

    /*+0x0B*/    quint16  BytesPerSector;    // 字节/扇区一般为0x0200 即512
    /*+0x0D*/    quint8   SectorsPerCluster; // 扇区/簇
    /*+0x0E*/    quint16  ReservedSectors;   // 保留扇区
    /*+0x10*/    quint8   RootEntries[3];       //
    /*+0x13*/    quint16  Sectors;           //
    /*+0x15*/    quint8   Media;             // 媒介描述
    /*+0x16*/    quint16  SectorsPerFat;     //
    /*+0x18*/    quint16  SectorsPerTrack;   // 扇区/磁轨
    /*+0x1A*/    quint16  Heads;             // 头
    /*+0x1C*/    quint32  HiddenSectors;     // 隐藏扇区
    /*+0x20*/    quint32  LargeSectors;      // checked when volume is mounted

}S_BIOS_PARAMETER_BLOCK, *PS_BIOS_PARAMETER_BLOCK;

/*序列号*/
typedef  union _UNION_SERIAL_NUMBER
{
    /*+0x48*/    union _32{
        quint8 SerialNumber_32[4];
        quint32 SerialNumber_SN;
    } SerialNumberLow_32;
    /*+0x48*/    quint8 SerialNumber_64[8];
}U_SERIAL_NUMBER,*PU_SERIAL_NUMBER;
//保存boot扇区结构
typedef struct _STRUCT_NTFS_BOOT{
    /*+0x00*/  quint8    JmpCode[3];        // 跳转指令
    /*+0x03*/  quint8     OemID[8];          // 文件系统ID
    /*+0x0B*/  S_BIOS_PARAMETER_BLOCK PackedBpb;   // BPB
    /*+0x24*/  quint32    Unused;           // 未使用,总是为
    /*+0x28*/  quint64   NumberSectors;       // 扇区总数
    /*+0x30*/  quint64      MftStartLcn;        // 开始C# $MFT  (簇) 乘以 BIOS_PARAMETER_BLOCK.SectorsPerCluster 值得到扇区号
    /*+0x38*/  quint64      Mft2StartLcn;       // 开始C# $MFTMirr (簇)
    /*+0x40*/  quint8    ClustersPerFileRecordSegment;  // 文件记录大小指示器 2的负一次方× -10=1024
    /*+0x41*/  quint8   Reserved0[3];       // 未使用
    /*+0x44*/  quint8 DefaultClustersPerIndexAllocationBuffer;     // 簇/索引块
    /*+0x45*/  quint8   Reserved1[3];       // 未使用
    /*+0x48*/  U_SERIAL_NUMBER  serialNumber;       // 64位序列号(卷标)
    /*+0x50*/  quint32  Checksum;           // 校验和
    /*+0x54*/  quint8   BootStrap[426];     // 启动代码
    /*+0x1FE*/ quint16  RecordEndSign;      // 0xAA55 结束标记
}S_NTFS_BOOT,*PS_NTFS_BOOT;

/********************************************************************************/


/********************************************************************************/

// 文件记录头
typedef struct _STRUCT_FILE_RECORD_HEADER
{
    /*+0x00*/  quint32 Type;            // 固定值'FILE'***(需要用到)***
    /*+0x04*/  quint16 UsaOffset;       // 更新序列号偏移, 与操作系统有关
    /*+0x06*/  quint16 UsaCount;        // 固定列表大小Size in words of Update Sequence Number & Array (S)
    /*+0x08*/  quint64 Lsn;             // 日志文件序列号(LSN)
} S_FILE_RECORD_HEADER, *PS_FILE_RECORD_HEADER;

// 文件记录体
typedef struct _STRUCT_FILE_RECORD{
    /*+0x00*/  S_FILE_RECORD_HEADER Ntfs;  // MFT表头
    /*+0x10*/  quint16  SequenceNumber;   // 序列号(用于记录文件被反复使用的次数)***(需要用到)***
    /*+0x12*/  quint16  LinkCount;        // 硬连接数-------------------------------------------------表示当前mft文件中0x30属性的个数-----
    /*+0x14*/  quint16  AttributeOffset;  // 第一个属性偏移***(需要用到)***
    /*+0x16*/  quint16  Flags;            // falgs, 00表示删除文件,01表示正常文件,02表示删除目录,03表示正常目录***(需要用到)***
    /*+0x18*/  quint32  BytesInUse;       // 文件记录实时大小(字节) 当前MFT表项长度,到 FF FF FF FF 的长度+4
    /*+0x1C*/  quint32  BytesAllocated;   // 文件记录分配大小(字节)
    /*+0x20*/  quint64  BaseFileRecord:48;   // = 0 基础文件记录 File reference to the base FILE record 【非0的时候表示本身mft，当前mft是扩展】
    /*+0x28*/  quint16  NextAttributeNumber; // 下一个自由ID号
    /*+0x2A*/  quint16  Pading;           // 边界
    /*+0x2C*/  quint32  MFTRecordNumber;  // windows xp中使用,本MFT记录号***(需要用到)***
    /*+0x30*/  quint16  MFTUSN;      // 更新序号                (重要)
    /*+0x32*/  char     MFTUSA_0[2];      // 更新队列  填充第一个扇区  (重要)2*UsaCount-2
    /*+0x34*/  char     MFTUSA_1[2];    // 更新队列  填充第二个扇区  (重要)
    /*+0x36*/  quint16  Reserved;      // 保留 00 00
}S_FILE_RECORD, *PS_FILE_RECORD;


/********************************************************************************/
// 属性类型定义
typedef enum _ENUM_ATTRIBUTE_TYPE{
    AttributeStandardInformation = 0x10, //标准信息
    AttributeAttributeList = 0x20,       //属性列表
    AttributeFileName = 0x30,            //文件名
    AttributeVolumeVersion = 0x40,       //卷版本
    AttributeSecurityDescriptor = 0x50,  //安全描述符
    AttributeVolumeName = 0x60,          //卷名
    AttributeVolumeInformation = 0x70,   //卷信息
    AttributeData = 0x80,                //数据
    AttributeIndexRoot = 0x90,           //索引根
    AttributeIndexAllocation = 0xA0,     //索引分配
    AttributeBitmap = 0xB0,              //位图
    AttributeSymbolicLink = 0xC0,        //符号链接
    AttributeEAInformation = 0xD0,       //扩展信息
    AttributeEA = 0xE0,                  //扩展

    //AttributePropertySet = 0xF0,       //特权设置
    //AttributeLoggedUtilityStream = 0x100//日志流
} E_ATTRIBUTE_TYPE, *PE_ATTRIBUTE_TYPE;


//------------------  属性头通用结构 ----
typedef struct _STRUCT_NTFS_ATTRIBUTE_HEADER //所有偏移量均为相对于属性类型 Type 的偏移量
{
    /*+0x00*/    quint32 AttributeType;             // 属性类型 0x10, 0x20, 0x30, 0x40,...,0xF0,0x100***(需要用到)***
    /*+0x04*/    quint16 AttributeLength;           // 属性的长度***(需要用到)***属性长度两个字节够用，用4个字节可能会出现，AttributeLength很大的值
    /*+0x06*/    quint16 other;                     // 未知-》同上AttributeLength，分开   只使用AttributeLength
    /*+0x08*/    quint8 NonResidentFlag;             // 是否是非常驻属性，l 为非常驻属性，0 为常驻属性 00***(需要用到)***
    /*+0x09*/    quint8 AttributeNameLength;         // 属性名称长度，如果无属性名称，该值为 00      (N)
    /*+0x0A*/    quint16 ContentOffset;             // 属性名内容的偏移量  18 00
    /*+0x0C*/    quint16 CompressedFiag;            // 该文件记录表示的文件数据是否被压缩过 0x0001:压缩，0x4000:加密，0x8000:稀疏
    /*+0x0E*/    quint16 Identify;                  /* 识别标志  00 00属性标识 每个文件记录中的每一个属性都有一个属性标志。该标志在文件记录中是唯一的，其作用是保持数据的完整性。
                                                LinkCount 硬链接数大于1  0x30属性中的对比所有的最小值 表示是可用的文件名。元文件除外有些不同。
                                                等于 1 就一个  除外。*/
    //--- 0ffset: 0x10 ---
    //--------  常驻属性和非常驻属性的公共部分 ----
    union _CCommon
    {
        //---- 如果该属性为 常驻 属性时使用该结构 ----
        struct _CResident
        {
            /*+0x10*/       quint32 StreamLength;        // 属性值的长度, 即属性具体内容的长度。"48 00 00 00"***(需要用到)***
            /*+0x14*/       quint16 StreamOffset;        // 属性值起始偏移量  "18 00"***(需要用到)***
            /*+0x16*/       quint8 IndexFiag;           // 属性是否被索引项所索引，索引项是一个索引(如目录)的基本组成  00 00
            /*+0x17*/       quint8 Reserved;           // 00
            /*+0x18*/       wchar_t CAttName[1];         //注意：常驻有属性名的时候用的到/*......*/    2*N  //暂时用不到
        }CResident;
        //------- 如果该属性为 非常驻 属性时使用该结构 ----
        struct _CNonResident
        {
            /*+0x10*/       quint64 StartVCN;            // 起始的 VCN 值(虚拟簇号：在一个文件中的内部簇编号,0起）***(需要用到)***
            /*+0x18*/       quint64 LastVCN;             // 最后的 VCN 值***(需要用到)***
            /*+0x20*/       quint16 RunListOffset;       // 运行列表的偏移量(数据运行偏移量)***(需要用到)***
            /*+0x22*/       quint16 CompressEngineIndex; // 压缩引擎的索引值，指压缩时使用的具体引擎。
            /*+0x24*/       quint32 Reserved;              //保留
            /*+0x28*/       quint64 StreamAiiocSize;     // 为属性值分配的空间 ，单位为B，压缩文件分配值小于实际值
            /*+0x30*/       quint64 StreamRealSize;      // 属性值实际使用的空间，单位为B(字节)
            /*+0x38*/       quint64 StreamCompressedSize; // 属性值经过压缩后的大小, 如未压缩, 其值为实际值
            /*+0x40*/       wchar_t CNonAttName[1];         //注意：非常驻有属性名的时候用的到/*......*/    2*N  //暂时用不到
        }CNonResident;
    }CCommon;
}S_NTFS_ATTRIBUTE_HEADER, *PS_NTFS_ATTRIBUTE_HEADER;


/********************************************************************************/

//-------------属性类型------------

// 基础信息ATTRIBUTE.AttributeType == 0x10
typedef struct _STRUCT_STANDARD_INFORMATION_ATTRIBUTE_0X10
{
    /*+0x00*/   quint64 CreationTime;         // 文件建立时间 ---- 与0x30属性一样，无区别。
    /*+0x08*/   quint64 ChangeTime;           // 文件修改时间 ---- 最早更新比0x30属性更新早，获取文件的最后修改时间，以这个为准
    /*+0x10*/   quint64 LastWriteTime;        // MFT修改时间  ---- 该时间与该文件的usn最后时间同步
    /*+0x18*/   quint64 LastReadTime;         // 文件读时间  ---- 最早更新比0x30属性更新早，获取文件的最后读取时间，以这个为准
    /*+0x20*/   quint32 FileAttribute;        // 文件属性（DOS文件许可）标志	描述
    /*0x0001	只读
                                            0x0002	隐藏
                                            0x0004	系统
                                            0x0020	档案
                                            0x0040	驱动
                                            0x0080	正常
                                            0x0100	临时
                                            0x0200	解析文件
                                            0x0400	重解析点
                                            0x0800	压缩
                                            0x1000	脱机
                                            0x2000	无内容索引
                                            0x4000	加密*/

    /*+0x24*/   quint32 MaxVersionNum;        // 最大版本数
    /*+0x28*/   quint32 VersionsNum;          // 版本数
    /*+0x2C*/   quint32 ClassId;              // ClassID（不使用）
    /*+0x30*/   quint32 QuotaId;              // 所有者标识  所有者标识有权利使用这个文件的用户的标识。如果其值为零，那么，配额就不起作用了。
    /*+0x34*/   quint32 SecurityId;           // 安全标识
    /*+0x38*/   quint64 QuotaCharge;          // 配额限制
    /*+0x40*/   quint64 USN;                  // 更新序号

} S_ATTRIBUTE_0X10, *PS_ATTRIBUTE_0X10;


// 属性列表ATTRIBUTE.AttributeType == 0x20
typedef struct _STRUCT_ATTRIBUTE_LIST_0X20
{
    /*+0x00*/   quint32 AttributeType;             // 属性类型 0x10, 0x20, 0x30, 0x40,...,0xF0,0x100
    /*+0x04*/   quint16 Length;                   //记录项长度
    /*+0x06*/   quint8 NameLength;                //属性名长度  （N）
    /*+0x07*/   quint8 NameOffset;                //属性名偏移
    /*+0x08*/   quint64 StartVcn; // LowVcn       //VCN号
    union _Fdir
    {
        /*+0x10*/   quint64 FileReferenceNumber:48;     //文件MFT参考号 FileReferenceNumber & 0x0000ffffffffffff
        struct _Sn
        {
            /*+0x10*/   quint32 i;              //用来占位，不使用
            /*+0x14*/   quint16 j;              //用来占位，不使用
            /*+0x16*/   quint16 sn;             //提取后两位序号
        }Sn;
    }Fdir;
    /*+0x18*/   quint16 AttributeNumber;         //属性标识
    /*+0x1A*/   wchar_t AlignmentName[1];  //属性名  2*N（ifN>0)  //暂时用不到
} S_ATTRIBUTE_0X20, *PS_ATTRIBUTE_0X20;


// 文件属性ATTRIBUTE.AttributeType == 0x30
typedef struct _STRUCT_FILE_NAME_ATTRIBUTE_0X30
{
    union _Fdir
    {
        /*+0x00*/   quint64 DirectoryFile:48;    // 父目录记录号(前6个字节) 或者用-》 DirectoryFile & 0x0000ffffffffffff
        struct _Sn
        {
            /*+0x00*/   quint32 i;              //用来占位，不使用
            /*+0x04*/   quint16 j;              //用来占位，不使用
            /*+0x06*/   quint16 sn;             //提取后两位序号
        }Sn;
    }Fdir;
    /*+0x08*/  quint64 CreationTime;        // 文件创建时间
    /*+0x10*/  quint64 ChangeTime;          // 文件修改时间
    /*+0x18*/  quint64 LastWriteTime;       // MFT更新的时间
    /*+0x20*/  quint64 LastReadTime;       // 文件读时间
    /*+0x28*/  quint64 AllocatedSize;       // 文件分配大小
    /*+0x30*/  quint64 DataSize;            // 文件实际大小
    /*+0x38*/  quint32 FileAttributes;      // 标志,如目录\压缩\隐藏等
    /*+0x3C*/  quint32 AlignmentOrReserved; // 用于EAS和重解析
    /*+0x40*/  quint8 NameLength;           // 以字符计的文件名长度,没字节占用字节数由下一字节命名空间确定
    /*+0x41*/  quint8 NameType;             // 文件名命名空间, 0 POSIX大小写敏感,1 win32空间,2 DOS空间, 3 win32&DOS空间
    /*+0x42*/  wchar_t Name[1];               // 以Unicode方式标识的文件名
} S_ATTRIBUTE_0X30, *PS_ATTRIBUTE_0X30;

// 卷版本属性 ATTRIBUTE.AttributeType == 0x40
//40H 属性  $OBJECT_ID

//MTFS统一给所有 MFT 记录分配的一个标识 --- 对象ID，即结构体第一个16B，可能只有一个全局对象ID，后面的3个ID可能没有。
typedef struct _STRUCT_VOLUME_VERSION_ATTRIBUTE_0X40
{
    UCHAR GObjectId[16];            // 全局对象ID 给文件的唯一ID号
    UCHAR GOriginalVolumeId[16];    // 全局原始卷ID 永不改变
    UCHAR GOriginalObjectId[16];    // 全局原始对象ID 派给本MFT记录的第一个对象ID
    UCHAR GDomain[16];              // 全局域ID (未使用)
} S_ATTRIBUTE_0X40,*PS_ATTRIBUTE_0X40;

// 安全描述符属性 ATTRIBUTE.AttributeType == 0x50
typedef struct _STRUCT_SECURITY_DESCRIPTOR_ATTRIBUTE_0X50
{

} S_ATTRIBUTE_0X50,*PS_ATTRIBUTE_0X50;

// 卷名属性 ATTRIBUTE.AttributeType == 0x60
typedef struct _STRUCT_VOLUME_NAME_ATTRIBUTE_0X60
{


} S_ATTRIBUTE_0X60,*PS_ATTRIBUTE_0X60;

// 卷信息属性 ATTRIBUTE.AttributeType == 0x70
typedef struct _STRUCT_VOLUME_INFORMATION_ATTRIBUTE_0X70
{
    quint64 noUsed1;        // 00
    quint8 mainVersion;       // 主版本号 1--winNT, 3--Win2000/XP
    quint8 SecVersion;        // 次版本号 当主为3, 0--win2000, 1--WinXP/7
    quint16 flag;           // 标志
    quint32 noUsed2;        // 00

    /* flag:
    *  0x0001  坏区标志 下次重启时chkdsk/f进行修复
    *  0x0002  调整日志文件大小
    *  0x0004  更新装载
    *  0x0008  装载到 NT4
    *  0x0010  删除进行中的USN
    *  0x0020  修复对象 Ids
    *  0x8000  用 chkdsk 修正卷
    */

} S_ATTRIBUTE_0X70,*PS_ATTRIBUTE_0X70;

// 索引根属性 ATTRIBUTE.AttributeType == 0x90
typedef struct _STRUCT_INDEX_ROOT_ATTRIBUTE_0X90
{
    /*索引根部分*/
    /*+0x00*/   quint32 AttributeType;          //属性类型
    /*+0x04*/   quint32 SortRule;               //排序规则
    /*+0x08*/   quint32 IndexAllocSize;         //索引分配大小
    /*+0x0C*/   quint8  IndexRecordNum;         //每一个索引记录所占的簇数
    /*+0x0D*/   quint8  Nouse_1[3];             // 填充
    /*索引区部分*/
    /*+0x10*/   quint32 FirstIdxOffset;         // 第一个索引项偏移
    /*+0x14*/   quint32 IndexTotalSize;         // 索引项总大小（包括索引区部分）
    /*+0x18*/   quint32 IndexAllocSize_1;       //索引分配大小
    /*+0x1C*/   quint8  Flag;                     // 标志
    /*+0x1D*/   quint8  Nouse_2[3];             // 填充

} S_INDEX_ROOT_ATTRIBUTE_0X90,*PS_INDEX_ROOT_ATTRIBUTE_0X90;

// 一般小目录在90属性中有常驻目录项，目录项的结构与INDX缓存中的目录项一样
// 索引根属性 ATTRIBUTE.AttributeType == 0x90索引项
typedef struct _STRUCT_INDEX_ROOT_ITEM_ATTRIBUTE_0X90
{
    union _dir
    {
        /*+0x00*/   quint64  MFTDirectoryFile:48;    // // 文件的 MFT 记录号，前6B是MFT编号,用于定位此文件记录  MFTDirectoryFile & 0x0000ffffffffffff
        struct _Sn
        {
            /*+0x00*/   quint32 i;              //用来占位，不使用
            /*+0x04*/   quint16 j;              //用来占位，不使用
            /*+0x06*/   quint16 sn;             //提取后两位序号
        }Sn;
    }dir;
    /*+0x08*/    quint16 ItemLen;              // 索引项的分配大小(包括上面8个字节)
    /*+0x0A*/    quint16 IndexItemActualLen;   // 索引项的实际大小0x10到0x10+
    /*+0x0C*/    quint16 IndexIdentifier;      // 索引标志 R
    /*+0x0E*/    quint16 Nouse_3;              // 填充

    union _Fdir
    {
        /*+0x10*/   quint64 DirectoryFile:48;    // 父目录记录号(前6个字节) 或者用-》 DirectoryFile & 0x0000ffffffffffff
        struct _Sn
        {
            /*+0x10*/   quint32 i;              //用来占位，不使用
            /*+0x14*/   quint16 j;              //用来占位，不使用
            /*+0x16*/   quint16 sn;             //提取后两位序号
        }Sn;
    }Fdir;
    /*+0x18*/       quint64 CreateTime;            //文件创建时间
    /*+0x20*/       quint64 FileModifyTime;        //文件修改时间
    /*+0x28*/       quint64 MFTModifyTime;         //文件记录最后修改时间
    /*+0x30*/       quint64 LastVisitTime;         //文件最后访问时间
    /*+0x38*/       quint64 FileAllocSize;         //文件分配大小 (B)
    /*+0x40*/       quint64 FileRealSize;          //文件实际大小 (B)
    /*+0x48*/       quint64 FileIdentifier;        //文件标志
    /*+0x50*/       quint8  FileNameLen;           //文件名长度（F）
    /*+0x51*/       quint8  FileNameSpace;         //文件名命名空间
    /*+0x52*/       wchar_t    FileName[1];           //文件名  2*F (末尾填满 8（P） 字节)  //暂时用不到

    ///*+0x52+2F+P*/  quint64 NextItemVCN;           // 下一项的 VCN (有子节点才有)（通过L-8获取）




} S_INDEX_ROOT_ITEM_ATTRIBUTE_0X90,*PS_INDEX_ROOT_ITEM_ATTRIBUTE_0X90;

// 索引分配属性 ATTRIBUTE.AttributeType == 0xA0
typedef struct _STRUCT_INDEX_ALLOCATION_ATTRIBUTE_0XA0
{
    //...
} S_ATTRIBUTE_0XA0,*PS_ATTRIBUTE_0XA0;

// 位图属性 ATTRIBUTE.AttributeType == 0xB0
typedef struct _STRUCT_BITMAP_ATTRIBUTE_0XB0
{
    //...
} S_ATTRIBUTE_0XB0,*PS_ATTRIBUTE_0XB0;

// 符号链接属性 ATTRIBUTE.AttributeType == 0xC0
typedef struct _STRUCT_SYMBOLIC_LINK_ATTRIBUTE_0XC0
{
    //...
} S_ATTRIBUTE_0XC0,*PS_ATTRIBUTE_0XC0;

// ATTRIBUTE.AttributeType == 0xD0
typedef struct _STRUCTEA_INFORMATION_ATTRIBUTE_0XD0
{
    //...
} S_ATTRIBUTE_0XD0,*PS_ATTRIBUTE_0XD0;

// ATTRIBUTE.AttributeType == 0xE0
typedef struct _STRUCT_EA_ATTRIBUTE_0XE0
{
    //...
} S_ATTRIBUTE_0XE0,*PS_ATTRIBUTE_0XE0;
//索引记录
typedef struct _STRUCT_INDX_HEADER
{
    /*+0x00*/   quint32 majic;              //  标志 "INDX"
    /*+0x04*/   quint16 usnOffset;          // 更新序列偏移
    /*+0x06*/   quint16 usnSize;            // 更新序列数组大小(S)
    /*+0x08*/   quint64 LSN;                // 日志文件序列号
    /*+0x10*/   quint64 VCN;                // 本索引缓存在分配索引中的VCN
    /*+0x18*/   quint32 itemOffset;         // 第一项索引的偏移(从这里开始计算)
    /*+0x1C*/   quint32 itemSize;           // 索引项实际大小(B)
    /*+0x20*/   quint32 itemAlloc;          // 索引项分配大小(B)(不包括头部) 略小于4K
    /*+0x24*/   quint8  isNode;             // 是结点置1，表示有子节点
    /*+0x25*/   quint8  noUse[3];           //填充
    /*+0x28*/   quint16 USN;                //更新序号
    /*+0x2A*/   quint8  USA[1];             //更新队列  大小（2S-2）
    /*+0x00*/

} S_INDX_HEADER,*PS_INDX_HEADER;

//数据运行区域结构
typedef struct _STRUCT_DATARUN
{
    quint64 datalengthCluster = 0;//数据长度簇大小
    qint64 dataStartCluster = 0;//数据运行簇位置LCN
}S_DATARUN,*PS_DATARUN;


//------------------  usnJournal属性头 ----------------------------------------------------------------------------
typedef struct _STRUCT_USN_JOURNAL_ATTRIBUTE_HEADER //所有偏移量均为相对于属性类型 Type 的偏移量
{
    /*+0x00*/    quint32 AttributeType;             // 属性类型 0x10, 0x20, 0x30, 0x40,...,0xF0,0x100***(需要用到)***
    /*+0x04*/    quint32 AttributeLength;           // 属性的长度***(需要用到)***
    /*+0x08*/    quint8 NonResidentFlag;             // 是否是非常驻属性，l 为非常驻属性，0 为常驻属性 00***(需要用到)***
    /*+0x09*/    quint8 AttributeNameLength;         // 属性名称长度，如果无属性名称，该值为 00      (N)
    /*+0x0A*/    quint16 ContentOffset;             // 属性名内容的偏移量  18 00
    /*+0x0C*/    quint16 CompressedFiag;            // 该文件记录表示的文件数据是否被压缩过 0x0001:压缩，0x4000:加密，0x8000:稀疏
    /*+0x0E*/    quint16 Identify;                  // 识别标志  00 00
    //--- 0ffset: 0x10 ---
    //--------  常驻属性和非常驻属性的公共部分 ----
    union _CCommon
    {
        //---- 如果该属性为 常驻 属性时使用该结构 ----
        struct _CResident
        {
            /*+0x10*/       quint32 StreamLength;        // 属性值的长度, 即属性具体内容的长度。"48 00 00 00"***(需要用到)***
            /*+0x14*/       quint16 StreamOffset;        // 属性值起始偏移量  "18 00"***(需要用到)***
            /*+0x16*/       quint8 IndexFiag;           // 属性是否被索引项所索引，索引项是一个索引(如目录)的基本组成  00 00
            /*+0x17*/       quint8 Reserved;           // 00
            /*+0x18*/       wchar_t CAttName[8];         //注意：常驻有属性名的时候用的到/*......*/    2*N
        }CResident;
        //------- 如果该属性为 非常驻 属性时使用该结构 ----
        struct _CNonResident
        {
            /*+0x10*/       quint64 StartVCN;            // 起始的 VCN 值(虚拟簇号：在一个文件中的内部簇编号,0起）***(需要用到)***
            /*+0x18*/       quint64 LastVCN;             // 最后的 VCN 值***(需要用到)***
            /*+0x20*/       quint16 RunListOffset;       // 运行列表的偏移量(数据运行偏移量)***(需要用到)***
            /*+0x22*/       quint16 CompressEngineIndex; // 压缩引擎的索引值，指压缩时使用的具体引擎。
            /*+0x24*/       quint32 Reserved;              //保留
            /*+0x28*/       quint64 StreamAiiocSize;     // 为属性值分配的空间 ，单位为B，压缩文件分配值小于实际值
            /*+0x30*/       quint64 StreamRealSize;      // 属性值实际使用的空间，单位为B
            /*+0x38*/       quint64 StreamCompressedSize; // 属性值经过压缩后的大小, 如未压缩, 其值为实际值
            /*+0x40*/       quint64 DataSize;               //数据大小。。。（包含保存数据的总大小，这个值会跟随每次超过会改变，改变幅度不会太大）
            /*+0x48*/       wchar_t CNonAttName[4];         //注意：非常驻有属性名的时候用的到/*......*/    2*N
        }CNonResident;
    }CCommon;
}S_USN_JOURNAL_ATTRIBUTE_HEADER,*PS_USN_JOURNAL_ATTRIBUTE_HEADER;

#pragma pack(pop)




/********************************************************************************/
/*###########################自定义结构体保存信息##################################*/
/********************************************************************************/
typedef struct _STRUCT_GPT_INFO
{
    S_PARTITION_ITEM pDrivePartitionItem;  //物理盘分区项
    QString pDrive;                        //存放一个物理硬盘名称
} S_GPT_INFO, *PS_GPT_INFO;

typedef struct _STRUCT_NTFS_INFO
{
    quint16 BytesPerSector;              // 每扇区的字节数
    QUuid   uuid;                        //盘符唯一ID
    QString pDrive;                      //存放一个物理硬盘名称
    QString partitionFlagName;           //分区标识名 //C:/    D:/  ....
    quint64 bytesTotal;                  //盘符总大小
    quint64 bytesOffset;                 //开始读的位置

} S_NTFS_INFO, *PS_NTFS_INFO;

//---------------------------------------------------------------------------------------------------------------
//保存mft 0 x80属性的的dataRun
typedef struct _STRUCT_MFT0_0X80_DATARUN
{

    qint64 dataStartByte;//数据运行字节位置LCN
    quint64 datalengthByte;//数据长度字节大小

    quint32 lowNumber;//最低的mft号
    quint32 highNumber;//最高的mft号
    //总共个数：highNumber-lowNumber+1
    bool operator!=(const _STRUCT_MFT0_0X80_DATARUN & src)
    {
        if(dataStartByte != src.dataStartByte)
            return true;
        if(datalengthByte != src.datalengthByte)
            return true;
        if(lowNumber != src.lowNumber)
            return true;
        if(highNumber != src.highNumber)
            return true;
        return false;
    }

}S_MFT0_0X80_DATARUN,*PS_MFT0_0X80_DATARUN;


//属性体
typedef struct _STRCUT_ATTRIBUTE_BODY
{
    QList<S_ATTRIBUTE_0X20 > _x20Body;
    S_INDEX_ROOT_ATTRIBUTE_0X90 INDEX_ROOT;//只用到：0x90属性的索引根和索引区部分
    QList<QPair<S_INDEX_ROOT_ITEM_ATTRIBUTE_0X90,QString> > _x90Body;
    QVariant data; //RunData
    QString name;           //结尾有名子用的到 x20 和 x90 不用这个保存
}S_ATTRIBUTE_BODY;
//保存一个文件（FILE)的所有信息
typedef struct _STRUCT_MFT_INFO
{
    //bool    ISOK;//是否可用
    quint8  flag;// falgs, 00表示删除文件,01表示正常文件,02表示删除目录,03表示正常目录
    quint32 MFTNumber;//本mft号
    quint32 PMFTNumber;//父mft号
    S_FILE_RECORD FileRecord;//file头

    /*
     * 常驻：QVariant就是属性体 _x20是QList<ATTRIBUTE_LIST_0x20>，_x90是QList<INDEX_ROOT_ITEM_0x90>
     * 非常驻：QVariant就是QList<DATARUN>
     */
    QList<QPair<S_NTFS_ATTRIBUTE_HEADER,S_ATTRIBUTE_BODY>> _x10;
    QList<QPair<S_NTFS_ATTRIBUTE_HEADER,S_ATTRIBUTE_BODY>> _x20;
    QList<QPair<S_NTFS_ATTRIBUTE_HEADER,S_ATTRIBUTE_BODY>> _x30;
    QList<QPair<S_NTFS_ATTRIBUTE_HEADER,S_ATTRIBUTE_BODY>> _x40;
    QList<QPair<S_NTFS_ATTRIBUTE_HEADER,S_ATTRIBUTE_BODY>> _x50;
    QList<QPair<S_NTFS_ATTRIBUTE_HEADER,S_ATTRIBUTE_BODY>> _x60;
    QList<QPair<S_NTFS_ATTRIBUTE_HEADER,S_ATTRIBUTE_BODY>> _x70;
    QList<QPair<S_NTFS_ATTRIBUTE_HEADER,S_ATTRIBUTE_BODY>> _x80;
    QList<QPair<S_NTFS_ATTRIBUTE_HEADER,S_ATTRIBUTE_BODY>> _x90;
    QList<QPair<S_NTFS_ATTRIBUTE_HEADER,S_ATTRIBUTE_BODY>> _xA0;
    QList<QPair<S_NTFS_ATTRIBUTE_HEADER,S_ATTRIBUTE_BODY>> _xB0;

}S_MFT_INFO;




//usnJournal MFT
typedef struct _STRUCT_MFT_USN_JOURNAL
{
    bool    ISOK;//是否可用
    QList<S_DATARUN> lDRun;//保存JAtt  x80属性数据运行区
    CREATE_USN_JOURNAL_DATA MaxAtt;//$Max 数据 ------>暂未用到
    QList<S_USN_JOURNAL_ATTRIBUTE_HEADER> JAtt;//$J 属性头
}S_MFT_USN_JOURNAL;




/*
 * 0:主文件表本身            $MFT
 * 1:主文件表的部分镜像       $MFTMirr
 * 2:日志文件                $logFile
 * 3:卷文件                 $Volume
 * 4:属性定义列表            $AttrDef
 * 5:根目录                 $Root
 * 6:位图文件               $Bitmap
 * 7:引导文件               $Boot
 * 8:坏簇文件               $BadClus
 * 9:安全文件               $Secure
 * 10:大写文件              $UpCase
 * 11:包含各种可选扩展名的文件系统目录，
 * 例如 $Quota、$ObjId、
 * $Reparse 或 $UsnJrnl。        $Extend
 * 12 - 23:扩展元数据目录         $Extend_metadata_directory
 *
 * 24:配额管理文件           $Extend$Quota
 * 25:对象ID文件            $Extend$ObjId
 * 26:重解析点文件           $Extend$Reparse
 */
enum NTFS_MD{$MFT=0,$MFTMirr,$logFile,$Volume,$AttrDef,$Root,$Bitmap,$Boot,
             $BadClus,$Secure,$UpCase,$Extend$UsnJrnl};


// 写入日志的类型，根据不同的类型保存到不同的文件，0：运行日志；1：异常日志；2：操作日志；
typedef enum _ENUM_LOG_TYPE{
    RunLog=0,ErrorLog,OperationLog,UsnRecordLog,DataUpdateLog
} E_LOG_TYPE;
//// 写入数据库的类型，执行不同的操作：0：添加；1：更新；2：删除；
//typedef enum _DATABASE_TYPE{
//    InitDataBase,AddToDataBase,UpdataToDataBase_name_path,UpdataToDataBase_changeTime,UpdataToDataBase_fileSize,DelToDataBase
//} DATABASE_TYPE;
// 执行不同的操作：0：初始化数据显示界面；1：更新数据显示界面；2：重置后删除多余的数据；
typedef enum _ENUM_DATABASE_TYPE{
    InitDataBase =0,UpdataToDataBase,DelToDataBase
} E_DATABASE_TYPE;
// 状态栏更新 0：显示view数据总数；1：发送需要导出的数据总数给进度条；2：数据导出的过程中发送数据位置更新进度条;3:数据导出完成；4：显示文本
typedef enum _ENUM_STATUSBAR_UPDATA{
    Status_DataCount=0,Status_ExportDataStart,Status_ExportDataCount,Status_ExportDataEnd,Status_Text,Status_LoadFinish
} E_STATUSBAR_UPDATA;

//增加到末尾，插入，删，改，查，上移，下移，
typedef enum _ENUM_CHANGE{
    Alldata = 0,AppendData,InsertData,DeleteData,UpdateData,FindData,UpData,DownData
}E_CHANGE;

//保存单个mft的部分属性
typedef struct _STRUCT_DATA_BODY_ATT
{
    quint8 refCount=0;//指针指向当前属性的个数，等于0释放内存
    quint8  flag=0;                   // falgs, 00表示删除文件,01表示正常文件,02表示删除目录,03表示正常目录
    quint32 FileAttributes=0;         //文件属性
    quint64 creationTime=0;         // 文件建立时间
    quint64 changeTime=0;           // 文件最后一次修改时间
    quint64 lastReadTime=0;           // 文件最后一次读时间---更新访问根据usnjournal日志改变
    quint64 fileRealSize=0;          // 属性值实际使用的空间，单位为B(字节)
    quint64 fileAiiocSize=0;           // 为属性值分配的空间 ，单位为B，
}S_DATA_BODY_ATT,*PS_DATA_BODY_ATT;
//数据个体
typedef struct _STRUCT_DATA_BODY
{
    S_DATA_BODY_ATT bodyAtt;
    //bool    IsX20;                  //有x20属性，如果x20属性中有和本mft不相同的x30mft，记录FALSE，目的本数据起到过渡作用，以防其他的mft的父是当前的mft。 正常为TRUE
    quint8  partitionSerial;        //盘符序号 -- 识别唯一的序号
    MFTNUM MFTNumber;              //本mft号
    MFTNUM PMFTNumber;             //父mft号
    QByteArray name;                   //当前文件名
    // wchar_t *name;                   //当前文件名

}S_DATA_BODY,*PS_DATA_BODY;


//更新数据的基本信息
typedef struct _STRUCT_DATA_SUBSET
{
    E_DATABASE_TYPE dataType;//传输到界面的数据类型-》更新项
    GUID    guid;//盘符的唯一标识
    QList<S_DATA_BODY*> dataBodyL;
    MFTNUM mft;//备用 -- 当查找列表为空，用来检索数据
    //DATA_BODY *dataBody;
    quint32 reason;
    quint64 usnOffset;

}S_DATA_SUBSET,*PDATA_SUBSET;
//保存盘的数据个数
typedef struct _STRUCT_PARTITION_DATA_COUNT
{
    quint32 _falg_0_count = 0;//mft flag 0的个数
    quint32 _falg_1_count = 0;
    quint32 _falg_2_count = 0;
    quint32 _falg_3_count = 0;
    quint32 _falg_other_count = 0;


}S_PARTITION_DATA_COUNT,*PS_PARTITION_DATA_COUNT;
//初始化数据-》model
typedef struct _STRUCT_INIT_DATAS_MODEL
{
    S_NTFS_INFO info;
    QHash<quint32,S_DATA_BODY*> dataBodys;  //采集盘符中的所有数据（未删除的文件和删除的文件，和未删除的文件夹）-- 使用共享指针，其他线程方便操作
    S_PARTITION_DATA_COUNT dataBodyCount ;  //数据个数
    quint8  partitionSerial;        //盘符序号 -- 识别唯一的序号
    //void *pNtfsSysInfo;             //当前指针
}S_INIT_DATAS_MODEL,*PS_INIT_DATAS_MODEL;
//保存恢复信息
typedef struct _STRUCT_RECOVER_INFO
{
    quint8 flag;                            //falgs, 00表示删除文件,01表示正常文件,02表示删除目录,03表示正常目录
    MFTNUM mft;                             //要恢复用到mft
    QString pDrive;                         //存放一个物理硬盘名称
    quint64 ntfsOffset;                     //盘符在物理硬盘中的偏移位置
    QList<S_MFT0_0X80_DATARUN> mftTables;   //mft数据表
    QString saveFileName;                   //保存文件名

}S_RECOVER_INFO,*PS_RECOVER_INFO;

//更新界面数据
typedef struct _STRUCT_UPDATE_VIEW
{
    bool CaseInsensitive;//区分大小写
    bool Complete;//完全匹配
    bool RegExp;//使用正则
    QString ExtendName;//扩展名
    QString searchName;//搜索文本

    qint32 sortColumn;//排序列
    Qt::SortOrder sortOrder;//升序TRUE，降序false

}S_UPDATE_VIEW;
//搜索的范围
typedef struct _STRUCT_RANGE
{
    quint64 leftValue;   //0:表示空
    quint64 rightValue;  //-1:表示空 -1 == FFFFFFFFFFFFFFFF

}STRUCT_RANGE;

//更新界面数据--高级搜索
typedef struct _STRUCT_HIGHFIND_UPDATE_VIEW
{
    qint32 partitionSerial;                 //盘符         -1:表示空
    qint32 fileFlag;                        //文件类型      -1:表示空
    QString ExtendName;                     //扩展名
    qint64 mft;                             //mft号        -1:表示空
    qint64 pMft;                            //Pmft号       -1:表示空
    qint64 fileAtt;                         //文件属性      -1:表示空
    QString searchName;                     //搜索文本
    bool CaseInsensitive;                   //区分大小写
    bool Complete;                          //前置完全匹配
    bool RegExp;                            //使用正则
    STRUCT_RANGE fileRealRange;             //文件大小范围
    STRUCT_RANGE fileCreationTimeRange;     //文件创建时间范围
    STRUCT_RANGE fileChangeTimeRange;       //文件修改时间范围
    STRUCT_RANGE fileReadTimeRange;         //文件访问时间范围

}S_HIGHFIND_UPDATE_VIEW;
//usn 日志
typedef struct _STRUCT_USN_LOG
{
    //    QByteArray dataTime;                        //采集时间
    //    QByteArray partition;                       //盘符
    //    QByteArray usnOffset;                       //本记录偏移
    //    QByteArray mft;                             //mft号
    //    QByteArray pMft;                            //Pmft号
    //    QByteArray usn;                             //usn
    //    QByteArray reasonDataTime;                  //文件变化日期
    //    QByteArray reason;                          //文件变化原因
    //    QByteArray attributes;                      //文件属性
    //    QByteArray fileName;                        //文件名
    QDateTime dataTime;                        //采集时间
    QString partition;                       //盘符
    qint64 usnOffset;                       //本记录偏移
    qint64 mft = -1;                            //mft号
    qint64 pMft = -1;                            //Pmft号
    qint64 usn;                             //usn
    qint64 reasonDataTime;                  //文件变化日期
    qint64 reason = -1;                         //文件变化原因
    qint64 attributes = -1;                    //文件属性
    QByteArray fileName;                        //文件名


}S_USN_LOG;

//usn日志监控项
typedef struct _STRUCT_MONITOR_ITEM
{
    S_USN_LOG usnLogItem;//监控的记录项
    bool CaseInsensitive;                   //区分大小写
    bool Complete;                          //完全匹配
    bool RegExp;                            //使用正则
    bool flag;           //是否执行command
    QString command;     //Windows命令
}S_MONITOR_ITEM;
//setting软件设置参数
typedef struct _STRUCT_SETTING_ARGS
{
    BOOL autoStaringUp;         //开机自动启动
    BOOL showSystemTray;        //显示托盘
    BOOL real_timeSearch;       //实时搜索
    BOOL enableDeleteFileFlag;  //正常文件启用已删除文件--可实现深度复制
    BOOL showSearchButton;      //显示搜索按钮
    BOOL dataBaseLoad;          //数据库加载数据
    BOOL windowTop;          //窗口置顶
    BOOL usnRecordNoMonitoring;         //usn日志不监控
    QList<BOOL> columns;     //显示哪些列
    //QMap<QString,bool> showColumns;     //显示哪些列
}S_SETTING_ARGS;

////注册为元类型。QVariant 使用

Q_DECLARE_METATYPE(QList<S_DATARUN>);
Q_DECLARE_METATYPE(S_ATTRIBUTE_0X10);
Q_DECLARE_METATYPE(S_ATTRIBUTE_0X30);
Q_DECLARE_METATYPE(_STRUCT_USN_LOG);
Q_DECLARE_METATYPE(S_PARTITION_DATA_COUNT);

/********************************************************************************/
/*
 * 公共函数
 */

class GlobalFunction
{
public:
    explicit GlobalFunction();
    ~GlobalFunction();
    static GlobalFunction * getInstance();//创建单例

    //获取参数对应的的卷标信息
    QStorageInfo getVolumeInfo(QUuid uuid);

    //关闭或重启计算机   默认重启
    bool systemShutDown(int flag=1);
    //快速查找
    /*
     * //查找列表中不小于数据查找到返回当前位置的迭代器，否则返回大于迭代器的第一个位置。
     * // 前提是排序好的列表，算法采用的是折半查找。
     */
    //std
    template<class ForwardIterator, class T, class Compare>
    QPair<ForwardIterator,ForwardIterator> equal_range(ForwardIterator first, ForwardIterator last, const T& val, Compare comp);
    //qt
    template<class ForwardIterator, class T, class Compare>
    QPair<ForwardIterator,ForwardIterator> quickFind(ForwardIterator first, ForwardIterator last, const T& val, Compare comp);

    //    闰年的判定方法：
    //    ①、普通年能被4整除且不能被100整除的为闰年。（如2004年就是闰年，1900年不是闰年）
    //    ②、世纪年能被400整除的是闰年。（如2000年是闰年，1900年不是闰年）
    //    ③、对于数值很大的年份，这年如果能整除3200，并且能整除172800则是闰年。如172800年是闰年，86400年不是闰年（因为虽然能整除3200，但不能整除172800）
    //返回TRUE为闰年
    //    static bool ISLeapYear(quint32 year)
    //    {
    //        return (year % 400 == 0) ? true : ((year % 4 == 0 && year % 100 != 0) ? true : false);
    //    }
    /*
     * 1：FILETIME转日期
     * FILETIME:结构持有的64位无符号的文件的日期和时间值。此值表示自UTC（Coordinated Universal Time）时间1601年1月1日开始的100纳秒为单位的时间。
     * 协调世界时间(UTC)UTC是根据原子钟来计算时间，现在世界上最精确的原子钟50亿年才会误差1秒，可以说非常精确。
     * 平年全年有365天，闰年全年有366天，因为闰年的二月有29天，而平年的二月只有28天
     * 闰年共有366天（1-12月分别为31天，29天，31天，30天，31天，30天，31天，31天，30天，31天，30天，31天）。
     * 平年共有365天（1-12月分别为31天，28天，31天，30天，31天，30天，31天，31天，30天，31天，30天，31天）。
     */
    //最大支持9999年，超出取年数的后四位，比如10020年，取0020，即：20.
    QByteArray FILETIMEToDateTime(quint64 FILETIME);

    /* 2：时间戳转日期
    * 时间戳是通常是一个字符序列，唯一地标识某一刻的时间。数字时间戳技术是数字签名技术一种变种的应用。
    * 其值表示从UTC（Coordinated Universal Time）时间1970年1月1日00:00:00（称为UNIX系统的Epoch时间） 到当前时刻的秒数。
    * 时间戳具体指格林威治时间1970年01月01日00时00分00秒(北京时间1970年01月01日08时00分00秒)起至现在的总秒数。
    * 格林威治时间（GWT）
    * 格林威治子午线上的地方时，位于伦敦郊区的皇家格林尼治天文台的标准时间，也叫世界时。比如我们中国是东八区，北京时间是（GMT+08:00）
    */
    QByteArray TimestampToDateTime(quint32 Timestamp);//(传入值以秒为单位)

    //3:时间戳(utc) 转 fileTime(utc) (传入值以秒为单位)
    quint64 TimestampToFILETIME(quint32 Timestamp);
    QIcon fileIcon();//获取文件图标
    QIcon folderIcon();//返回目录图标
    QIcon driveIcon();//返回驱动图标
    void sort3Ways_wchar_asc(QList<S_DATA_BODY*>* a);//3区基数快速排序_升序
    void sort3Ways_wchar_des(QList<S_DATA_BODY*>* a);//3区基数快速排序_降序

    int search_BM(wchar_t* text,wchar_t* pat);//BM快速匹配

    QHash<QString, QIcon> fileIcons();//获取所有文件图标
    QIcon winFileIcon(QString filename);// 获取文件图标 --


    void chenSort(QList<S_DATA_BODY*> &list);



private:
    void sort3Ways_wchar_private_asc(QList<S_DATA_BODY*>* a,int lo,int hi,quint32 d);//3区基数快速排序_私人_升序
    void sort3Ways_wchar_private_des(QList<S_DATA_BODY*>* a,int lo,int hi,quint32 d);//3区基数快速排序_私人_降序

private:

    static GlobalFunction* m_instance;

};



//存放名和图标
typedef struct _LIST_ITEM
{
    QString name;
    QIcon icon;
    bool operator<(const _LIST_ITEM& src) const
    {
        return QString::compare(name, src.name, Qt::CaseInsensitive)<0 ? true : false;
    }
}LIST_ITEM;
class MyListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit MyListModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void extendList();//扩展名排序
    void numberList();//数字排序
    void defaultList();//默认排序
    void initStrData(QHash<QString,QIcon>*);//初始字符串数据
    void initNumData(QHash<QString,QIcon>*);//初始数字数据
    void initData(QHash<QString,QIcon>*);//初始数据
public slots:

private:

    QList<LIST_ITEM> m_listItem;
    QList<LIST_ITEM> m_tempListItem;
    const QHash<QString,QIcon>* m_iconList;
};

/* 存放各种数据 共享线程使用 -- 全局用到的参数
 */
class MemoryPool : public QObject
{
    Q_OBJECT
public:
    explicit MemoryPool(QObject *parent = nullptr);
    ~MemoryPool();

    QList<S_INIT_DATAS_MODEL>* getInfoList();
    QHash<QString,QIcon>* getIconList();
    QHash<QString,QIcon>* getPartitionList();
    QHash<QString,QIcon>* getFlagList();
    QHash<QString,QIcon>* getAttList();
    QList<WindowClass*>* getWindows();

    MyListModel* getIconModel();
    MyListModel* getPartitionModel();
    MyListModel* getFlagModel();
    MyListModel* getAttModel();

    QList<S_DATA_BODY*>* getModelData();

    quint32 getModelCreateCount();
    bool getIsFinish();
    bool getFinishThreadsFlag();   //获取线程是否结束标志
    const S_SETTING_ARGS getSettingArgs() ;           //获取设置的参数
    void loadSettingArgs() ;           //从ini文件加载设置的参数

    void setSettingArgs(S_SETTING_ARGS);       //改变设置的参数
    void setIsFinish(bool);
    void setFinishThreadsFlag(bool);   //设置线程是否结束标志



    static MemoryPool * getInstance();//创建单例
    void initModelList();//初始化mode列表
signals:
private:
    static MemoryPool* m_instance;//单例对象
    static QMutex m_mutex;//互斥锁

    QList<S_INIT_DATAS_MODEL>* m_infoList;    //所有盘符INFO（包括数据）
    QList<S_DATA_BODY*>* m_modelData;          //存放ui界面的数据，加速查找速度使用

    QHash<QString,QIcon>* m_iconList;          //存放扩展名的图标-》动态存放
    QHash<QString,QIcon>* m_partitionList;      //盘符列表
    QHash<QString,QIcon>* m_flagList;           //文件类型
    QHash<QString,QIcon>* m_attList;            //属性列表
    QList<WindowClass*>* windowList;//保存新建窗口//管理内存
    MyListModel* m_iconModel;
    MyListModel* m_partitionModel;
    MyListModel* m_flagModel;
    MyListModel* m_attModel;

    bool m_isFinish;//初始化完成标志
    QList<S_DATA_BODY*>* m_removeData;//需要删除的数据
    QMutex m_IconListMutex;//互斥锁
    QMutex m_InfoListMutex;//互斥锁
    QMutex m_ModelDataMutex;//互斥锁
    QMutex m_windowListMutex;//互斥锁

    QString m_UsnRecordFile;//记录usn日志文件名

    QMutex m_setting;
    S_SETTING_ARGS m_settingArgs;//保存设置的参数

    bool m_finishThreads;       //线程退出 -- 结束程序 true:结束  false:正常
    QMutex m_finishThreadsMutex;

};

#endif // INCLUDEFILE_H
