/********************************************************************************
** Form generated from reading UI file 'settingdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGDIALOG_H
#define UI_SETTINGDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <mycheckboxcombo.h>

QT_BEGIN_NAMESPACE

class Ui_SettingDialog
{
public:
    QGridLayout *gridLayout;
    QCheckBox *AppAutoRuncheckBox;
    QCheckBox *SystemTrayCheckBox;
    QCheckBox *real_timeSearchCheckBox;
    QCheckBox *hideSearchButton;
    QCheckBox *enableDelFileFlagCheckBox;
    QCheckBox *dataBaseLoadCheckBox;
    QCheckBox *windowTopCheckBox;
    QCheckBox *usnRecordNoMonitoringCheckBox;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    XComboBox *columnsComboBox;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pushButton;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SettingDialog)
    {
        if (SettingDialog->objectName().isEmpty())
            SettingDialog->setObjectName(QString::fromUtf8("SettingDialog"));
        SettingDialog->resize(262, 465);
        SettingDialog->setMaximumSize(QSize(1500, 500));
        gridLayout = new QGridLayout(SettingDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        AppAutoRuncheckBox = new QCheckBox(SettingDialog);
        AppAutoRuncheckBox->setObjectName(QString::fromUtf8("AppAutoRuncheckBox"));

        gridLayout->addWidget(AppAutoRuncheckBox, 0, 0, 1, 1);

        SystemTrayCheckBox = new QCheckBox(SettingDialog);
        SystemTrayCheckBox->setObjectName(QString::fromUtf8("SystemTrayCheckBox"));

        gridLayout->addWidget(SystemTrayCheckBox, 1, 0, 1, 1);

        real_timeSearchCheckBox = new QCheckBox(SettingDialog);
        real_timeSearchCheckBox->setObjectName(QString::fromUtf8("real_timeSearchCheckBox"));

        gridLayout->addWidget(real_timeSearchCheckBox, 2, 0, 1, 1);

        hideSearchButton = new QCheckBox(SettingDialog);
        hideSearchButton->setObjectName(QString::fromUtf8("hideSearchButton"));

        gridLayout->addWidget(hideSearchButton, 3, 0, 1, 1);

        enableDelFileFlagCheckBox = new QCheckBox(SettingDialog);
        enableDelFileFlagCheckBox->setObjectName(QString::fromUtf8("enableDelFileFlagCheckBox"));

        gridLayout->addWidget(enableDelFileFlagCheckBox, 4, 0, 1, 2);

        dataBaseLoadCheckBox = new QCheckBox(SettingDialog);
        dataBaseLoadCheckBox->setObjectName(QString::fromUtf8("dataBaseLoadCheckBox"));

        gridLayout->addWidget(dataBaseLoadCheckBox, 5, 0, 1, 1);

        windowTopCheckBox = new QCheckBox(SettingDialog);
        windowTopCheckBox->setObjectName(QString::fromUtf8("windowTopCheckBox"));

        gridLayout->addWidget(windowTopCheckBox, 6, 0, 1, 1);

        usnRecordNoMonitoringCheckBox = new QCheckBox(SettingDialog);
        usnRecordNoMonitoringCheckBox->setObjectName(QString::fromUtf8("usnRecordNoMonitoringCheckBox"));

        gridLayout->addWidget(usnRecordNoMonitoringCheckBox, 7, 0, 1, 2);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(SettingDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMaximumSize(QSize(68, 16777215));

        horizontalLayout_2->addWidget(label);

        columnsComboBox = new XComboBox(SettingDialog);
        columnsComboBox->setObjectName(QString::fromUtf8("columnsComboBox"));
        columnsComboBox->setMinimumSize(QSize(100, 0));

        horizontalLayout_2->addWidget(columnsComboBox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Ignored, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        gridLayout->addLayout(horizontalLayout_2, 8, 0, 1, 2);

        verticalSpacer = new QSpacerItem(20, 241, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 9, 1, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        pushButton = new QPushButton(SettingDialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setMinimumSize(QSize(100, 0));
        pushButton->setMaximumSize(QSize(100, 16777215));

        horizontalLayout->addWidget(pushButton);


        gridLayout->addLayout(horizontalLayout, 10, 0, 1, 1);

        buttonBox = new QDialogButtonBox(SettingDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 11, 0, 1, 1);


        retranslateUi(SettingDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SettingDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SettingDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(SettingDialog);
    } // setupUi

    void retranslateUi(QDialog *SettingDialog)
    {
        SettingDialog->setWindowTitle(QApplication::translate("SettingDialog", "Dialog", nullptr));
        AppAutoRuncheckBox->setText(QApplication::translate("SettingDialog", "\345\274\200\346\234\272\350\207\252\345\220\257\345\212\250", nullptr));
        SystemTrayCheckBox->setText(QApplication::translate("SettingDialog", "\346\230\276\347\244\272\346\211\230\347\233\230\345\233\276\346\240\207", nullptr));
        real_timeSearchCheckBox->setText(QApplication::translate("SettingDialog", "\345\256\236\346\227\266\346\220\234\347\264\242", nullptr));
        hideSearchButton->setText(QApplication::translate("SettingDialog", "\351\232\220\350\227\217\346\220\234\347\264\242\346\214\211\351\222\256", nullptr));
        enableDelFileFlagCheckBox->setText(QApplication::translate("SettingDialog", "\346\255\243\345\270\270\346\226\207\344\273\266\345\220\257\347\224\250(\346\201\242\345\244\215/\345\244\215\345\210\266\346\225\260\346\215\256)", nullptr));
        dataBaseLoadCheckBox->setText(QApplication::translate("SettingDialog", "\346\225\260\346\215\256\344\277\235\345\255\230\345\210\260\346\225\260\346\215\256\345\272\223", nullptr));
        windowTopCheckBox->setText(QApplication::translate("SettingDialog", "\347\252\227\345\217\243\347\275\256\351\241\266", nullptr));
        usnRecordNoMonitoringCheckBox->setText(QApplication::translate("SettingDialog", "UsnJournal\344\270\215\347\233\221\346\216\247(\344\270\215\346\233\264\346\226\260\346\225\260\346\215\256)", nullptr));
        label->setText(QApplication::translate("SettingDialog", "\351\232\220\350\227\217\346\237\220\345\210\227:", nullptr));
        pushButton->setText(QApplication::translate("SettingDialog", "\346\201\242\345\244\215\345\210\260\351\273\230\350\256\244", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingDialog: public Ui_SettingDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGDIALOG_H
