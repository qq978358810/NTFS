#include "IncludeFile.h"
GlobalFunction* GlobalFunction::m_instance = new GlobalFunction;//单例对象
GlobalFunction::GlobalFunction()
{

}
GlobalFunction::~GlobalFunction()
{

}
GlobalFunction * GlobalFunction::getInstance()//创建单例
{
    return m_instance;
}

void GlobalFunction::chenSort(QList<S_DATA_BODY*> &list) {
    int length = list.size();
    if (length < 2) {
        return;
    }

    qint64 maxValue = list.at(0)->MFTNumber;
    qint64 minValue = list.at(0)->MFTNumber;
    for (S_DATA_BODY* element : list) {
        if (element->MFTNumber > maxValue) {
            maxValue = element->MFTNumber;
        }
        if (element->MFTNumber < minValue) {
            minValue = element->MFTNumber;
        }
    }

    /// All elements are the same and do not need to be sorted.
    if (maxValue == minValue) {
        return;
    }

    /// Limit the maximum size of the bucket to ensure the performance of long list
    /// sorting, which can be adjusted according to the actual situation.
    ///
    /// The essential difference between this and bucket sorting is that the size of
    /// the bucket is only related to the length of the list, not the range of element values.
    //int bucketSize = qMin(length, 50000);
    int bucketSize = qMax(length/10000, 1);
    int maxBucketIndex = bucketSize - 1;

    QList<QList<S_DATA_BODY*>> buckets;
    for(int i = 0;i<bucketSize;i++) {
        buckets.append(QList<S_DATA_BODY*>());
    }
    //buckets.reserve(bucketSize);
    int slot;

    /// Calculate the bucket in which the element is located based on the value of the element
    /// and the maximum and minimum values.


    double factor = maxBucketIndex * 1.0 / (maxValue - minValue);
    for (S_DATA_BODY* element : list) {
        slot = (int) ((element->MFTNumber - minValue) * factor);
        buckets[slot].append(element);
    }



    int index = 0;
    for (QList<S_DATA_BODY*> bucket : buckets) {
        if (!bucket.isEmpty()) {
            if (bucket.size() > 1) {
                //if (bucket.size() >= 10000) {
//                        chenSort(bucket);
//                    } else {
                    /// The sort method here represents the fastest comparison-type algorithm (Quick sort, Tim sort, etc.)
                        std::sort(bucket.begin(),bucket.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
                        {
                            return infoA->MFTNumber < infoB->MFTNumber;//从大到小
                        });
//                pdqsort(bucket.begin(),bucket.end(),[=]( S_DATA_BODY* infoA,  S_DATA_BODY* infoB)
//                {
//                    return infoA->MFTNumber < infoB->MFTNumber;//从大到小
//                });
              //  }
                for (S_DATA_BODY* element : bucket) {
                    list[index++] = element;
                }
            } else {
                list[index++] = bucket.at(0);
            }
        }
    }
}

//3区基数快速排序_私人_升序
void GlobalFunction::sort3Ways_wchar_private_asc(QList<S_DATA_BODY*>* a,int lo,int hi,quint32 d)
{

    /* UTF-8最大的一个特点，就是它是一种变长的编码方式。它可以使用1~4个字节表示一个符号，根据不同的符号而变化字节长度。
    * UTF-8的编码规则很简单，只有二条：
    * 1）对于单字节的符号，字节的第一位设为0，后面7位为这个符号的unicode码。因此对于英语字母，UTF-8编码和ASCII码是相同的。
    * 2）对于n字节的符号（n>1），第一个字节的前n位都设为1，第n+1位设为0，后面字节的前两位一律设为10。剩下的没有提及的二进制位，全部为这个符号的unicode码。
    * 下表总结了编码规则，字母x表示可用编码的位。
    * Unicode符号范围 | UTF-8编码方式
    * (十六进制)           |          （二进制）
    * --------------------+---------------------------------------------
    *  0000 0000-0000 007F | 0xxxxxxx
    *  0000 0080-0000 07FF | 110xxxxx 10xxxxxx
    *  0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
    *  0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    * 跟据上表，解读UTF-8编码非常简单。如果一个字节的第一位是0，则这个字节单独就是一个字符；如果第一位是1，则连续有多少个1，就表示当前字符占用多少个字节。
    * 注：按照utf-8的存储方式 QByteArray存储也是utf-8，这样比较只需要取出一个字节即可。
    */

    bool isUpper=false;
    bool isLower=false;
    if(hi<=lo)
        return;
    int lt = lo,gt = hi;
    quint8 v = a->at(lo)->name[d];
    if(v>64 && v<91)//如果值为大写
        isUpper = true;
    if(v>96 && v<123)//如果值为小写
        isLower = true;
    int i = lo+1;
    while(i<=gt){
        quint8 t = a->at(i)->name[d];
        if(isUpper && t>96 && t<123 )//如果v值为大写，t转为大写比较。不区分大小写
            t-=32;
        if(isLower && t>64 && t<91 )//如果v值为小写，t转为小写比较。不区分大小写
            t+=32;
        if(t<v)
            a->swap(lt++,i++);
        else if(t>v)
            a->swap(i,gt--);
        else
            i++;
    }
    sort3Ways_wchar_private_asc(a,lo,lt-1,d);
    if(v>0)
        sort3Ways_wchar_private_asc(a,lt,gt,d+1);
    sort3Ways_wchar_private_asc(a,gt+1,hi,d);
}
//3区基数快速排序
void GlobalFunction::sort3Ways_wchar_asc(QList<S_DATA_BODY*>* a)
{
    sort3Ways_wchar_private_asc(a,0,a->count()-1,0);
}
//3区基数快速排序_私人_降序
void GlobalFunction::sort3Ways_wchar_private_des(QList<S_DATA_BODY*>* a,int lo,int hi,quint32 d)
{
    bool isUpper=false;
    bool isLower=false;
    if(hi<=lo)
        return;
    int lt = lo,gt = hi;
    quint16 v = a->at(lo)->name[d];
    if(v>64 && v<91)//如果值为大写
        isUpper = true;
    if(v>96 && v<123)//如果值为小写
        isLower = true;
    int i = lo+1;
    while(i<=gt){
        quint16 t = a->at(i)->name[d];
        if(isUpper && t>96 && t<123 )//如果v值为大写，t转为大写比较。不区分大小写
            t-=32;
        if(isLower && t>64 && t<91 )//如果v值为小写，t转为小写比较。不区分大小写
            t+=32;
        if(t<v)
            a->swap(i,gt--);
        else if(t>v)
            a->swap(lt++,i++);
        else
            i++;
    }
    sort3Ways_wchar_private_des(a,lo,lt-1,d);
    if(v>0)
        sort3Ways_wchar_private_des(a,lt,gt,d+1);
    sort3Ways_wchar_private_des(a,gt+1,hi,d);
}
//3区基数快速排序
void GlobalFunction::sort3Ways_wchar_des(QList<S_DATA_BODY*>* a)
{
    sort3Ways_wchar_private_des(a,0,a->count()-1,0);
}
int GlobalFunction::search_BM(wchar_t* text,wchar_t* pat)//BM快速匹配
{
    quint64 N=wcslen(text);
    quint64 M=wcslen(pat);
    quint64 R = 65536,skip;
    int *right = new int[R];
    for(quint64 c=0;c<R;c++)
        right[c] = -1;
    for(quint64 j=0;j<M;j++)
        right[pat[j]] = j;
    for(quint64 i=0;i<=N-M;i+=skip){
        skip=0;
        for(int j=M-1;j>=0;j--){
            if(pat[j] != text[i+j]){
                skip = qMax(1,j);
                break;
            }
        }
        if(skip == 0)
            return i;
    }
    return N;
}

//获取参数对应的的卷标信息
QStorageInfo GlobalFunction::getVolumeInfo(QUuid uuid)
{
    QList<QStorageInfo> volumesInfoS = QStorageInfo::mountedVolumes();
    for(int i=0;i<volumesInfoS.count();++i){
        if(uuid == QUuid(QString(QString(volumesInfoS[i].device()).section('{',1,1)).section('}',0,0)))
            return volumesInfoS[i];
    }
    return QStorageInfo();
}

//关闭或重启计算机   默认重启
bool GlobalFunction::systemShutDown(int flag)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    //获取进程标志
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return false;
    //获取关机特权的LUID
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    //获取这个进程的关机特权
    AdjustTokenPrivileges(hToken, false, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    if (GetLastError() != ERROR_SUCCESS)
        return false;
    if(flag==1){
        // 强制重启计算机
        if ( !ExitWindowsEx(EWX_REBOOT| EWX_FORCE, 0))
            system("shutdown -r -t 0");//继续重启
        return true;
    }else{
        //强制关闭计算机
        if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0))
            return false;
        return true;
    }
}

//快速查找
/*
 * //查找列表中不小于数据查找到返回当前位置的迭代器，否则返回大于迭代器的第一个位置。
 * // 前提是排序好的列表，算法采用的是折半查找。
 */
//std
template<class ForwardIterator, class T, class Compare>
QPair<ForwardIterator,ForwardIterator> GlobalFunction::equal_range(ForwardIterator first, ForwardIterator last, const T& val, Compare comp)
{
    ForwardIterator it = std::lower_bound (first,last,val,comp);
    return qMakePair( it, std::upper_bound(it,last,val,comp) );
}
//qt
template<class ForwardIterator, class T, class Compare>
QPair<ForwardIterator,ForwardIterator> GlobalFunction::quickFind(ForwardIterator first, ForwardIterator last, const T& val, Compare comp)
{
    ForwardIterator it = qLowerBound (first,last,val,comp);
    return qMakePair ( it, qUpperBound(it,last,val,comp) );
}

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
QByteArray GlobalFunction::FILETIMEToDateTime(quint64 FILETIME)
{
    FILETIME += 8*36000000000;//北京时间是（UTC+08:00:00:00:00:00:00）
    quint32 _ns = 1; //纳秒--最小单位100ns
    quint32 _us = 10*_ns;//微秒
    quint32 _ms = 1000*_us;//毫秒
    quint64 _second = 1000*_ms;//秒
    quint64 _minute = 60*_second;//分
    quint64 _hour = 60*_minute;//时
    quint64 _day = 24*_hour;//日
    int leapMonth[12] = {31,29,31,30,31,30,31,31,30,31,30,31};//闰年月份
    int commonMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};//平年月份

    quint32 yearStart = 1601;//保存年份
    quint16 month=0;//保存月份
    quint16 day=0;//保存日数
    quint16 hour=0;//保存小时
    quint16 minute=0;//保存分钟
    quint16 second=0;//保存秒数
    quint16 ms=0;//保存毫秒数
    quint16 us=0;//保存微秒数
    quint16 ns=0;//保存纳秒数

    quint32 allDayCount = 0;
    bool LeapOrCommon ;//保存是平年FALSE还是闰年TRUE
    qint64 tempDayCount = FILETIME / _day; //获取日数
    hour = (FILETIME % _day)/_hour; //获取小时
    minute = ((FILETIME % _day) % _hour) / _minute; //获取分钟
    second = (((FILETIME % _day) % _hour) % _minute) /_second ; //获取秒
    ms = ((((FILETIME % _day) % _hour) % _minute) % _second) / _ms; //获取毫秒
    us = (((((FILETIME % _day) % _hour) % _minute) % _second) % _ms) / _us; //获取微秒
    ns = ((((((FILETIME % _day) % _hour) % _minute) % _second) % _ms) % _us) / _ns; //获取纳秒

    //获取年份
    for(;;){
        qint64 day = (yearStart % 400 == 0) ? 366 : ((yearStart % 4 == 0 && yearStart % 100 != 0) ? 366 : 365);//判断是否闰年 = 366，平年 = 355
        allDayCount += day;
        if(allDayCount > tempDayCount){
            tempDayCount = tempDayCount + day + 1 - allDayCount;//剩余的日数
            LeapOrCommon = (day == 366) ? true : false;
            break;
        }
        yearStart++;
    }
    //获取月份和日数
    for(int i=0;i<12;++i){
        if(LeapOrCommon)
            tempDayCount -= leapMonth[i];
        else
            tempDayCount -= commonMonth[i];
        if(tempDayCount<1){
            month = i+1;
            if(LeapOrCommon)
                day = tempDayCount + leapMonth[i];
            else
                day = tempDayCount + commonMonth[i];
            break;
        }
    }
    QByteArray array;
    //年
    array.append(QByteArray::number(yearStart));
    array.append('-');
    //月
    array.append(QByteArray::number(month));
    array.append('-');
    //日
    array.append(QByteArray::number(day));
    array.append(' ');
    //时
    array.append(QByteArray::number(hour));
    array.append(':');
    //分
    array.append(QByteArray::number(minute));
    array.append(':');
    //秒
    array.append(QByteArray::number(second));
    array.append('.');
    //毫秒
    array.append(QByteArray::number(ms));
    array.append('.');
    //微秒
    array.append(QByteArray::number(us));
    array.append('.');
    //纳秒
    array.append(QByteArray::number(ns));
    return array;

}

/* 2：时间戳转日期
* 时间戳是通常是一个字符序列，唯一地标识某一刻的时间。数字时间戳技术是数字签名技术一种变种的应用。
* 时间戳具体指格林威治时间1970年01月01日00时00分00秒(北京时间1970年01月01日08时00分00秒)起至现在的总秒数。
* 格林威治时间（GWT）
* 格林威治子午线上的地方时，位于伦敦郊区的皇家格林尼治天文台的标准时间，也叫世界时。比如我们中国是东八区，北京时间是（GMT+08:00）
*/
QByteArray GlobalFunction::TimestampToDateTime(quint32 Timestamp)
{
    Timestamp += 8*3600;//北京时间是（GMT+08:00）

    quint32 _second = 1;//秒
    quint32 _minute = 60*_second;//分
    quint32 _hour = 60*_minute;//时
    quint32 _day = 24*_hour;//日
    int leapMonth[12] = {31,29,31,30,31,30,31,31,30,31,30,31};//闰年月份
    int commonMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};//平年月份

    quint32 yearStart = 1970;//保存年份
    quint16 month=0;//保存月份
    quint16 day=0;//保存日数
    quint16 hour=0;//保存小时
    quint16 minute=0;//保存分钟
    quint16 second=0;//保存秒数

    quint32 allDayCount = 0;
    bool LeapOrCommon ;//保存是平年FALSE还是闰年TRUE
    qint64 tempDayCount = Timestamp / _day; //获取日数
    hour = (Timestamp % _day)/_hour; //获取小时
    minute = ((Timestamp % _day) % _hour) / _minute; //获取分钟
    second = (((Timestamp % _day) % _hour) % _minute) /_second ; //获取秒


    //获取年份
    for(;;){
        qint64 day = (yearStart % 400 == 0) ? 366 : ((yearStart % 4 == 0 && yearStart % 100 != 0) ? 366 : 365);//判断是否闰年 = 366，平年 = 355
        allDayCount += day;
        if(allDayCount > tempDayCount){
            tempDayCount = tempDayCount + day + 1 - allDayCount;//剩余的日数
            LeapOrCommon = (day == 366) ? true : false;
            break;
        }
        yearStart++;
    }
    //获取月份和日数
    for(int i=0;i<12;++i){
        if(LeapOrCommon)
            tempDayCount -= leapMonth[i];
        else
            tempDayCount -= commonMonth[i];
        if(tempDayCount<1){
            month = i+1;
            if(LeapOrCommon)
                day = tempDayCount + leapMonth[i];
            else
                day = tempDayCount + commonMonth[i];
            break;
        }
    }

    QByteArray array;
    //年
    array.append(QByteArray::number(yearStart));
    array.append('-');
    //月
    array.append(QByteArray::number(month));
    array.append('-');
    //日
    array.append(QByteArray::number(day));
    array.append(' ');
    //时
    array.append(QByteArray::number(hour));
    array.append(':');
    //分
    array.append(QByteArray::number(minute));
    array.append(':');
    //秒
    array.append(QByteArray::number(second));
    return array;

}
//3:时间戳(utc) 转 fileTime(utc) (传入值以秒为单位)
quint64 GlobalFunction::TimestampToFILETIME(quint32 Timestamp)
{
    //公元1601年到公元1970年相差369年，转化为秒数即为11644473600秒
    return (Timestamp+11644473600)*10000000;

}
// 根据扩展名->获取文件图标
QIcon GlobalFunction::fileIcon()
{
    QFileIconProvider provider;
    return provider.icon(QFileIconProvider::File);
}

// 获取文件图标 --
QIcon GlobalFunction::winFileIcon(QString filename)
{

    SHFILEINFO *info = new SHFILEINFO;
#ifdef UNICODE
    const wchar_t* name = reinterpret_cast<const wchar_t *>(filename.utf16());//qstring 转 wchar_t*
#else
    std::string str = filename.toStdString();
    LPCSTR name = str.c_str();

#endif
    //HRESULT comType = OleInitialize(NULL);//必须初始化com组件，不然会造成ui异常
    static bool comOk = true;
    if(comOk){
        CoInitialize(NULL);//必须初始化com组件，不然会造成ui异常
        comOk = false;
    }

    if (SHGetFileInfo(name,FILE_ATTRIBUTE_NORMAL,info,sizeof(SHFILEINFO),SHGFI_SYSICONINDEX | SHGFI_ICON )){
        if(info->hIcon){
            QPixmap pixmap = QtWin::fromHICON(info->hIcon);
            DestroyIcon(info->hIcon);//销毁图标
            return QIcon(pixmap);
        }
    }
    delete info;
    return QIcon();
}
// 从注册表获取所有文件图标
QHash<QString, QIcon> GlobalFunction::fileIcons()
{
    QHash<QString, QIcon> hash;
    QString path = "HKEY_CLASSES_ROOT\\";
    QSettings settings(path,QSettings::NativeFormat);
    QStringList keys= settings.allKeys();//获取所有键
    QStringList keysTemp;
    for(int i=0;i<keys.size();i++){
        //筛选带.的键
        QString str =  keys.at(i).split("/").at(0);
        if(str.at(0) == ".")
            keysTemp.append(str);
    }
    keysTemp = keysTemp.toSet().toList();//去重
    //遍历扩展名，获取图标
    for(int i=0;i<keysTemp.size();i++){
        QSettings sett(path+keysTemp.at(i)+"\\",QSettings::NativeFormat);
        QString str = sett.value(".").toString();//获取扩展名的键的默认值 //默认值：保存带有图标的键
        if(!str.isEmpty()){
            QSettings sett(path+str+"\\",QSettings::NativeFormat);
            //找出带有图标的符合的键
            if(sett.contains("DefaultIcon/.")){
                QString str1 = sett.value("DefaultIcon/.").toString();
                //默认图标键的值非空
                if(!str1.isEmpty()) {
                    QStringList strL = str1.split(",");//以,分割： 1：包含图标的字符串（可执行文件、DLL 或图标文件的名称） 和 2：索引
                    //只有一个字符串，索引默认是0
                    if(strL.size()==1){
                        if(!strL.at(0).isEmpty()){
                            if(strL.at(0) == "%1")//为 %1  时 按照文件绝对路径获取图标
                                hash.insert(keysTemp.at(i).toLower(),QIcon());
                            else{
                                strL[0].replace('"',"");
                                HICON bICON ;
                                HICON sICON ;
                                uint jj = ExtractIconEx(reinterpret_cast<const wchar_t *>(strL.at(0).utf16()),0,&bICON,&sICON,1);//从文件名中获取图标
                                //获取失败为：UINT_MAX
                                if(jj != UINT_MAX){
                                    if(sICON){
                                        //图标非NULL 直接添加
                                        QPixmap pixmap = QtWin::fromHICON(sICON);
                                        hash.insert(keysTemp.at(i).toLower(),QIcon(pixmap));
                                    }else{
                                        //重新获取  -  从文件信息中直接获取文件图标
                                        QIcon icon = winFileIcon(strL.at(0));
                                        if(!icon.isNull())
                                            hash.insert(keysTemp.at(i).toLower(),icon);
                                    }
                                }
                            }
                        }
                    }else if(strL.size()==2){
                        //带有索引的字符串，从字符串中的索引位置读取图标
                        if(!strL.at(0).isEmpty()){
                            strL[0].replace('"',"");
                            bool isOk;
                            int index = strL.at(1).toInt(&isOk);
                            if(isOk){
                                HICON bICON ;
                                HICON sICON ;

                                uint jj = ExtractIconEx(reinterpret_cast<const wchar_t *>(strL.at(0).utf16()),index,&bICON,&sICON,1);
                                if(jj != UINT_MAX){
                                    if(sICON){
                                        QPixmap pixmap = QtWin::fromHICON(sICON);
                                        hash.insert(keysTemp.at(i).toLower(),QIcon(pixmap));
                                    }else{
                                        QIcon icon = winFileIcon(strL.at(0));
                                        if(!icon.isNull())
                                            hash.insert(keysTemp.at(i).toLower(),icon);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return hash;
}

QIcon GlobalFunction::folderIcon()//返回目录图标
{
    QFileIconProvider provider;
    return provider.icon(QFileIconProvider::Folder);
}
QIcon GlobalFunction::driveIcon()//返回驱动图标
{
    QFileIconProvider provider;
    return provider.icon(QFileIconProvider::Drive);
}


//-------------------------------------------------------------------------------------------------------
//                                 MemoryPool类
//-------------------------------------------------------------------------------------------------------
MemoryPool* MemoryPool::m_instance = new MemoryPool;//单例对象
MemoryPool::MemoryPool(QObject *parent) : QObject(parent)
{
    m_infoList = new QList<S_INIT_DATAS_MODEL>;    //所有盘符INFO（包括数据）
    m_iconList = new QHash<QString,QIcon>;          //存放扩展名的图标-》动态存放
    m_partitionList = new QHash<QString,QIcon>;;      //盘符列表
    m_flagList = new QHash<QString,QIcon>;;           //文件类型列表
    m_attList = new QHash<QString,QIcon>;;            //属性列表
    m_iconModel = new MyListModel;
    m_partitionModel = new MyListModel;
    m_flagModel = new MyListModel;
    m_attModel = new MyListModel;
    windowList = new QList<WindowClass*>;           //

    m_modelData = new QList<S_DATA_BODY*>;          //存放ui界面的数据，加速查找速度使用
    m_removeData = new QList<S_DATA_BODY*>;//需要删除的数据
    m_isFinish = false;
    m_finishThreads = false;       //线程退出 -- 结束程序
    //初始化设置信息
    m_settingArgs.dataBaseLoad = false;
    m_settingArgs.autoStaringUp = false;
    m_settingArgs.showSystemTray = false;
    m_settingArgs.real_timeSearch = false;
    m_settingArgs.enableDeleteFileFlag = false;
    m_settingArgs.windowTop = false;
    m_settingArgs.usnRecordNoMonitoring = false;
    for(int i=0;i<g_ModelHeadL.count();i++)
        m_settingArgs.columns.append(false);

}
MemoryPool::~MemoryPool()
{
}
MemoryPool * MemoryPool::getInstance()//创建单例
{
    return m_instance;
}

//1获取modelList数据
QHash<QString,QIcon>* MemoryPool::getIconList()
{
    return m_iconList;
}
QHash<QString,QIcon>* MemoryPool::getPartitionList()
{
    return m_partitionList;
}
QHash<QString,QIcon>* MemoryPool::getFlagList()
{
    return m_flagList;
}
QHash<QString,QIcon>* MemoryPool::getAttList()
{
    return m_attList;
}

//2获取model
MyListModel* MemoryPool::getIconModel()
{
    return m_iconModel;
}
MyListModel* MemoryPool::getPartitionModel()
{
    return m_partitionModel;
}
MyListModel* MemoryPool::getFlagModel()
{
    return m_flagModel;
}
MyListModel* MemoryPool::getAttModel()
{
    return m_attModel;
}

//...
QList<S_INIT_DATAS_MODEL>* MemoryPool::getInfoList()
{
    QMutexLocker _lock(&m_InfoListMutex);
    return m_infoList;
}
QList<S_DATA_BODY*>* MemoryPool::getModelData()
{
    QMutexLocker _lock(&m_ModelDataMutex);
    return m_modelData;
}

void MemoryPool::setIsFinish(bool isok)
{
    m_isFinish = isok;
}
bool MemoryPool::getIsFinish()
{
    return m_isFinish;
}
//设置线程是否结束标志
void MemoryPool::setFinishThreadsFlag(bool flag)
{
    QMutexLocker _lock(&m_finishThreadsMutex);
    m_finishThreads = flag;
}

//获取线程是否结束标志
bool MemoryPool::getFinishThreadsFlag()
{
    QMutexLocker _lock(&m_finishThreadsMutex);
    return m_finishThreads;
}
//获取设置的参数
const S_SETTING_ARGS MemoryPool::getSettingArgs()
{
    QMutexLocker _lock(&m_setting);
    return m_settingArgs;
}
//改变设置的参数
void MemoryPool::setSettingArgs(S_SETTING_ARGS args)
{
    //m_setting.lock();
    m_settingArgs = args;
    //m_setting.unlock();
    QSettings settings(CONFIGFILE_PATH,QSettings::IniFormat);
    settings.setIniCodec("utf-8");// 解决乱码
    settings.beginGroup("Setting");
    settings.setValue("autoStaringUp",m_settingArgs.autoStaringUp);
    settings.setValue("showSystemTray",m_settingArgs.showSystemTray);
    settings.setValue("real_timeSearch",m_settingArgs.real_timeSearch);
    settings.setValue("enableDeleteFileFlag",m_settingArgs.enableDeleteFileFlag);
    settings.setValue("showSearchButton",m_settingArgs.showSearchButton);
    settings.setValue("dataBaseLoad",m_settingArgs.dataBaseLoad);
    settings.setValue("windowTop",m_settingArgs.windowTop);
    settings.setValue("usnRecordNoMonitoring",m_settingArgs.usnRecordNoMonitoring);


    for(int i= 0;i<m_settingArgs.columns.count();i++)
        settings.setValue(QString("Column")+QString::number(i),m_settingArgs.columns.at(i));
    settings.endGroup();
}
//从ini文件加载设置的参数
void MemoryPool::loadSettingArgs()
{
    QSettings settings(CONFIGFILE_PATH,QSettings::IniFormat);
    settings.setIniCodec("utf-8");// 解决乱码
    settings.beginGroup("Setting");
    m_settingArgs.autoStaringUp = settings.value("autoStaringUp",m_settingArgs.autoStaringUp).toBool();
    m_settingArgs.showSystemTray = settings.value("showSystemTray",m_settingArgs.showSystemTray).toBool();
    m_settingArgs.real_timeSearch = settings.value("real_timeSearch",m_settingArgs.real_timeSearch).toBool();
    m_settingArgs.enableDeleteFileFlag = settings.value("enableDeleteFileFlag",m_settingArgs.enableDeleteFileFlag).toBool();
    m_settingArgs.showSearchButton = settings.value("showSearchButton",m_settingArgs.showSearchButton).toBool();
    m_settingArgs.dataBaseLoad = settings.value("dataBaseLoad",m_settingArgs.dataBaseLoad).toBool();
    m_settingArgs.windowTop = settings.value("windowTop",m_settingArgs.windowTop).toBool();
    m_settingArgs.usnRecordNoMonitoring = settings.value("usnRecordNoMonitoring",m_settingArgs.usnRecordNoMonitoring).toBool();


    for(int i= 0;i<m_settingArgs.columns.count();i++)
        m_settingArgs.columns[i] = settings.value(QString("Column")+QString::number(i),m_settingArgs.columns.at(i)).toBool();
    settings.endGroup();
}

//初始化mode列表
void MemoryPool::initModelList()
{
    //获取所有盘符
    MemoryPool::getInstance()->getPartitionList()->clear();
    for(int i=0;i<MemoryPool::getInstance()->getInfoList()->count();i++) {
        QString str = QString("%1%2%3").arg(MemoryPool::getInstance()->getInfoList()->at(i).info.partitionFlagName).arg(QChar(' ')).arg(QString::number(MemoryPool::getInstance()->getInfoList()->at(i).partitionSerial));
        MemoryPool::getInstance()->getPartitionList()->insert(str,GlobalFunction::getInstance()->driveIcon());
    }


    //获取所有文件类型
    MemoryPool::getInstance()->getFlagList()->clear();
    for(int i=0;i<g_FileFlagL.count();i++) {
        MemoryPool::getInstance()->getFlagList()->insert(g_FileFlagL.at(i),QIcon());
    }

    m_iconModel->initStrData(m_iconList);
    m_partitionModel->initStrData(m_partitionList);
    m_flagModel->initData(m_flagList);
    m_attModel->initNumData(m_attList);

}
//管理窗口
QList<WindowClass*>* MemoryPool::getWindows()
{
    QMutexLocker _lock(&m_windowListMutex);
    return windowList;
}

//-------------------------------------------------------------------------------------------------------
//                                  listModel类
//-------------------------------------------------------------------------------------------------------
MyListModel::MyListModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

//初始字符串数据
void MyListModel::initStrData(QHash<QString,QIcon>* temp)
{
    m_iconList = temp;
    extendList();
}
//初始数字数据
void MyListModel::initNumData(QHash<QString,QIcon>* temp)
{
    m_iconList = temp;
    numberList();
}
//初始数据
void MyListModel::initData(QHash<QString,QIcon>* temp)
{
    m_iconList = temp;
    defaultList();
}

QVariant MyListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QAbstractListModel::headerData( section, orientation,  role);
}

int MyListModel::rowCount(const QModelIndex &parent) const
{

    if (parent.isValid())
        return 0;

    return m_listItem.count();
}

QVariant MyListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole )//显示数据
    {
        return m_listItem.at(index.row()).name;
    }

    if (role == Qt::DecorationRole)//显示icon
    {
        return m_listItem.at(index.row()).icon;
    }
    if (role == Qt::TextAlignmentRole)
    {    //对其方式
        return Qt::AlignLeft; //左对齐
    }
    return QVariant();
}
//数字排序
void MyListModel::numberList()
{
    m_tempListItem.clear();
    LIST_ITEM exIcon;
    QHash<QString, QIcon>::const_iterator iter= m_iconList->constBegin();
    while(iter!= m_iconList->constEnd()){
        exIcon.name = iter.key();
        exIcon.icon = iter.value();
        m_tempListItem.append(exIcon);
        iter++;
    }
    //扩展名排序 从小到大
    std::sort(m_tempListItem.begin(),m_tempListItem.end(),[](const LIST_ITEM &infoA,const LIST_ITEM &infoB)
    {
        return infoA.name.toUInt() < infoB.name.toUInt();
    });
    LIST_ITEM temp;
    temp.name=DEFAULT_ALL;
    temp.icon=QIcon("");
    m_tempListItem.prepend(temp);//在列表最前面加入全部
    beginResetModel();
    m_listItem.swap(m_tempListItem);
    endResetModel();
    m_tempListItem.clear();
}

//扩展名和图标处理显示
void MyListModel::extendList()
{
    m_tempListItem.clear();
    LIST_ITEM exIcon;

    QHash<QString, QIcon>::const_iterator iter= m_iconList->constBegin();
    while(iter!= m_iconList->constEnd()){
        exIcon.name = iter.key();
        exIcon.icon = iter.value();
        m_tempListItem.append(exIcon);
        iter++;
    }
    //扩展名排序 从小到大
    std::sort(m_tempListItem.begin(),m_tempListItem.end(),[](const LIST_ITEM &infoA,const LIST_ITEM &infoB){return infoA< infoB;});
    LIST_ITEM temp;
    temp.name=DEFAULT_ALL;
    temp.icon=QIcon("");
    m_tempListItem.prepend(temp);//在列表最前面加入全部
    beginResetModel();
    m_listItem.swap(m_tempListItem);
    endResetModel();
    m_tempListItem.clear();

}
//默认排序
void MyListModel::defaultList()
{
    m_tempListItem.clear();
    LIST_ITEM exIcon;
    QHash<QString, QIcon>::const_iterator iter= m_iconList->constBegin();
    while(iter!= m_iconList->constEnd()){
        exIcon.name = iter.key();
        exIcon.icon = iter.value();
        m_tempListItem.append(exIcon);
        iter++;
    }
    //扩展名排序 从小到大

    LIST_ITEM temp;
    temp.name=DEFAULT_ALL;
    temp.icon=QIcon("");
    m_tempListItem.prepend(temp);//在列表最前面加入全部
    beginResetModel();
    m_listItem.swap(m_tempListItem);
    endResetModel();
    m_tempListItem.clear();
}
