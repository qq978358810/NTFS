/********************************************************************************
** Form generated from reading UI file 'usnmonitordialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USNMONITORDIALOG_H
#define UI_USNMONITORDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_UsnMonitorDialog
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *partitonComboBox;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *mftLineEdit_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QLineEdit *pMftLineEdit_3;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QLineEdit *reasonLineEdit_4;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_5;
    QLineEdit *attrLineEdit_5;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_6;
    QLineEdit *fileNameLineEdit_6;
    QHBoxLayout *horizontalLayout_14;
    QCheckBox *CaseInsensitiveCheckBox;
    QCheckBox *completeCheckBox;
    QCheckBox *RegExpCheckBox;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_7;
    QTextEdit *commandTextEdit;
    QCheckBox *execCheckBox;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *UsnMonitorDialog)
    {
        if (UsnMonitorDialog->objectName().isEmpty())
            UsnMonitorDialog->setObjectName(QString::fromUtf8("UsnMonitorDialog"));
        UsnMonitorDialog->resize(409, 466);
        gridLayout = new QGridLayout(UsnMonitorDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(UsnMonitorDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMinimumSize(QSize(100, 0));
        label->setMaximumSize(QSize(100, 16777215));

        horizontalLayout->addWidget(label);

        partitonComboBox = new QComboBox(UsnMonitorDialog);
        partitonComboBox->setObjectName(QString::fromUtf8("partitonComboBox"));

        horizontalLayout->addWidget(partitonComboBox);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(UsnMonitorDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setMinimumSize(QSize(100, 0));
        label_2->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_2->addWidget(label_2);

        mftLineEdit_2 = new QLineEdit(UsnMonitorDialog);
        mftLineEdit_2->setObjectName(QString::fromUtf8("mftLineEdit_2"));

        horizontalLayout_2->addWidget(mftLineEdit_2);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_3 = new QLabel(UsnMonitorDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setMinimumSize(QSize(100, 0));
        label_3->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_3->addWidget(label_3);

        pMftLineEdit_3 = new QLineEdit(UsnMonitorDialog);
        pMftLineEdit_3->setObjectName(QString::fromUtf8("pMftLineEdit_3"));

        horizontalLayout_3->addWidget(pMftLineEdit_3);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_4 = new QLabel(UsnMonitorDialog);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setMinimumSize(QSize(100, 0));
        label_4->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_4->addWidget(label_4);

        reasonLineEdit_4 = new QLineEdit(UsnMonitorDialog);
        reasonLineEdit_4->setObjectName(QString::fromUtf8("reasonLineEdit_4"));

        horizontalLayout_4->addWidget(reasonLineEdit_4);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_5 = new QLabel(UsnMonitorDialog);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setMinimumSize(QSize(100, 0));
        label_5->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_5->addWidget(label_5);

        attrLineEdit_5 = new QLineEdit(UsnMonitorDialog);
        attrLineEdit_5->setObjectName(QString::fromUtf8("attrLineEdit_5"));

        horizontalLayout_5->addWidget(attrLineEdit_5);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_6 = new QLabel(UsnMonitorDialog);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setMinimumSize(QSize(100, 0));
        label_6->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_6->addWidget(label_6);

        fileNameLineEdit_6 = new QLineEdit(UsnMonitorDialog);
        fileNameLineEdit_6->setObjectName(QString::fromUtf8("fileNameLineEdit_6"));

        horizontalLayout_6->addWidget(fileNameLineEdit_6);


        verticalLayout->addLayout(horizontalLayout_6);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);

        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        CaseInsensitiveCheckBox = new QCheckBox(UsnMonitorDialog);
        CaseInsensitiveCheckBox->setObjectName(QString::fromUtf8("CaseInsensitiveCheckBox"));
        CaseInsensitiveCheckBox->setMaximumSize(QSize(200, 16777215));

        horizontalLayout_14->addWidget(CaseInsensitiveCheckBox);

        completeCheckBox = new QCheckBox(UsnMonitorDialog);
        completeCheckBox->setObjectName(QString::fromUtf8("completeCheckBox"));
        completeCheckBox->setMaximumSize(QSize(200, 16777215));

        horizontalLayout_14->addWidget(completeCheckBox);

        RegExpCheckBox = new QCheckBox(UsnMonitorDialog);
        RegExpCheckBox->setObjectName(QString::fromUtf8("RegExpCheckBox"));
        RegExpCheckBox->setMaximumSize(QSize(200, 16777215));

        horizontalLayout_14->addWidget(RegExpCheckBox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_14->addItem(horizontalSpacer);


        gridLayout->addLayout(horizontalLayout_14, 1, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 169, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 2, 0, 1, 1);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        label_7 = new QLabel(UsnMonitorDialog);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setMinimumSize(QSize(100, 0));
        label_7->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_7->addWidget(label_7);

        commandTextEdit = new QTextEdit(UsnMonitorDialog);
        commandTextEdit->setObjectName(QString::fromUtf8("commandTextEdit"));

        horizontalLayout_7->addWidget(commandTextEdit);

        execCheckBox = new QCheckBox(UsnMonitorDialog);
        execCheckBox->setObjectName(QString::fromUtf8("execCheckBox"));

        horizontalLayout_7->addWidget(execCheckBox);


        gridLayout->addLayout(horizontalLayout_7, 3, 0, 1, 1);

        buttonBox = new QDialogButtonBox(UsnMonitorDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 4, 0, 1, 1);


        retranslateUi(UsnMonitorDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), UsnMonitorDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), UsnMonitorDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(UsnMonitorDialog);
    } // setupUi

    void retranslateUi(QDialog *UsnMonitorDialog)
    {
        UsnMonitorDialog->setWindowTitle(QApplication::translate("UsnMonitorDialog", "Dialog", nullptr));
        label->setText(QApplication::translate("UsnMonitorDialog", "\347\233\230\347\254\246:", nullptr));
        label_2->setText(QApplication::translate("UsnMonitorDialog", "MFT:", nullptr));
        label_3->setText(QApplication::translate("UsnMonitorDialog", "PMFT:", nullptr));
        label_4->setText(QApplication::translate("UsnMonitorDialog", "\346\226\207\344\273\266\345\217\230\345\214\226\345\216\237\345\233\240:", nullptr));
        label_5->setText(QApplication::translate("UsnMonitorDialog", "\346\226\207\344\273\266\345\261\236\346\200\247:", nullptr));
        label_6->setText(QApplication::translate("UsnMonitorDialog", "\346\226\207\344\273\266\345\220\215:", nullptr));
        CaseInsensitiveCheckBox->setText(QApplication::translate("UsnMonitorDialog", "\345\214\272\345\210\206\345\244\247\345\260\217\345\206\231", nullptr));
        completeCheckBox->setText(QApplication::translate("UsnMonitorDialog", "\345\256\214\345\205\250\345\214\271\351\205\215", nullptr));
        RegExpCheckBox->setText(QApplication::translate("UsnMonitorDialog", "\344\275\277\347\224\250\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217", nullptr));
        label_7->setText(QApplication::translate("UsnMonitorDialog", "Windows\350\277\220\350\241\214:", nullptr));
        execCheckBox->setText(QApplication::translate("UsnMonitorDialog", "\345\220\257\347\224\250", nullptr));
    } // retranslateUi

};

namespace Ui {
    class UsnMonitorDialog: public Ui_UsnMonitorDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USNMONITORDIALOG_H
