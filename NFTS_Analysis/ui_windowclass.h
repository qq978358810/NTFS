/********************************************************************************
** Form generated from reading UI file 'windowclass.ui'
**
** Created by: Qt User Interface Compiler version 5.12.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WINDOWCLASS_H
#define UI_WINDOWCLASS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WindowClass
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *WindowClass)
    {
        if (WindowClass->objectName().isEmpty())
            WindowClass->setObjectName(QString::fromUtf8("WindowClass"));
        WindowClass->resize(697, 498);
        centralwidget = new QWidget(WindowClass);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        WindowClass->setCentralWidget(centralwidget);
        menubar = new QMenuBar(WindowClass);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 697, 25));
        WindowClass->setMenuBar(menubar);
        statusbar = new QStatusBar(WindowClass);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        WindowClass->setStatusBar(statusbar);
        toolBar = new QToolBar(WindowClass);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        WindowClass->addToolBar(Qt::TopToolBarArea, toolBar);

        retranslateUi(WindowClass);

        QMetaObject::connectSlotsByName(WindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *WindowClass)
    {
        WindowClass->setWindowTitle(QApplication::translate("WindowClass", "MainWindow", nullptr));
        toolBar->setWindowTitle(QApplication::translate("WindowClass", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class WindowClass: public Ui_WindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WINDOWCLASS_H
