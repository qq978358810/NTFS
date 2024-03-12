#include "readwritexml.h"
#include <QDebug>
#include <QFile>
ReadWriteXml::ReadWriteXml(QObject *parent) : QObject(parent)
{
}
//读xml
QList<S_MONITOR_ITEM> ReadWriteXml::ReadXml()
{
    QList<S_MONITOR_ITEM> monitorL;
    QFile file(MONITOR_LIST);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug()<<"读取MonitorCfg.xml文件失败";
        return monitorL;
    }
    QXmlStreamReader reader(&file);
    while(!reader.atEnd() && !reader.hasError()) {
        reader.readNextStartElement();

        if (reader.isStartElement() && reader.name() == "MonitorItem") {
            S_MONITOR_ITEM item;
            while(!reader.atEnd() && !reader.hasError()) {
                reader.readNextStartElement();

                if (reader.isStartElement() && reader.name() == "partition") {
                    item.usnLogItem.partition = reader.readElementText();
                } else if (reader.isStartElement() && reader.name() == "mft") {
                    item.usnLogItem.mft = reader.readElementText().toLongLong();
                } else if (reader.isStartElement() && reader.name() == "pMft") {
                    item.usnLogItem.pMft = reader.readElementText().toLongLong();
                } else if (reader.isStartElement() && reader.name() == "reason") {
                    item.usnLogItem.reason = reader.readElementText().toLongLong();
                } else if (reader.isStartElement() && reader.name() == "attributes") {
                    item.usnLogItem.attributes = reader.readElementText().toLongLong();
                } else if (reader.isStartElement() && reader.name() == "fileName") {
                    item.usnLogItem.fileName = reader.readElementText().toUtf8();
                } else if (reader.isStartElement() && reader.name() == "CaseInsensitive") {
                    item.CaseInsensitive = (bool)reader.readElementText().toULong();
                } else if (reader.isStartElement() && reader.name() == "Complete") {
                    item.Complete = (bool)reader.readElementText().toULong();
                } else if (reader.isStartElement() && reader.name() == "RegExp") {
                    item.RegExp = (bool)reader.readElementText().toULong();
                } else if (reader.isStartElement() && reader.name() == "flag") {
                    item.flag = (bool)reader.readElementText().toULong();
                } else if (reader.isStartElement() && reader.name() == "command") {
                    item.command = reader.readElementText();
                } else {
                    monitorL.append(item);
                    break;
                }
            }

        }
    }
    if (reader.hasError()) {
        qDebug() <<"xml错误:"<< reader.errorString();
    }
    file.close();
    return monitorL;
}
quint64 ReadWriteXml::ReadTimeXml()
{
    QFile file(MONITOR_LIST);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug()<<"读取MonitorCfg.xml文件失败";
        return 1000;
    }
    QXmlStreamReader reader(&file);
    while(!reader.atEnd() && !reader.hasError()) {
        reader.readNextStartElement();
        if (reader.isStartElement() && reader.name() == "elapsed") {
            quint64 time = reader.readElementText().toULongLong();
            return time;
        }

    }
    if (reader.hasError()) {
        qDebug() <<"xml错误:"<< reader.errorString();
    }
    file.close();
    return 1000;
}
//增加xml内容
void ReadWriteXml::AddXml()
{


}
//删减xml内容
void ReadWriteXml::RemoveXml()
{

}
//更新xml内容
void ReadWriteXml::UpdateXml(QList<S_MONITOR_ITEM> monitorL,quint64 elapsed)
{
    QFile file(MONITOR_LIST);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);

    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("MonitorCfg");
    stream.writeStartElement("MonitorList");
    for(int i=0;i<monitorL.count();++i) {
        stream.writeStartElement("MonitorItem");
        // stream.writeTextElement("dataTime", monitorL.at(i).usnLogItem.dataTime.toString());
        stream.writeTextElement("partition", monitorL.at(i).usnLogItem.partition);
        //stream.writeTextElement("usnOffset",QString::number( monitorL.at(i).usnLogItem.usnOffset));
        stream.writeTextElement("mft",QString::number( monitorL.at(i).usnLogItem.mft));
        stream.writeTextElement("pMft",QString::number( monitorL.at(i).usnLogItem.pMft));
        // stream.writeTextElement("usn",QString::number( monitorL.at(i).usnLogItem.usn));
        // stream.writeTextElement("reasonDataTime",QString::number( monitorL.at(i).usnLogItem.reasonDataTime));
        stream.writeTextElement("reason",QString::number( monitorL.at(i).usnLogItem.reason));
        stream.writeTextElement("attributes",QString::number( monitorL.at(i).usnLogItem.attributes));
        stream.writeTextElement("fileName",monitorL.at(i).usnLogItem.fileName);

        stream.writeTextElement("CaseInsensitive",QString::number((quint32) monitorL.at(i).CaseInsensitive));
        stream.writeTextElement("Complete",QString::number((quint32)monitorL.at(i).Complete));
        stream.writeTextElement("RegExp",QString::number( (quint32)monitorL.at(i).RegExp));
        stream.writeTextElement("flag",QString::number((quint32) monitorL.at(i).flag));
        stream.writeTextElement("command",monitorL.at(i).command);
        stream.writeEndElement();
    }
    stream.writeEndElement();
    stream.writeTextElement("elapsed",QString::number(elapsed));
    stream.writeEndElement();
    stream.writeEndDocument();
    file.close();
}
