

/*
 * 由于时间有限，暂时功能增加增加这几个。通过写这个程序发现，没有人能写出没有bug的程序，bug并不是一件坏事，
 * 它只会让一个程序越来越完美，但永远不会有bug，谁都无法保证，没有bug，不代表以后没有bug，它只是还没有被发现。
 * 一个没有bug的产品，迟早会有一个更大bug。
 */
#include "mainwindow.h"
#include <QApplication>
#include <QTextStream>
#include "crashinstaller.h"

int main(int argc, char *argv[])
{

    CrashInstaller::installCrashHandler();   //2.调用静态函数
    qRegisterMetaType<E_LOG_TYPE>("E_LOG_TYPE");//LOG_TYPE 自定义类型用于signal和slot
    qRegisterMetaType<E_DATABASE_TYPE>("E_DATABASE_TYPE");
    qRegisterMetaType<S_DATA_SUBSET>("S_DATA_SUBSET");
    qRegisterMetaType<QList<QPersistentModelIndex>>("QList<QPersistentModelIndex>");
    qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>("QAbstractItemModel::LayoutChangeHint");
    qRegisterMetaType<Qt::Orientation>("Qt::Orientation");
    qRegisterMetaType<S_INIT_DATAS_MODEL>("S_INIT_DATAS_MODEL");
    qRegisterMetaType<E_STATUSBAR_UPDATA>("E_STATUSBAR_UPDATA");
    qRegisterMetaType<S_PARTITION_DATA_COUNT>("S_PARTITION_DATA_COUNT");
    qRegisterMetaType<S_UPDATE_VIEW>("S_UPDATE_VIEW");
    qRegisterMetaType<S_RECOVER_INFO>("S_RECOVER_INFO");
    qRegisterMetaType<S_HIGHFIND_UPDATE_VIEW>("S_HIGHFIND_UPDATE_VIEW");
    qRegisterMetaType<S_USN_LOG>("S_USN_LOG");
    qRegisterMetaType<PS_NTFS_INFO>("PS_NTFS_INFO");
    qRegisterMetaType<S_NTFS_INFO>("S_NTFS_INFO");
    qRegisterMetaType<S_MONITOR_ITEM>("S_MONITOR_ITEM");
    qRegisterMetaType<E_CHANGE>("E_CHANGE");

    MainWindow a(argc, argv);
    QObject::connect(&a,&MainWindow::finishAllThread,[&]()
    {
        a.quit();//退出程序
    });
    a.setQuitOnLastWindowClosed(false);//关闭 (没有窗口自动退出程序)
    return a.exec();
}
