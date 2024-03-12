/********************************************************************************
** Form generated from reading UI file 'logitemdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGITEMDIALOG_H
#define UI_LOGITEMDIALOG_H

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

QT_BEGIN_NAMESPACE

class Ui_LogItemDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout_2;
    QLabel *partitionLable;
    QComboBox *partitionComboBox;
    QLabel *PMftLableIcon;
    QHBoxLayout *horizontalLayout_3;
    QLabel *MftNumbeLable;
    QLineEdit *MftNumbeLineEdit;
    QHBoxLayout *horizontalLayout;
    QLabel *PMftNumbeLable;
    QLineEdit *PMftNumbeLineEdit;
    QHBoxLayout *horizontalLayout_6;
    QLabel *FileNameLable;
    QLineEdit *FileNameLineEdit;
    QHBoxLayout *horizontalLayout_14;
    QCheckBox *CaseInsensitiveCheckBox;
    QCheckBox *RegExpCheckBox;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *verticalSpacer;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout_4;
    QLabel *FileNameLable_2;
    QLineEdit *CMDCommandlineEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *LogItemDialog)
    {
        if (LogItemDialog->objectName().isEmpty())
            LogItemDialog->setObjectName(QString::fromUtf8("LogItemDialog"));
        LogItemDialog->resize(592, 445);
        LogItemDialog->setMaximumSize(QSize(1000, 500));
        gridLayout = new QGridLayout(LogItemDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(LogItemDialog);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        partitionLable = new QLabel(LogItemDialog);
        partitionLable->setObjectName(QString::fromUtf8("partitionLable"));
        partitionLable->setEnabled(true);
        partitionLable->setMinimumSize(QSize(60, 30));
        partitionLable->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(partitionLable);

        partitionComboBox = new QComboBox(LogItemDialog);
        partitionComboBox->setObjectName(QString::fromUtf8("partitionComboBox"));
        partitionComboBox->setMinimumSize(QSize(100, 0));

        horizontalLayout_2->addWidget(partitionComboBox);


        gridLayout->addLayout(horizontalLayout_2, 1, 0, 1, 2);

        PMftLableIcon = new QLabel(LogItemDialog);
        PMftLableIcon->setObjectName(QString::fromUtf8("PMftLableIcon"));
        PMftLableIcon->setMaximumSize(QSize(30, 30));

        gridLayout->addWidget(PMftLableIcon, 1, 2, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setSizeConstraint(QLayout::SetDefaultConstraint);
        MftNumbeLable = new QLabel(LogItemDialog);
        MftNumbeLable->setObjectName(QString::fromUtf8("MftNumbeLable"));
        MftNumbeLable->setMinimumSize(QSize(100, 30));

        horizontalLayout_3->addWidget(MftNumbeLable);

        MftNumbeLineEdit = new QLineEdit(LogItemDialog);
        MftNumbeLineEdit->setObjectName(QString::fromUtf8("MftNumbeLineEdit"));

        horizontalLayout_3->addWidget(MftNumbeLineEdit);


        gridLayout->addLayout(horizontalLayout_3, 2, 0, 1, 4);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        PMftNumbeLable = new QLabel(LogItemDialog);
        PMftNumbeLable->setObjectName(QString::fromUtf8("PMftNumbeLable"));
        PMftNumbeLable->setMinimumSize(QSize(100, 30));

        horizontalLayout->addWidget(PMftNumbeLable);

        PMftNumbeLineEdit = new QLineEdit(LogItemDialog);
        PMftNumbeLineEdit->setObjectName(QString::fromUtf8("PMftNumbeLineEdit"));

        horizontalLayout->addWidget(PMftNumbeLineEdit);


        gridLayout->addLayout(horizontalLayout, 3, 0, 1, 4);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        FileNameLable = new QLabel(LogItemDialog);
        FileNameLable->setObjectName(QString::fromUtf8("FileNameLable"));
        FileNameLable->setMinimumSize(QSize(100, 30));

        horizontalLayout_6->addWidget(FileNameLable);

        FileNameLineEdit = new QLineEdit(LogItemDialog);
        FileNameLineEdit->setObjectName(QString::fromUtf8("FileNameLineEdit"));

        horizontalLayout_6->addWidget(FileNameLineEdit);


        gridLayout->addLayout(horizontalLayout_6, 4, 0, 1, 4);

        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        CaseInsensitiveCheckBox = new QCheckBox(LogItemDialog);
        CaseInsensitiveCheckBox->setObjectName(QString::fromUtf8("CaseInsensitiveCheckBox"));
        CaseInsensitiveCheckBox->setMaximumSize(QSize(200, 16777215));

        horizontalLayout_14->addWidget(CaseInsensitiveCheckBox);

        RegExpCheckBox = new QCheckBox(LogItemDialog);
        RegExpCheckBox->setObjectName(QString::fromUtf8("RegExpCheckBox"));
        RegExpCheckBox->setMaximumSize(QSize(200, 16777215));

        horizontalLayout_14->addWidget(RegExpCheckBox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_14->addItem(horizontalSpacer);


        gridLayout->addLayout(horizontalLayout_14, 5, 0, 1, 4);

        verticalSpacer = new QSpacerItem(20, 120, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 6, 1, 1, 1);

        label_2 = new QLabel(LogItemDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 7, 0, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        FileNameLable_2 = new QLabel(LogItemDialog);
        FileNameLable_2->setObjectName(QString::fromUtf8("FileNameLable_2"));
        FileNameLable_2->setMinimumSize(QSize(100, 30));

        horizontalLayout_4->addWidget(FileNameLable_2);

        CMDCommandlineEdit = new QLineEdit(LogItemDialog);
        CMDCommandlineEdit->setObjectName(QString::fromUtf8("CMDCommandlineEdit"));

        horizontalLayout_4->addWidget(CMDCommandlineEdit);


        gridLayout->addLayout(horizontalLayout_4, 8, 0, 1, 4);

        buttonBox = new QDialogButtonBox(LogItemDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 9, 3, 1, 1);


        retranslateUi(LogItemDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), LogItemDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), LogItemDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(LogItemDialog);
    } // setupUi

    void retranslateUi(QDialog *LogItemDialog)
    {
        LogItemDialog->setWindowTitle(QApplication::translate("LogItemDialog", "Dialog", nullptr));
        label->setText(QApplication::translate("LogItemDialog", "\346\235\241\344\273\266", nullptr));
        partitionLable->setText(QApplication::translate("LogItemDialog", "\347\233\230\347\254\246:", nullptr));
        PMftLableIcon->setText(QString());
        MftNumbeLable->setText(QApplication::translate("LogItemDialog", "MFT:    ", nullptr));
        PMftNumbeLable->setText(QApplication::translate("LogItemDialog", "PMFT:   ", nullptr));
        FileNameLable->setText(QApplication::translate("LogItemDialog", "\346\226\207\344\273\266\345\220\215:  ", nullptr));
        CaseInsensitiveCheckBox->setText(QApplication::translate("LogItemDialog", "\345\214\272\345\210\206\345\244\247\345\260\217\345\206\231", nullptr));
        RegExpCheckBox->setText(QApplication::translate("LogItemDialog", "\344\275\277\347\224\250\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217", nullptr));
        label_2->setText(QApplication::translate("LogItemDialog", "\346\211\247\350\241\214", nullptr));
        FileNameLable_2->setText(QApplication::translate("LogItemDialog", "CMD\346\214\207\344\273\244:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LogItemDialog: public Ui_LogItemDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGITEMDIALOG_H
