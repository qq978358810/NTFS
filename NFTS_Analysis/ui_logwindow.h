/********************************************************************************
** Form generated from reading UI file 'logwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGWINDOW_H
#define UI_LOGWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LogWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout_3;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *clearButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *exportButton;
    QTableView *m_realTableView;
    QWidget *tab_3;
    QGridLayout *gridLayout_2;
    QPushButton *appendButton;
    QPushButton *insertButton;
    QPushButton *deleteButton;
    QPushButton *updateButton;
    QPushButton *upButton;
    QPushButton *downButton;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QDoubleSpinBox *doubleSpinBox;
    QLabel *label;
    QTableView *monitorTableView;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *LogWindow)
    {
        if (LogWindow->objectName().isEmpty())
            LogWindow->setObjectName(QString::fromUtf8("LogWindow"));
        LogWindow->resize(800, 600);
        centralwidget = new QWidget(LogWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_3 = new QGridLayout(centralwidget);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setDocumentMode(true);
        tabWidget->setTabsClosable(false);
        tabWidget->setMovable(false);
        tabWidget->setTabBarAutoHide(false);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        gridLayout = new QGridLayout(tab);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        clearButton = new QPushButton(tab);
        clearButton->setObjectName(QString::fromUtf8("clearButton"));

        horizontalLayout->addWidget(clearButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        exportButton = new QPushButton(tab);
        exportButton->setObjectName(QString::fromUtf8("exportButton"));

        horizontalLayout->addWidget(exportButton);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        m_realTableView = new QTableView(tab);
        m_realTableView->setObjectName(QString::fromUtf8("m_realTableView"));

        gridLayout->addWidget(m_realTableView, 1, 0, 1, 1);

        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/realMonitor.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabWidget->addTab(tab, icon, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        gridLayout_2 = new QGridLayout(tab_3);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        appendButton = new QPushButton(tab_3);
        appendButton->setObjectName(QString::fromUtf8("appendButton"));

        gridLayout_2->addWidget(appendButton, 0, 1, 1, 1);

        insertButton = new QPushButton(tab_3);
        insertButton->setObjectName(QString::fromUtf8("insertButton"));

        gridLayout_2->addWidget(insertButton, 1, 1, 1, 1);

        deleteButton = new QPushButton(tab_3);
        deleteButton->setObjectName(QString::fromUtf8("deleteButton"));

        gridLayout_2->addWidget(deleteButton, 2, 1, 1, 1);

        updateButton = new QPushButton(tab_3);
        updateButton->setObjectName(QString::fromUtf8("updateButton"));

        gridLayout_2->addWidget(updateButton, 3, 1, 1, 1);

        upButton = new QPushButton(tab_3);
        upButton->setObjectName(QString::fromUtf8("upButton"));

        gridLayout_2->addWidget(upButton, 4, 1, 1, 1);

        downButton = new QPushButton(tab_3);
        downButton->setObjectName(QString::fromUtf8("downButton"));

        gridLayout_2->addWidget(downButton, 5, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 253, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 6, 1, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        doubleSpinBox = new QDoubleSpinBox(tab_3);
        doubleSpinBox->setObjectName(QString::fromUtf8("doubleSpinBox"));
        doubleSpinBox->setValue(1.000000000000000);

        horizontalLayout_2->addWidget(doubleSpinBox);

        label = new QLabel(tab_3);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_2->addWidget(label);


        gridLayout_2->addLayout(horizontalLayout_2, 7, 1, 1, 1);

        monitorTableView = new QTableView(tab_3);
        monitorTableView->setObjectName(QString::fromUtf8("monitorTableView"));

        gridLayout_2->addWidget(monitorTableView, 0, 0, 8, 1);

        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/monitorList.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabWidget->addTab(tab_3, icon1, QString());

        gridLayout_3->addWidget(tabWidget, 0, 1, 1, 1);

        LogWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(LogWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 28));
        LogWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(LogWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        LogWindow->setStatusBar(statusbar);

        retranslateUi(LogWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(LogWindow);
    } // setupUi

    void retranslateUi(QMainWindow *LogWindow)
    {
        LogWindow->setWindowTitle(QApplication::translate("LogWindow", "MainWindow", nullptr));
        clearButton->setText(QApplication::translate("LogWindow", "\346\270\205\345\261\217", nullptr));
        exportButton->setText(QApplication::translate("LogWindow", "\345\257\274\345\207\272", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("LogWindow", "USN\346\227\245\345\277\227\345\256\236\346\227\266\347\233\221\346\216\247", nullptr));
        appendButton->setText(QApplication::translate("LogWindow", "\346\267\273\345\212\240", nullptr));
        insertButton->setText(QApplication::translate("LogWindow", "\346\217\222\345\205\245", nullptr));
        deleteButton->setText(QApplication::translate("LogWindow", "\345\210\240\351\231\244", nullptr));
        updateButton->setText(QApplication::translate("LogWindow", "\344\277\256\346\224\271", nullptr));
        upButton->setText(QApplication::translate("LogWindow", "\344\270\212\347\247\273", nullptr));
        downButton->setText(QApplication::translate("LogWindow", "\344\270\213\347\247\273", nullptr));
        label->setText(QApplication::translate("LogWindow", "\347\247\222", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("LogWindow", "USN\346\227\245\345\277\227\347\233\221\346\216\247\345\210\227\350\241\250", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LogWindow: public Ui_LogWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGWINDOW_H
