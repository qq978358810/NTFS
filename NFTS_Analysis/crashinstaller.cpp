
#include "crashinstaller.h"
#include <QDesktopServices>
#include <QCoreApplication>
#include <QMessageBox>
#include <Windows.h>
#include <QDateTime>
#include <DbgHelp.h>
#include <QDebug>
#include <QDir>
#include "IncludeFile.h"
//生成的dump日志里存放的信息
LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
    QString dumpPath = LOG_PATH+QDir::separator()+QDateTime::currentDateTime().toString("yyyy-MM-dd HH_mm_ss.zzz")+".dmp";
    qDebug()<<dumpPath;

    HANDLE hDumpFile = CreateFile(dumpPath.toStdWString().c_str(),GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hDumpFile != INVALID_HANDLE_VALUE)
    {
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = TRUE;
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
    }

    QMessageBox::critical(NULL, "程序异常", "请联系开发者!");

    return EXCEPTION_EXECUTE_HANDLER;
}

CrashInstaller::CrashInstaller(QObject *parent) : QObject(parent)
{

}
void CrashInstaller::installCrashHandler()
{
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
}
