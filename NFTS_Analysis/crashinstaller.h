#ifndef CRASHINSTALLER_H
#define CRASHINSTALLER_H

#include <QObject>
#pragma comment(lib,"Dbghelp.lib")

class CrashInstaller : public QObject
{
    Q_OBJECT
public:
    explicit CrashInstaller(QObject *parent = nullptr);
    static void installCrashHandler();
signals:

};

#endif // CRASHINSTALLER_H
