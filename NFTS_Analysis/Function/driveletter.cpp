#include "driveletter.h"
using namespace std;
DriveLetter::DriveLetter(HKEY KEY,QString path,QObject *parent) : QObject(parent)
{


    this->KEY=KEY;
    this->path=path;
    this->fullPath = RetFullPath(KEY,path);



    wcscpy_s(reinterpret_cast<wchar_t*>(path_w),
        sizeof (path_w) / sizeof (wchar_t),
        reinterpret_cast< const  wchar_t*>(path.utf16()));

    allValue = getRegedit();

}
//组合注册表完整路径
QString DriveLetter::RetFullPath(HKEY KEY,QString path)
{
    QString temp;
    if(KEY == HKEY_CLASSES_ROOT)
    {
        temp = QString("HKEY_CLASSES_ROOT\\")+path;
    }
    else if(KEY == HKEY_CURRENT_USER)
    {
        temp = QString("HKEY_CURRENT_USER\\")+path;
    }
    else if(KEY == HKEY_LOCAL_MACHINE)
    {
        temp = QString("HKEY_LOCAL_MACHINE\\")+path;
    }
    else if(KEY == HKEY_USERS)
    {
        temp = QString("HKEY_USERS\\")+path;
    }
    else if(KEY == HKEY_PERFORMANCE_DATA)
    {
        temp = QString("HKEY_PERFORMANCE_DATA\\")+path;
    }
    else if(KEY == HKEY_PERFORMANCE_TEXT)
    {
        temp = QString("HKEY_PERFORMANCE_TEXT\\")+path;
    }
    else if(KEY == HKEY_PERFORMANCE_NLSTEXT)
    {
        temp = QString("HKEY_PERFORMANCE_NLSTEXT\\")+path;
    }
    #if (WINVER >= 0x0400)
    else if(KEY == HKEY_CURRENT_CONFIG)
    {
        temp = QString("HKEY_CURRENT_CONFIG\\")+path;
    }
    else if(KEY == HKEY_DYN_DATA)
    {
        temp = QString("HKEY_DYN_DATA\\")+path;
    }
    else if(KEY == HKEY_CURRENT_USER_LOCAL_SETTINGS)
    {
        temp = QString("HKEY_CURRENT_USER_LOCAL_SETTINGS\\")+path;
    }
    #endif
    return temp;
}
//获取键和值
QMap<QString,QByteArray> DriveLetter::getRegedit()
//index为索引号
{
    QSettings reg(fullPath,QSettings::NativeFormat);
    int indexNum=reg.allKeys().count();
    QMap<QString,QByteArray> map;



    //QString commresult;
    HKEY hKey;
    DWORD keysize,valueseize,type;
    QString keymessage;
    QString message;
    QString valuemessage;
    wchar_t keyname[256];
    char keyvalue[1024];
    keysize =sizeof (keyname);
    valueseize = sizeof(keyvalue);
    if(RegOpenKeyEx(KEY,path_w,0,KEY_READ,&hKey)!=0)
    {
        //QString error = "错误";
        return  QMap<QString,QByteArray>();
    }
    for(int i=0;i<indexNum;++i)
    {
        if(RegEnumValue(hKey,i,keyname,&keysize,0,&type,(BYTE*)keyvalue,&valueseize)==0)
        {
            QString key = QString::fromWCharArray(keyname);
            QByteArray value = QByteArray(keyvalue,valueseize);
            map.insert(key,value);
            //map[key] = value;
        }

    }

    RegCloseKey(hKey);
    return map;
}
//根据key修改注册表，没有则新建
bool DriveLetter::writeRegedit(QString key,DWORD type,BYTE* data)
{

    HKEY hkey;
    HKEY hTempKey;
    QString path_1 = path.section("\\",0,-2);
    //QString path_2 = path.section("\\",-1);

    std::wstring wstr = path_1.toStdWString();
    const wchar_t* data_set = wstr.c_str();  //适合做函数参数

    //LPCTSTR data_set= L"SYSTEM";
    if (ERROR_SUCCESS == ::RegOpenKeyEx(KEY, data_set, 0,  KEY_WRITE, &hkey))
    {
        QString path_2 = path.section("\\",-1);

        std::wstring wstr = path_2.toStdWString();
        const wchar_t* data_set = wstr.c_str();  //适合做函数参数
        if (ERROR_SUCCESS==::RegCreateKey(hkey,data_set,&hTempKey))
        {
            std::wstring wstr = key.toStdWString();
            const wchar_t* data_set = wstr.c_str();  //适合做函数参数

            if (ERROR_SUCCESS == ::RegSetValueEx(hTempKey, data_set, 0, type, (const BYTE*)data, strlen((char *)data)))
            {
                //::RegCloseKey(hkey);
               return true;
            }
        }

    }

    ::RegCloseKey(hkey);
    return false;
}


//根据key和类型读取注册表
BYTE*  DriveLetter::readRegedit(QString key,DWORD type)
{

    std::wstring wstr = key.toStdWString();
    const wchar_t* szBuf = wstr.c_str();  //适合做函数参数
    HKEY hKEY;//定义有关的键，在查询结束时关闭
	BYTE *dwValue = new BYTE[1024];//长整型数据，如果是字符串数据用char数组
    memset(dwValue,0,1024);
    QScopedArrayPointer<BYTE, QScopedPointerArrayDeleter<BYTE>> rawdata(dwValue);
	DWORD dwSize = 1024;
    //访问注册表，hKEY则保存此函数所打开的键的句柄
    if (ERROR_SUCCESS == ::RegOpenKeyEx(KEY, path_w, 0, KEY_READ, &hKEY))
    {

        
        if (::RegQueryValueEx(hKEY,szBuf, 0, &type, dwValue, &dwSize) == ERROR_SUCCESS)
        {
            ::RegCloseKey(hKEY);
            return rawdata.take();
        }


    }
    ::RegCloseKey(hKEY);
    return NULL;

}

//删除键和对应的值；（文件夹也属于键）
bool  DriveLetter::delete_key(QString key)
{
   //QSettings sett(fullPath,QSettings::NativeFormat);
   //sett.remove(key);
    std::wstring wstr = key.toStdWString();
    const wchar_t* szBuf = wstr.c_str();  //适合做函数参数
    HKEY hKEY;//定义有关的键，在查询结束时关闭
    //访问注册表，hKEY则保存此函数所打开的键的句柄
    if (ERROR_SUCCESS == ::RegOpenKeyEx(KEY, path_w, 0,  KEY_WRITE, &hKEY))
    {
        if (::RegDeleteValue(hKEY,szBuf)== ERROR_SUCCESS)
        {
            ::RegCloseKey(hKEY);
            return true;
        }
    }
    ::RegCloseKey(hKEY);
    return false;


}
//隐藏驱动号（盘符）
bool DriveLetter::hideDriveLetter(QString drive)
{
    QString key = QString("#")+ QUuid::createUuid().toString();
    return changeDriveLetter(key,drive);
}
//更改驱动号（盘符）
bool DriveLetter::changeDriveLetter(QString newdrive,QString olddrive)
{
    BYTE* value = readRegedit(olddrive,REG_BINARY);
	QScopedArrayPointer<BYTE, QScopedPointerArrayDeleter<BYTE>> rawdata(value);
    if(!rawdata.isNull())
     {
        if(writeRegedit(newdrive,REG_BINARY,value))
            delete_key(olddrive);
        return true;

     }
    return false;

}
//获取所有ntfs盘符（包括隐藏后的）
QMap<QString,QByteArray> DriveLetter::getNTFSDrive(QList<QUuid> allGuid)
{
    QMap<QString,QByteArray> exitsDL;
    QMapIterator<QString,QByteArray> i(allValue);
        while(i.hasNext())
        {
            i.next();

            DriveLetterValue dV;
            const char * iv = i.value().data();
            memcpy(&dV,iv,strlen(iv));

            if(allGuid.contains(QUuid(dV.guid)))
            {
                exitsDL.insert(i.key(),i.value());
            }
        }
        return exitsDL;

}
