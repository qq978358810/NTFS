#include "mycheckboxcombo.h"
#include <QLineEdit>
#include <QMouseEvent>

XComboBox::XComboBox(QWidget *parent)
    : QComboBox(parent)
{
    pLineEdit = new QLineEdit(this);
    pLineEdit->setReadOnly(true);
    this->setLineEdit(pLineEdit);
    this->lineEdit()->disconnect();

    KeyPressEater *keyPressEater = new KeyPressEater(this);
    pListView = new QListView(this);
    pListView->installEventFilter(keyPressEater);
    this->setView(pListView);

    this->setModel(&m_model);

    connect(this, QOverload<int>::of(&QComboBox::activated), this, &XComboBox::sltActivated);
    connect(keyPressEater, &KeyPressEater::sigActivated, this, &XComboBox::sltActivated);
}

XComboBox::~XComboBox()
{
}

void XComboBox::AddItem(const QString& str, bool bChecked /*= false*/, const QVariant &userData /*= QVariant()*/)
{
    QStandardItem* item = new QStandardItem(str);
    item->setCheckable(true);
    item->setCheckState(bChecked ? Qt::Checked : Qt::Unchecked);
    item->setData(userData, Qt::UserRole + 1);
    m_model.appendRow(item);

    UpdateText();
}

void XComboBox::AddItems(const QList<ItemInfo>& lstItemInfo)
{
    for (auto a : lstItemInfo)
        AddItem(a.str, a.bChecked, a.userData);
}

void XComboBox::AddItems(const QMap<QString, bool>& mapStrChk)
{
    for (auto it = mapStrChk.begin(); it != mapStrChk.end(); ++it)
        AddItem(it.key(), it.value());
}

void XComboBox::AddItems(const QList<QString>& lstStr)
{
    for (auto a : lstStr)
        AddItem(a, false);
}
QStringList XComboBox::GetSelItemsText()
{
    QStringList lst;
    QString str = pLineEdit->text();
    if (str.isEmpty())
        return lst;
    else{
        lst = pLineEdit->text().split(",");
        return lst;
    }
}

QList<ItemInfo> XComboBox::GetSelItemsInfo()
{
    QList<ItemInfo> lstInfo;
    for (int i = 0; i < m_model.rowCount(); i++){
        QStandardItem* item = m_model.item(i);
        if (item->checkState() == Qt::Unchecked) continue;

        ItemInfo info;
        info.idx = i;
        info.str = item->text();
        info.bChecked = true;
        info.userData = item->data(Qt::UserRole + 1);

        lstInfo << info;
    }

    return lstInfo;
}

QString XComboBox::GetItemText(int idx)
{
    if (idx < 0 || idx >= m_model.rowCount())
        return QString("");
    return m_model.item(idx)->text();
}

ItemInfo XComboBox::GetItemInfo(int idx)
{
    ItemInfo info;
    if (idx < 0 || idx >= m_model.rowCount())
        return info;
    QStandardItem* item = m_model.item(idx);
    info.idx = idx;
    info.str = item->text();
    info.bChecked = (item->checkState() == Qt::Checked);
    info.userData = item->data(Qt::UserRole + 1);
    return info;
}

// 查找目标项，找到则返回所在行号,否则返回-1
int XComboBox::FindItem(const QVariant &Key) const
{
    for (int nRow = 0; nRow < m_model.rowCount(); nRow++){
        QStandardItem *pItem = m_model.item(nRow);
        if (pItem == nullptr)
            continue;
        if (pItem->data(Qt::UserRole + 1) == Key)
            return nRow;
    }
    return -1;
}

void XComboBox::showPopup()
{
    QComboBox::showPopup();
}

void XComboBox::hidePopup()
{
    int width = this->view()->width();//视口的宽度
    int height = this->view()->height();//视口的高度

    QPoint g_point =QPoint(this->mapToGlobal(QPoint(0,0)).x(),this->mapToGlobal(QPoint(0,0)).y()) ; //获取窗口左上角全局坐标
    QRect rectView(g_point.x(), g_point.y()+this->height(), width, height);
    if (!rectView.contains(QCursor::pos().x(), QCursor::pos().y()))//鼠标点击不在构建的范围内，关闭视口
        QComboBox::hidePopup();
}

void XComboBox::mousePressEvent(QMouseEvent * event)
{
    QComboBox::mousePressEvent(event);
    event->accept();
}

void XComboBox::mouseReleaseEvent(QMouseEvent * event)
{
    QComboBox::mouseReleaseEvent(event);
    event->accept();
}

void XComboBox::mouseMoveEvent(QMouseEvent * event)
{
    QComboBox::mouseMoveEvent(event);
    event->accept();
}

void XComboBox::UpdateText()
{
    QStringList lstTxt;
    for (int i = 0; i < m_model.rowCount(); ++i){
        QStandardItem* item = m_model.item(i);
        if (item->checkState() == Qt::Unchecked)
            continue;
        lstTxt << item->text();
    }

    pLineEdit->setText(lstTxt.join(","));
    pLineEdit->setToolTip(lstTxt.join("\n"));
}

void XComboBox::sltActivated(int idx)
{
    QStandardItem* item = m_model.item(idx);
    if (nullptr == item)
        return;

    Qt::CheckState state = (item->checkState() == Qt::Checked) ? Qt::Unchecked : Qt::Checked;
    item->setCheckState(state);

    UpdateText();
}
//获取所有选择状态
QList<BOOL> XComboBox::GetShowColumns()
{
    QList<BOOL> map;
    for (int i = 0; i < m_model.rowCount(); ++i){
        QStandardItem* item = m_model.item(i);
        map.append(item->checkState() == Qt::Unchecked ? false : true);
    }
    return map;
}
//重置所有项
void XComboBox::resetItems(Qt::CheckState state)
{
    for (int i = 0; i < m_model.rowCount(); ++i){
        QStandardItem* item = m_model.item(i);
        item->setCheckState(state);
    }
    UpdateText();
}
void  XComboBox::resetItems(QList<BOOL> items)
{
    for (int i = 0; i < m_model.rowCount(); ++i) {
        QStandardItem* item = m_model.item(i);
        item->setCheckState(items.at(i) == false ? Qt::Unchecked : Qt::Checked );
    }
    UpdateText();
}
