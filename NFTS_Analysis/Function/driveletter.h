#ifndef DRIVELETTER_H
#define DRIVELETTER_H

#include <QObject>
#include <QSettings>
#include <Windows.h>
#include <IncludeFile.h>
typedef  struct _DriveLetterValue
{
    quint64 flag;
    GUID    guid;
}DriveLetterValue;
class DriveLetter : public QObject
{
    Q_OBJECT
public:
    explicit DriveLetter(HKEY KEY,QString path,QObject *parent = nullptr);
    QMap<QString,QByteArray> getRegedit();//获取 注册表对应的键和值
    bool writeRegedit(QString key,DWORD type,BYTE* data);//根据key修改注册表，没有则新建
    BYTE*  readRegedit(QString key,DWORD type);//根据key和类型读取注册表
    bool delete_key(QString key);//删除键和对应的值；（文件夹也属于键）

    bool hideDriveLetter(QString drive);//隐藏驱动号（盘符）
    bool changeDriveLetter(QString newdrive,QString olddrive);//更改驱动号（盘符）
    QMap<QString,QByteArray> getNTFSDrive(QList<QUuid> allGuid);//获取所有ntfs盘符（包括隐藏后的）

signals:
protected:
    QString RetFullPath(HKEY KEY,QString path);//组合注册表完整路径
private:

    HKEY KEY;
    wchar_t path_w[1024];
    QString path;
    QString fullPath;
    QMap<QString,QByteArray> allValue;
    //quint32 indexNum;

};

#endif // DRIVELETTER_H
