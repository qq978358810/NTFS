
---
@[TOC](文章目录)

---

# 0前言说明
## 0.1 软件说明
1：本软件只适用于Windows系统，主要偏重整体框架和UI交互等。
2：主要功能包括文件名快速搜索，删除的文件恢复，usnjournal监控，多窗口同时显示等功能。
## 0.2 特别说明
1：程序只获取NTFS系统下（包括GPT下的隐藏盘（隐藏盘只获取GPT分区下的）和U盘符）的文件名数据(非NFTS系统暂不处理)。
2：程序采集NTFS系统是动态运行，ntfs系统U盘的插入和拔出，程序自动获取数据和移除数据，格式化自动更新数据，更改盘符自动更新盘符名。
3：程序中对盘符的信息采用定时获取和比对，目的是节流。
4：程序打开盘符或打开物理硬盘只采用只读的形式打开，不做更改。
5：程序从NTFS系统中只获取一些文件的基本信息。
6：程序中对文件名排序的算法使用三路快排，其他用到的是快排。
7：程序中的文件图标从Windows注册表中解析后，存储到hash表中，实现快速获取。
8：程序中的文件数据从NTFS系统下的MFT表解析后，存储到hash表中（保存到内存中）实现快速搜索。
9：程序获取usnjournal然后写入到本地文件，本地文件发生改变，usnjournal还会获取，，，这样会造成一个死循环浪费cpu性能、占用不必要的内存。解决方法：程序内部获取要写入的日志文件夹(MFT号)，从而过滤掉文件夹下的所有文件的变化。
9：系统采用纯QtWidget（非qml）编写，同时集成了数据库、多线程、配置文件、UI布局等众多知识点。
## 0.3 功能特点
1：文件：新建窗口，导出，关闭窗口，退出程序。
2：日志：USN日志，运行日志。
3：视图：排序。
4：搜索：区分大小写，前置匹配，高级搜索，使用正则表达式，扩展名。
5：选项：设置，关于。
6：设置：开机自启动，显示托盘图标，实时搜索，隐藏搜索按钮，正常文件启用(恢复/复制数据)，数据保存到数据库，窗口置顶，usnjournal不监控（不更新数据），隐藏某列。
![在这里插入图片描述](https://img-blog.csdnimg.cn/9e6093fd8580469b8d38130c7d1e1e60.gif#pic_center)

# 1程序说明
## 1.1文件

![在这里插入图片描述](https://img-blog.csdnimg.cn/48934083252d4a4c84cc8d44b0f44d05.png)


### 1.1.1：新建窗口
实现：
```cpp

//创建界面
void MainWindow::createUi()
{
    SlotNewWindow(initWindowRect());
}
//初始化窗口大小和位置
QRect MainWindow::initWindowRect()
{
    QDesktopWidget *screenResolution = QApplication::desktop();
    int h = 600;//默认窗口的高度
    int w = 1000;//默认窗口的宽度
    //生成随机位置
    quint32 w_rand =QRandomGenerator::global()->generate()%10;
    quint32 h_rand =QRandomGenerator::global()->generate()%10;
    return QRect((screenResolution->width()-w)/(w_rand == 0 ? 1 : w_rand),(screenResolution->height()- h)/(h_rand == 0 ? 1 : h_rand),w, h);
}
//新建窗口
void MainWindow::SlotNewWindow(QRect rect)
{
    WindowClass *window = new WindowClass(rect);
    window->show();
    connect(m_dataManage,&DataManage::SignalDataChange,window->getModel(),&MyTableModel::SlotDataChange,Qt::QueuedConnection);//数据管理发送到model
    MemoryPool::getInstance()->getWindows()->append(window);
}
```

### 1.1.2：导出
对当前显示的数据导出。保存形式有（.txt 、.csv 、.xlsx），xlsx用到的是外部库[链接](https://blog.csdn.net/weixin_43763292/article/details/122791286)
![在这里插入图片描述](https://img-blog.csdnimg.cn/f57063cdcba44109a1638bf82486190d.png)
### 1.1.3：关闭窗口

对当前窗口关闭（如果设置中【显示托盘图标启用】，对当前窗口会隐藏。），如果是最后一个窗口，直接突出程序。
### 1.1.4：退出程序
直接关闭程序。
## 1.2日志

![在这里插入图片描述](https://img-blog.csdnimg.cn/27803ceda04f42f29d38011f8fee777d.png) 
### 1.2.1：usnjournal	
[实现详解](https://blog.csdn.net/weixin_43763292/article/details/125096938)
1：usn日志实现监控：监控当前Windows系统下的所有ntfs系统的数据的变更，每条usnjournal会追加到到界面显示。后台会对采集的数据保存到数据库中。实现用防抖。

```cpp

void RealLogModel::init()
{
    //定时器的创建和调用必须在同一个线程
    timer = new QTimer();
    //设置时间后，只触发一次，单次触发
    if(!timer->isSingleShot()) {
    	timer->setSingleShot(true);
    }   
    connect(timer,&QTimer::timeout,this,[&]()mutable {
    	//写入数据到数据库
        writeDatas();
        beData();
    });

}
//接收数据
void RealLogModel::SlotAddData(S_USN_LOG item)
{
    m_tempDatas.append(item);
    //同时接收大量数据超过设定值，更新界面
    if(m_tempDatas.count() >= ENDCOUNT){
    	//写入数据到数据库
        writeDatas();
        beData();
        return;
    }
    timer->setInterval(100);
    timer->start();

}
```

![在这里插入图片描述](https://img-blog.csdnimg.cn/300defa202a0493a8485d02bbb200ab7.png)2：usn日志监控列表：添加监控项对实时监控的日志记录比对，符合监控项并且启用Windows运行指令，就会执行当前指令（Windows运行指令不做验证，填写当前指令请自己复检可以正常执行，如果指令是错误的就什么也不执行）。添加的监控项会保存到当前程序目录下MonitorCfg.xml文件中，下次启动程序会重新加载到列表中，执行顺序采用当前列表中从上往下判断执行。
Windows运行的最低时长：每次执行监控项会先休眠当前时长，根据情况而设定（如果添加的监控项，全部留空（默认）,然后填上Windows运行命令，时长为0的话，usnjournal只要有记录产生，就会每一个都会执行，造成Windows系统卡、甚至崩溃）
![在这里插入图片描述](https://img-blog.csdnimg.cn/053e9ca20b0a4c30a2d1d583ee0ad175.png)
![在这里插入图片描述](https://img-blog.csdnimg.cn/1625f979a1ee45d48a3cab61884bcce5.png)

### 1.2.2：Run日志
1：运行日志窗口采用Windows控制台窗口：输出当前打印的日志（打开和关闭都用下图中的按钮形式。采用重定向形式，打开后重定向控制台，日志打印到控制台，同时对日志分类保存到日志文件中。点击关闭按钮后重定向程序目录下 "___redirectionFile.txt" 文件下实现控制台的关闭。）
![在这里插入图片描述](https://img-blog.csdnimg.cn/cd9afc768ce3461387b31e4e46ab08cd.png)

![在这里插入图片描述](https://img-blog.csdnimg.cn/b77a438286834bdcb0e9657ead83ef8a.png)![在这里插入图片描述](https://img-blog.csdnimg.cn/f1cd32c68180416391486795e41441ef.png)
## 1.3 视图
### 1.31：排序
![在这里插入图片描述](https://img-blog.csdnimg.cn/8f3459f217c24becbb3497a4f372d0e6.png)
## 1.4 搜索
![在这里插入图片描述](https://img-blog.csdnimg.cn/96be22bc2f944ed9a8e590383798778f.png)

### 1.4.1 区分大小写
对搜索的文件名进行大小写区分，默认不区分。
### 1.4.2 前置匹配
对搜索的文件名前置完全匹配搜索，默认是包含内容搜索。
### 1.4.3 正则表达式
使用正则语法进行搜索，如果正则语法无效，则提示。[正则语法1](https://www.jianshu.com/p/9c4979a3b7e4)        [正则语法2](https://blog.csdn.net/William0318/article/details/102775217?utm_medium=distribute.pc_relevant.none-task-blog-BlogCommendFromBaidu-1.control&depth_1-utm_source=distribute.pc_relevant.none-task-blog-BlogCommendFromBaidu-1.control)
### 1.4.4 扩展名
使用文件后缀进行搜索，后缀包含文件图标显示。
### 1.4.5 高级搜索
实现更精准的搜索方式，同时包含以上搜素方式，包含对某个盘符的数据全部搜索、文件类型的搜索、扩展名的搜索、MFT号的搜索（一个盘只有一个mft号，一个mft号可能包含几个文件名(共享一块内存数据)，mft一般是文件或目录）、PMFT号的搜索（和mft号一样，pmft号一般是目录）、文件属性的搜索（只读，隐藏等等）、文件名的搜索、文件大小的搜索、文件的创建时间和修改时间和访问时间的搜索。同时支持一键恢复到默认值。
![在这里插入图片描述](https://img-blog.csdnimg.cn/c7249205e9664e89aba22412558ac46a.png)
## 1.5 设置

![在这里插入图片描述](https://img-blog.csdnimg.cn/132e83b2065b425ab02ee2ca49a5726a.png)
### 1.5.1 开机自启动
1：有两种启动方式，一种采用注册表方式，这种方式有个弊端，会被忽略掉，开机自启动会失败。
这里实现用的Windows任务计划的方式。每次开机登录的时候启动程序。下面使用Windows命令编程实现：

```cpp
if(checked){
        //采用创建任务计划来完成开机登录自启
        AutoRunprocess->start(QString("schtasks.exe"),QStringList()<<"/create"<<"/tn"<<application_name<<"/rl"<<"highest"<<"/tr"<<application_path<<"/sc"<<"onlogon"<<"/f");

    } else {
    	//取消开机登录自启
        AutoRunprocess->start(QString("schtasks.exe"),QStringList()<<"/delete"<<"/tn"<<application_name<<"/f");
    }
```

![在这里插入图片描述](https://img-blog.csdnimg.cn/dc759105946d4379a3748d7a0b8055a8.png)
### 1.5.2 显示托盘图标
![在这里插入图片描述](https://img-blog.csdnimg.cn/c78071266c654d49b09f62e5d1f5bdd7.png)

启用后会在Windows电脑右下角生成一个小图标，当软件关闭窗口最后一个时，这个时候软件会隐藏，不退出。不启用的话，默认直接退出程序。

### 1.5.3 实时搜索
启用后当主界面文件名编辑框发生改变，就会实时搜索数据。默认是不开启的。
### 1.5.4 隐藏搜索按钮
启用后对主界面的搜索按钮进行隐藏。默认显示。
### 1.5.5 正常文件启用(恢复/复制数据)
![在这里插入图片描述](https://img-blog.csdnimg.cn/9485dade5a034963b85a9b9e70bf9cd3.png)恢复/复制数据：默认是只有对已删除的文件才会启用这个功能，在设置启用后，正常文件也可以使用此功能，可以实现复制数据。

### 1.5.6 数据保存到数据库
启用后程序退出后会将当前的数据保存到数据库（程序目录下database文件夹中），留作备份。（注意：就算保存到数据库，程序下次启用也不会从数据库中加载）
### 1.5.7 窗口置顶
启用后主窗口永远在最前面。
### 1.5.8 UsnJournal不监控(不更新数据)
启用后所有盘符不监控usnjournal，所有文件的变化也不会更新。
### 1.5.9 隐藏某列
只有文件名没勾选，主界面只有文件名列显示。（作用：可以方便的把不感兴趣的信息隐藏）
![在这里插入图片描述](https://img-blog.csdnimg.cn/f0e726c28ce04f638c87900bebe23145.png)

![在这里插入图片描述](https://img-blog.csdnimg.cn/ca2da56aa8194d7faaea902e116536ed.png)
# 2 打包发布
程序采用安装和覆盖安装的形式，同时创建桌面图标和开始菜单链接。
[参考链接](https://blog.csdn.net/qq_40602000/article/details/123139346)

# 3 体验地址
[链接提取码：gmbb ](https://pan.baidu.com/s/1btKngqnrdKHUuBAF5rU23A)
# 4 实现链接
[地址链接](https://blog.csdn.net/weixin_43763292/article/details/123218378)


---

# 总结
`提示：这里对文章进行总结：`
首次更新：2022.6.20



