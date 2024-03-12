#include "mysqlite.h"
MySQLite::MySQLite(QString connectName,QObject *parent) : QObject(parent)
{
    m_connectName = connectName;
    m_name = "Sqlite";
    m_name += SPLIT_SYMBOLS;
    qDebug()<<m_name<<"构造...";
}
MySQLite::~MySQLite()
{
    if(MySQLite_db.isOpen())
        MySQLite_db.close();
    qDebug()<<m_name<<"析构~";
}
bool MySQLite::connectDB()
{
    qDebug()<<m_name<<"支持的数据库类型:"<<QSqlDatabase::drivers();//打印qt支持的数据库类型
//    if(QSqlDatabase::contains("qt_sql_default_connection"))
//        MySQLite_db = QSqlDatabase::database("qt_sql_default_connection");//默认连接
//    else
//        MySQLite_db = QSqlDatabase::addDatabase("QSQLITE",m_connectName);//m_connectName;创建不同的连接名，用于多线程

    if(QSqlDatabase::contains(m_connectName)) {
        MySQLite_db = QSqlDatabase::database(m_connectName);
    } else {
        MySQLite_db = QSqlDatabase::addDatabase("QSQLITE",m_connectName);//m_connectName;创建不同的连接名，用于多线程
    }
    static QDir dir;
    static QString path = QCoreApplication::applicationDirPath()+"\\DATABASE\\";
    if(dir.mkpath(path)){
        MySQLite_db.setDatabaseName( path + QString("%1.db").arg(QCoreApplication::applicationName()));
    }

    if (!MySQLite_db.open()){
        qDebug()<<m_name<<"打开数据库失败!";
        return false;
    }
    qDebug()<<m_name<<"打开数据库成功!";
    QSqlQuery query(MySQLite_db);
    query.exec("PRAGMA synchronous=OFF;");//PRAGMA synchronous = OFF 关闭同步模式，减少时间
    query.exec("PRAGMA Journal_Mode=WAL;");
    query.exec("PRAGMA Cache_Size=5000;");
    return true;
}

bool MySQLite::createTable(QString tableName)
{
    if(g_SqliteHeadL.count()!=11)
        Q_ASSERT(0);
    QSqlQuery query(MySQLite_db);

    //查询表是否存在
    query.exec(QString("select count(*) from sqlite_master where type='table' and name='%1'").arg(tableName));
    if(query.next()){
        if(query.value(0).toInt()>0){//表存在
            //删除表
            query.exec(QString("DROP TABLE '%1'").arg(tableName));
            qDebug()<<m_name<<"删除表:"<<tableName;
        }
    }
    //保存文件路径表
    QString ss =QString("CREATE TABLE if not exists '%1' ("//if not exists
                        //" 'ID'	INTEGER NOT NULL,"      //id
                        " '%2'	INTEGER NOT NULL,"//mft号
                        " '%3'	INTEGER NOT NULL,"//父mft号
                        " '%4'	INTEGER NOT NULL,"
                        " '%5'	TEXT NOT NULL,"//文件名
                        " '%6'	INTEGER NOT NULL,"//文件创建时间
                        " '%7'	INTEGER NOT NULL,"//文件修改时间
                        " '%8'	INTEGER NOT NULL," //
                        " '%9'	INTEGER NOT NULL,"//
                        " '%10'	INTEGER NOT NULL," //
                        " '%11'	INTEGER NOT NULL,"//
                        " '%12'	INTEGER NOT NULL"//
                        //" PRIMARY KEY('MftNumber')"
                        " )").arg(tableName).arg(g_SqliteHeadL.at(0)).arg(g_SqliteHeadL.at(1)).arg(g_SqliteHeadL.at(2)).arg(g_SqliteHeadL.at(3))
            .arg(g_SqliteHeadL.at(4)).arg(g_SqliteHeadL.at(5)).arg(g_SqliteHeadL.at(6)).arg(g_SqliteHeadL.at(7)).arg(g_SqliteHeadL.at(8))
            .arg(g_SqliteHeadL.at(9)).arg(g_SqliteHeadL.at(10));

    if(!(query.exec(ss))){
        qDebug()<<m_name<<"生成表("+tableName+")失败:"<<query.lastError();
        return false;
    }
    qDebug()<<m_name<<"生成表("+tableName+")成功!";
    return true;

}
//单个盘保存
void MySQLite::initOneWriteDatas(S_INIT_DATAS_MODEL modelData)
{
    if (!connectDB())
        return ;

    QElapsedTimer time;
    time.start();

    QString tableName = QUuid(modelData.info.uuid).toString();
    qDebug()<<m_name<<"数据库表("+tableName+")数据正在保存...";
    if(createTable(tableName)){
        QVariantList _1;
        QVariantList _2;
        QVariantList _3;
        QVariantList _4;
        QVariantList _5;
        QVariantList _6;
        QVariantList _7;
        QVariantList _8;
        QVariantList _9;
        QVariantList _10;
        QVariantList _11;
        MySQLite_db.transaction();
        QSqlQuery query(MySQLite_db);
        QString insert_sql = QString("INSERT INTO '%1' ('%2','%3','%4','%5','%6','%7','%8','%9','%10','%11','%12')\
                                     VALUES(?,?,?,?,?,?,?,?,?,?,?)").arg(tableName).arg(g_SqliteHeadL.at(0)).arg(g_SqliteHeadL.at(1)).arg(g_SqliteHeadL.at(2)).arg(g_SqliteHeadL.at(3))
                                     .arg(g_SqliteHeadL.at(4)).arg(g_SqliteHeadL.at(5)).arg(g_SqliteHeadL.at(6)).arg(g_SqliteHeadL.at(7)).arg(g_SqliteHeadL.at(8))
                                     .arg(g_SqliteHeadL.at(9)).arg(g_SqliteHeadL.at(10));
                query.prepare(insert_sql);
        QHash<quint32,S_DATA_BODY*>::const_iterator i =modelData.dataBodys.constBegin();

        while (i != modelData.dataBodys.constEnd()){
            _1<<i.value()->MFTNumber;
            _2<<i.value()->PMFTNumber;
            _3<<i.value()->bodyAtt.FileAttributes;
            _4<<i.value()->name;
            _5<<i.value()->bodyAtt.creationTime;
            _6<<i.value()->bodyAtt.changeTime;
            _7<<i.value()->bodyAtt.lastReadTime;
            _8<<i.value()->bodyAtt.fileRealSize;
            _9<<i.value()->bodyAtt.fileAiiocSize;
            _10<<i.value()->bodyAtt.flag;
            _11<<i.value()->partitionSerial;
            ++i;
        }

        query.addBindValue( _1);
        query.addBindValue( _2);
        query.addBindValue( _3);
        query.addBindValue( _4);
        query.addBindValue( _5);
        query.addBindValue( _6);
        query.addBindValue( _7);
        query.addBindValue( _8);
        query.addBindValue( _9);
        query.addBindValue( _10);
        query.addBindValue( _11);
        if(!query.execBatch()){
            qDebug()<<m_name<<"数据库表("+tableName+")保存失败:"<<query.lastError();
            MySQLite_db.rollback();
            return;
        }else{
            MySQLite_db.commit();
        }
    }

    QSqlQuery query(MySQLite_db);
    query.exec(QString("VACUUM %1").arg(tableName));//压缩空间

    if(MySQLite_db.isOpen()){
        MySQLite_db.close();
    }
    qDebug()<<m_name<<"数据库表("+tableName+")保存成功耗时:"<<time.elapsed()<<" ms";

}
//初始化数据库 -- 写入
void MySQLite::initWriteDatas()
{
    qDebug()<<m_name<<"数据库写入全部数据Start...";
    QList<S_INIT_DATAS_MODEL>*  initDatas = MemoryPool::getInstance()->getInfoList();
    if (!connectDB() || initDatas->isEmpty())
        return ;

    QElapsedTimer time;
    time.start();

    for(int i=0;i<initDatas->count();++i){
        S_INIT_DATAS_MODEL modelData = initDatas->at(i);
        QString tableName = QUuid(modelData.info.uuid).toString();
        if(createTable(tableName)){
            qDebug()<<m_name<<"数据库表("+tableName+")数据正在保存...";
            QElapsedTimer t;
            t.start();
            QVariantList _1;
            QVariantList _2;
            QVariantList _3;
            QVariantList _4;
            QVariantList _5;
            QVariantList _6;
            QVariantList _7;
            QVariantList _8;
            QVariantList _9;
            QVariantList _10;
            QVariantList _11;

            QSqlQuery query(MySQLite_db);
            QString insert_sql = QString("INSERT INTO '%1' ('%2','%3','%4','%5','%6','%7','%8','%9','%10','%11','%12')\
                                         VALUES(?,?,?,?,?,?,?,?,?,?,?)").arg(tableName).arg(g_SqliteHeadL.at(0)).arg(g_SqliteHeadL.at(1)).arg(g_SqliteHeadL.at(2)).arg(g_SqliteHeadL.at(3))
                                         .arg(g_SqliteHeadL.at(4)).arg(g_SqliteHeadL.at(5)).arg(g_SqliteHeadL.at(6)).arg(g_SqliteHeadL.at(7)).arg(g_SqliteHeadL.at(8))
                                         .arg(g_SqliteHeadL.at(9)).arg(g_SqliteHeadL.at(10));
                    query.prepare(insert_sql);
            QHash<quint32,S_DATA_BODY*>::const_iterator i =modelData.dataBodys.constBegin();
            MySQLite_db.transaction();
            while (i != modelData.dataBodys.constEnd()){
                _1<<i.value()->MFTNumber;
                _2<<i.value()->PMFTNumber;
                _3<<i.value()->bodyAtt.FileAttributes;
                _4<<i.value()->name;
                _5<<i.value()->bodyAtt.creationTime;
                _6<<i.value()->bodyAtt.changeTime;
                _7<<i.value()->bodyAtt.lastReadTime;
                _8<<i.value()->bodyAtt.fileRealSize;
                _9<<i.value()->bodyAtt.fileAiiocSize;
                _10<<i.value()->bodyAtt.flag;
                _11<<i.value()->partitionSerial;
                ++i;
            }

            query.addBindValue( _1);
            query.addBindValue( _2);
            query.addBindValue( _3);
            query.addBindValue( _4);
            query.addBindValue( _5);
            query.addBindValue( _6);
            query.addBindValue( _7);
            query.addBindValue( _8);
            query.addBindValue( _9);
            query.addBindValue( _10);
            query.addBindValue( _11);

            if(!query.execBatch()){
                qDebug()<<m_name<<"数据库表("+tableName+")保存失败:"<<query.lastError();
                MySQLite_db.rollback();
                return;
            }else{
                MySQLite_db.commit();
                qDebug()<<m_name<<"数据库表("+tableName+")保存成功耗时:"<<t.elapsed()<<" ms";
            }
        }
    }

    QSqlQuery query(MySQLite_db);
    query.exec(QString("VACUUM"));//压缩空间

    if(MySQLite_db.isOpen()){
        MySQLite_db.close();
    }
    qDebug()<<m_name<<"数据库全部写入End;耗时:"<<time.elapsed()<<" ms";
}
//初始化数据库 -- 读取
S_INIT_DATAS_MODEL MySQLite::initReadDatas(QString tableName)
{
    S_INIT_DATAS_MODEL initDatasInfo;
    if (!connectDB())
        return initDatasInfo;

    qDebug()<<m_name<<"数据库读取("+tableName+")数据Start...";
    QElapsedTimer time;
    time.start();

    QSqlQuery query(MySQLite_db);

    //查询表是否存在
    query.exec(QString("select count(*) from sqlite_master where type='table' and name='%1'").arg(tableName));
    if(query.next()){
        if(query.value(0).toInt()==0){//表不存在
            return initDatasInfo;
        }
    }

    //查询数据
    query.prepare(QString( "SELECT * FROM  '%1'").arg(tableName));
    query.exec();	//执行
    query.setForwardOnly(true);

    while (query.next()){
        S_DATA_BODY * temp = new S_DATA_BODY;
        temp->MFTNumber = query.value("MftNumber").toUInt();
        temp->PMFTNumber = query.value("PMftNumber").toUInt();
        temp->bodyAtt.FileAttributes = query.value("FileAttribute").toUInt();
        temp->name = query.value("FileName").toByteArray();
        temp->bodyAtt.creationTime = query.value("CreationTime").toULongLong();
        temp->bodyAtt.changeTime = query.value("ChangeTime").toULongLong();
        temp->bodyAtt.lastReadTime = query.value("LastReadTime").toULongLong();
        temp->bodyAtt.fileRealSize = query.value("RealSize(Byte)").toULongLong();
        temp->bodyAtt.fileAiiocSize = query.value("AiiocSize(Byte)").toULongLong();
        temp->bodyAtt.flag = query.value("FileType").toUInt();
        switch (temp->bodyAtt.flag){//统计数量
        case 0x0:
            initDatasInfo.dataBodyCount._falg_0_count++;
            break;
        case 0x1:
            initDatasInfo.dataBodyCount._falg_1_count++;
            break;
        case 0x2:
            initDatasInfo.dataBodyCount._falg_2_count++;
            break;
        case 0x3:
            initDatasInfo.dataBodyCount._falg_3_count++;
            break;
        default:
            initDatasInfo.dataBodyCount._falg_other_count++;
            break;
        }
        initDatasInfo.dataBodys.insertMulti(temp->MFTNumber,temp);   //将查询到的内容存到testInfo向量中
    }

    if(MySQLite_db.isOpen())
        MySQLite_db.close();
    qDebug()<<m_name<<"数据库读取("+tableName+")数据End;耗时:"<<time.elapsed()<<" ms";
    return initDatasInfo;
}
