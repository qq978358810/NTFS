#ifndef MYTABLEVIEW_H
#define MYTABLEVIEW_H

#include <QWidget>
#include <QTableView>
#include "IncludeFile.h"


QT_BEGIN_NAMESPACE
class QMenu;
class QAction;
class QDialog;
class QVBoxLayout;
class QTextEdit;
QT_END_NAMESPACE

class MyTableView : public QTableView
{
    Q_OBJECT
public:
    explicit MyTableView(QWidget *parent = nullptr);

    void contextMenuEvent(QContextMenuEvent *event);//重新实现菜单事件
    void openFile(QString);//打开文件

signals:
public slots:
    void SlotActionOpen();//打开
    void SlotActionOpenPath();//打开路径
    void SlotActiCopyFullPath();//复制完整路径
    void SlotActiCopyPath();//复制路径
    void SlotActiCopy();//复制选择内容
    void SlotActiFileRecover();//文件恢复 -- 针对已删除的文件
    void SlotActiFileInfo();//文件信息
    void SlotDoublenOpen(const QModelIndex &index);//双击打开
    void SlotClicked(const QModelIndex &index);//单击获取索引


private:
    QMenu *m_menu;//右键菜单
    QAction *m_actionOpen;//打开
    QAction *m_actionOpenPath;//打开路径
    QAction *m_actionCopyFullPath;//复制完整路径
    QAction *m_actionCopyPath;//复制路径
    QAction *m_actionCopy;//复制选择内容
    QAction *m_actionFileRecover;//文件恢复 -- 针对已删除的文件
    QAction *m_actionFileInfo;//文件信息
    QDialog *m_dialog;//对话框
    QVBoxLayout *m_dialogLayout;
    QTextEdit * m_dialogShowInfo;

    QPersistentModelIndex m_openPerIndex;
    QPersistentModelIndex perIndex;
    int m_indexRow;
    int m_indexColumn;

};

#endif // MYTABLEVIEW_H
