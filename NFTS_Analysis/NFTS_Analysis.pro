QT       += core gui sql concurrent winextras xlsx xml
RC_ICONS = ntfs.ico
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
QMAKE_CXXFLAGS_EXCEPTIONS_ON = /EHa
QMAKE_CXXFLAGS_STL_ON = /EHa
# Release 版本包含这些信息:文件名、函数名、行数 -> qInstallMessageHandler
DEFINES += QT_MESSAGELOGCONTEXT
# 在.pro 中增加如下配置 ==》 Qt默认是不开启异常的
CONFIG += exceptions
# 运行文件附带调试输出窗口
#CONFIG += console
# 多线程编译，默认是-j8
MAKEFLAGS=-j12
#if defined(_MSC_VER)
#pragma execution_character_set("utf-8")
#endif
#报警告报warning: unknown pragma ignored，原来只是中文乱码问题，现在编译不过了。
#继续查，发现execution_character_set只在vs2015里支持，继续搜索方案，解决方法如下：
#在pro文件添加
win32{
    QMAKE_CXXFLAGS += /source-charset:utf-8 /execution-charset:utf-8
}
#编译时生成pdb文件
QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

# build dump + pdb
#CONFIG(debug,debug|release){
#    QT_BUILD_MODE = debug
#}else{
#    QT_BUILD_MODE = release
#    QMAKE_LFLAGS_RELEASE = /INCREMENTAL:NO /DEBUG
#}

# 程序名
TARGET = "QuickRun"
# 版本信息
VERSION = 1.1.2.62722

# 产品名称
QMAKE_TARGET_PRODUCT = "QuickRun"

# 文件说明
QMAKE_TARGET_DESCRIPTION = "QuickRun"

# 版权信息
QMAKE_TARGET_COPYRIGHT = "Copyright (c) 2021.8.31"

# 中文（简体）
RC_LANG = 0x0804
#获取管理员权限-msvc版本获取管理员权限的方法
QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"

LIBS += -luser32
LIBS += -ladvapi32

SOURCES += \
    Database/mysqlite.cpp \
    IncludeFile.cpp \
    crashinstaller.cpp \
    datamanage.cpp \
    highfinddialog.cpp \
    logmodel.cpp \
    logwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    mycheckboxcombo.cpp \
    mytablemodel.cpp \
    mytableview.cpp \
    ntfs.cpp \
    ntfsdatarecovery.cpp \
    ntfssysinfo.cpp \
    readwritexml.cpp \
    settingdialog.cpp \
    usnmonitordialog.cpp \
    windowclass.cpp

HEADERS += \
    Database/mysqlite.h \
    IncludeFile.h \
    crashinstaller.h \
    datamanage.h \
    highfinddialog.h \
    logmodel.h \
    logwindow.h \
    mainwindow.h \
    mycheckboxcombo.h \
    mytablemodel.h \
    mytableview.h \
    ntfs.h \
    ntfsdatarecovery.h \
    ntfssysinfo.h \
    pdqSort.h \
    readwritexml.h \
    settingdialog.h \
    usnmonitordialog.h \
    windowclass.h

FORMS += \
    highfinddialog.ui \
    logwindow.ui \
    settingdialog.ui \
    usnmonitordialog.ui \
    windowclass.ui
   # LIBS +=User32.LIB
   #win32 {
   #   LIBS += -luser32
   #}
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Images.qrc
