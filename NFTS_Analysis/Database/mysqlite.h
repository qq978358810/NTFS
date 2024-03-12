#ifndef MYSQLITE_H
#define MYSQLITE_H

#include <QSqlDatabase>
#include <QDebug>
#include <QElapsedTimer>
#include <QSqlQuery>
#include <QSqlError>
#include "IncludeFile.h"
class MySQLite : public QObject
{
    Q_OBJECT
public:
    explicit MySQLite(QString connectName, QObject *parent = nullptr);

    ~MySQLite();

signals:

public slots:
    void initWriteDatas();//初始化数据库 -- 写入
    void initOneWriteDatas(S_INIT_DATAS_MODEL);//初始化数据库 -- 写入
    S_INIT_DATAS_MODEL initReadDatas(QString);//初始化数据库 -- 读取
    bool connectDB(); //连接数据库
    bool createTable(QString); //创建

private:
    QString m_name;
    QString m_connectName;
    QSqlDatabase MySQLite_db;

};

#endif // MYSQLITE_H
