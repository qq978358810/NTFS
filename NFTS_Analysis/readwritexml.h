#ifndef READWRITEXML_H
#define READWRITEXML_H

#include <QObject>
#include <QDomText>
#include <QDomDocument>
#include <QDomNode>
#include <QDomElement>
#include <QXmlStreamAttributes>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "IncludeFile.h"

#define  MONITOR_LIST   QCoreApplication::applicationDirPath()+QDir::separator() +"MonitorCfg.xml"

class ReadWriteXml : public QObject
{
    Q_OBJECT
public:
    explicit ReadWriteXml( QObject *parent = nullptr);
    //读xml
    QList<S_MONITOR_ITEM> ReadXml();
    quint64 ReadTimeXml();
    //增加xml内容
    void AddXml();
    //删减xml内容
    void RemoveXml();
    //更新xml内容
    void UpdateXml(QList<S_MONITOR_ITEM>,quint64);

signals:

public slots:

private:
    QString m_fileName;

};

#endif // READWRITEXML_H
