/********************************************************************************
** Form generated from reading UI file 'highfinddialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HIGHFINDDIALOG_H
#define UI_HIGHFINDDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_HighFindDialog
{
public:
    QGridLayout *gridLayout;
    QDialogButtonBox *buttonBox;
    QHBoxLayout *horizontalLayout_8;
    QLabel *leftFileRealSizeLable;
    QLineEdit *leftFileRealSizeLine;
    QComboBox *leftFileRealSizeComboBox;
    QLabel *rightFileRealSizeLable;
    QLineEdit *rightFileRealSizeLine;
    QComboBox *rightFileRealSizeComboBox;
    QHBoxLayout *horizontalLayout_9;
    QLabel *leftFileCreationTimeLable;
    QCheckBox *leftFileCreationTimeCheckBox;
    QDateEdit *leftFileCreationTimeDateEdit;
    QLabel *rightFileCreationTimeLable;
    QCheckBox *rightFileCreationTimeCheckBox;
    QDateEdit *rightFileCreationTimeDateEdit;
    QHBoxLayout *horizontalLayout_10;
    QLabel *leftFileChangeTimeLable;
    QCheckBox *leftFileChangeTimeCheckBox;
    QDateEdit *leftFileChangeTimeDateEdit;
    QLabel *rightFileChangeTimeLable;
    QCheckBox *rightFileChangeTimeCheckBox;
    QDateEdit *rightFileChangeTimeDateEdit;
    QHBoxLayout *horizontalLayout_3;
    QLabel *MftNumbeLable;
    QLabel *MftLableIcon;
    QLineEdit *MftNumbeLineEdit;
    QHBoxLayout *horizontalLayout_11;
    QLabel *leftFileReadTimeLable;
    QCheckBox *leftFileReadTimeCheckBox;
    QDateEdit *leftFileReadTimeDateEdit;
    QLabel *rightFileReadTimeLable;
    QCheckBox *rightFileReadTimeCheckBox;
    QDateEdit *rightFileReadTimeDateEdit;
    QHBoxLayout *horizontalLayout_12;
    QHBoxLayout *horizontalLayout_2;
    QLabel *partitionLable;
    QComboBox *partitionComboBox;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout;
    QLabel *fileFlagLable;
    QComboBox *fileFlagComboBox;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_15;
    QLabel *extendLable;
    QComboBox *extendComboBox;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *resetPushButton;
    QHBoxLayout *horizontalLayout_14;
    QCheckBox *CaseInsensitiveCheckBox;
    QCheckBox *completeCheckBox;
    QCheckBox *RegExpCheckBox;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_4;
    QLabel *PMftNumbeLable;
    QLabel *PMftLableIcon;
    QLineEdit *PMftNumbeLineEdit;
    QHBoxLayout *horizontalLayout_6;
    QLabel *FileNameLable;
    QLineEdit *FileNameLineEdit;
    QHBoxLayout *horizontalLayout_5;
    QLabel *FileAttributeLable;
    QLineEdit *FileAttributeLine;
    QComboBox *FileAttributeLineComboBox;

    void setupUi(QDialog *HighFindDialog)
    {
        if (HighFindDialog->objectName().isEmpty())
            HighFindDialog->setObjectName(QString::fromUtf8("HighFindDialog"));
        HighFindDialog->resize(700, 550);
        HighFindDialog->setMinimumSize(QSize(700, 550));
        gridLayout = new QGridLayout(HighFindDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        buttonBox = new QDialogButtonBox(HighFindDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 11, 0, 1, 2);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        leftFileRealSizeLable = new QLabel(HighFindDialog);
        leftFileRealSizeLable->setObjectName(QString::fromUtf8("leftFileRealSizeLable"));
        leftFileRealSizeLable->setMinimumSize(QSize(150, 30));
        leftFileRealSizeLable->setMaximumSize(QSize(150, 16777215));

        horizontalLayout_8->addWidget(leftFileRealSizeLable);

        leftFileRealSizeLine = new QLineEdit(HighFindDialog);
        leftFileRealSizeLine->setObjectName(QString::fromUtf8("leftFileRealSizeLine"));

        horizontalLayout_8->addWidget(leftFileRealSizeLine);

        leftFileRealSizeComboBox = new QComboBox(HighFindDialog);
        leftFileRealSizeComboBox->addItem(QString());
        leftFileRealSizeComboBox->addItem(QString());
        leftFileRealSizeComboBox->addItem(QString());
        leftFileRealSizeComboBox->addItem(QString());
        leftFileRealSizeComboBox->setObjectName(QString::fromUtf8("leftFileRealSizeComboBox"));
        leftFileRealSizeComboBox->setMaximumSize(QSize(70, 16777215));

        horizontalLayout_8->addWidget(leftFileRealSizeComboBox);

        rightFileRealSizeLable = new QLabel(HighFindDialog);
        rightFileRealSizeLable->setObjectName(QString::fromUtf8("rightFileRealSizeLable"));
        rightFileRealSizeLable->setMinimumSize(QSize(50, 0));
        rightFileRealSizeLable->setMaximumSize(QSize(50, 16777215));
        rightFileRealSizeLable->setLayoutDirection(Qt::LeftToRight);
        rightFileRealSizeLable->setAlignment(Qt::AlignCenter);

        horizontalLayout_8->addWidget(rightFileRealSizeLable);

        rightFileRealSizeLine = new QLineEdit(HighFindDialog);
        rightFileRealSizeLine->setObjectName(QString::fromUtf8("rightFileRealSizeLine"));

        horizontalLayout_8->addWidget(rightFileRealSizeLine);

        rightFileRealSizeComboBox = new QComboBox(HighFindDialog);
        rightFileRealSizeComboBox->addItem(QString());
        rightFileRealSizeComboBox->addItem(QString());
        rightFileRealSizeComboBox->addItem(QString());
        rightFileRealSizeComboBox->addItem(QString());
        rightFileRealSizeComboBox->setObjectName(QString::fromUtf8("rightFileRealSizeComboBox"));
        rightFileRealSizeComboBox->setMaximumSize(QSize(70, 16777215));

        horizontalLayout_8->addWidget(rightFileRealSizeComboBox);


        gridLayout->addLayout(horizontalLayout_8, 7, 0, 1, 2);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        leftFileCreationTimeLable = new QLabel(HighFindDialog);
        leftFileCreationTimeLable->setObjectName(QString::fromUtf8("leftFileCreationTimeLable"));
        leftFileCreationTimeLable->setMinimumSize(QSize(0, 30));
        leftFileCreationTimeLable->setMaximumSize(QSize(150, 16777215));

        horizontalLayout_9->addWidget(leftFileCreationTimeLable);

        leftFileCreationTimeCheckBox = new QCheckBox(HighFindDialog);
        leftFileCreationTimeCheckBox->setObjectName(QString::fromUtf8("leftFileCreationTimeCheckBox"));
        leftFileCreationTimeCheckBox->setMaximumSize(QSize(30, 30));
        leftFileCreationTimeCheckBox->setMouseTracking(true);
        leftFileCreationTimeCheckBox->setLayoutDirection(Qt::RightToLeft);
        leftFileCreationTimeCheckBox->setAutoFillBackground(false);
        leftFileCreationTimeCheckBox->setTristate(false);

        horizontalLayout_9->addWidget(leftFileCreationTimeCheckBox);

        leftFileCreationTimeDateEdit = new QDateEdit(HighFindDialog);
        leftFileCreationTimeDateEdit->setObjectName(QString::fromUtf8("leftFileCreationTimeDateEdit"));
        leftFileCreationTimeDateEdit->setWrapping(false);
        leftFileCreationTimeDateEdit->setReadOnly(true);
        leftFileCreationTimeDateEdit->setAccelerated(false);
        leftFileCreationTimeDateEdit->setCalendarPopup(true);

        horizontalLayout_9->addWidget(leftFileCreationTimeDateEdit);

        rightFileCreationTimeLable = new QLabel(HighFindDialog);
        rightFileCreationTimeLable->setObjectName(QString::fromUtf8("rightFileCreationTimeLable"));
        rightFileCreationTimeLable->setMaximumSize(QSize(50, 16777215));
        rightFileCreationTimeLable->setLayoutDirection(Qt::LeftToRight);
        rightFileCreationTimeLable->setAlignment(Qt::AlignCenter);

        horizontalLayout_9->addWidget(rightFileCreationTimeLable);

        rightFileCreationTimeCheckBox = new QCheckBox(HighFindDialog);
        rightFileCreationTimeCheckBox->setObjectName(QString::fromUtf8("rightFileCreationTimeCheckBox"));
        rightFileCreationTimeCheckBox->setMaximumSize(QSize(30, 30));
        rightFileCreationTimeCheckBox->setLayoutDirection(Qt::RightToLeft);
        rightFileCreationTimeCheckBox->setTristate(false);

        horizontalLayout_9->addWidget(rightFileCreationTimeCheckBox);

        rightFileCreationTimeDateEdit = new QDateEdit(HighFindDialog);
        rightFileCreationTimeDateEdit->setObjectName(QString::fromUtf8("rightFileCreationTimeDateEdit"));
        rightFileCreationTimeDateEdit->setReadOnly(true);
        rightFileCreationTimeDateEdit->setCurrentSection(QDateTimeEdit::YearSection);
        rightFileCreationTimeDateEdit->setCalendarPopup(true);

        horizontalLayout_9->addWidget(rightFileCreationTimeDateEdit);


        gridLayout->addLayout(horizontalLayout_9, 8, 0, 1, 2);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        leftFileChangeTimeLable = new QLabel(HighFindDialog);
        leftFileChangeTimeLable->setObjectName(QString::fromUtf8("leftFileChangeTimeLable"));
        leftFileChangeTimeLable->setMinimumSize(QSize(0, 30));
        leftFileChangeTimeLable->setMaximumSize(QSize(150, 16777215));

        horizontalLayout_10->addWidget(leftFileChangeTimeLable);

        leftFileChangeTimeCheckBox = new QCheckBox(HighFindDialog);
        leftFileChangeTimeCheckBox->setObjectName(QString::fromUtf8("leftFileChangeTimeCheckBox"));
        leftFileChangeTimeCheckBox->setMaximumSize(QSize(30, 30));
        leftFileChangeTimeCheckBox->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_10->addWidget(leftFileChangeTimeCheckBox);

        leftFileChangeTimeDateEdit = new QDateEdit(HighFindDialog);
        leftFileChangeTimeDateEdit->setObjectName(QString::fromUtf8("leftFileChangeTimeDateEdit"));
        leftFileChangeTimeDateEdit->setWrapping(false);
        leftFileChangeTimeDateEdit->setReadOnly(true);
        leftFileChangeTimeDateEdit->setCalendarPopup(true);

        horizontalLayout_10->addWidget(leftFileChangeTimeDateEdit);

        rightFileChangeTimeLable = new QLabel(HighFindDialog);
        rightFileChangeTimeLable->setObjectName(QString::fromUtf8("rightFileChangeTimeLable"));
        rightFileChangeTimeLable->setMaximumSize(QSize(50, 16777215));
        rightFileChangeTimeLable->setLayoutDirection(Qt::LeftToRight);
        rightFileChangeTimeLable->setAlignment(Qt::AlignCenter);

        horizontalLayout_10->addWidget(rightFileChangeTimeLable);

        rightFileChangeTimeCheckBox = new QCheckBox(HighFindDialog);
        rightFileChangeTimeCheckBox->setObjectName(QString::fromUtf8("rightFileChangeTimeCheckBox"));
        rightFileChangeTimeCheckBox->setMaximumSize(QSize(30, 30));
        rightFileChangeTimeCheckBox->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_10->addWidget(rightFileChangeTimeCheckBox);

        rightFileChangeTimeDateEdit = new QDateEdit(HighFindDialog);
        rightFileChangeTimeDateEdit->setObjectName(QString::fromUtf8("rightFileChangeTimeDateEdit"));
        rightFileChangeTimeDateEdit->setReadOnly(true);
        rightFileChangeTimeDateEdit->setCalendarPopup(true);

        horizontalLayout_10->addWidget(rightFileChangeTimeDateEdit);


        gridLayout->addLayout(horizontalLayout_10, 9, 0, 1, 2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setSizeConstraint(QLayout::SetDefaultConstraint);
        MftNumbeLable = new QLabel(HighFindDialog);
        MftNumbeLable->setObjectName(QString::fromUtf8("MftNumbeLable"));
        MftNumbeLable->setMinimumSize(QSize(100, 30));

        horizontalLayout_3->addWidget(MftNumbeLable);

        MftLableIcon = new QLabel(HighFindDialog);
        MftLableIcon->setObjectName(QString::fromUtf8("MftLableIcon"));
        MftLableIcon->setMaximumSize(QSize(30, 30));

        horizontalLayout_3->addWidget(MftLableIcon);

        MftNumbeLineEdit = new QLineEdit(HighFindDialog);
        MftNumbeLineEdit->setObjectName(QString::fromUtf8("MftNumbeLineEdit"));

        horizontalLayout_3->addWidget(MftNumbeLineEdit);


        gridLayout->addLayout(horizontalLayout_3, 2, 0, 1, 2);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        leftFileReadTimeLable = new QLabel(HighFindDialog);
        leftFileReadTimeLable->setObjectName(QString::fromUtf8("leftFileReadTimeLable"));
        leftFileReadTimeLable->setMinimumSize(QSize(0, 30));
        leftFileReadTimeLable->setMaximumSize(QSize(150, 16777215));

        horizontalLayout_11->addWidget(leftFileReadTimeLable);

        leftFileReadTimeCheckBox = new QCheckBox(HighFindDialog);
        leftFileReadTimeCheckBox->setObjectName(QString::fromUtf8("leftFileReadTimeCheckBox"));
        leftFileReadTimeCheckBox->setMaximumSize(QSize(30, 30));
        leftFileReadTimeCheckBox->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_11->addWidget(leftFileReadTimeCheckBox);

        leftFileReadTimeDateEdit = new QDateEdit(HighFindDialog);
        leftFileReadTimeDateEdit->setObjectName(QString::fromUtf8("leftFileReadTimeDateEdit"));
        leftFileReadTimeDateEdit->setAcceptDrops(false);
        leftFileReadTimeDateEdit->setReadOnly(true);
        leftFileReadTimeDateEdit->setCalendarPopup(true);

        horizontalLayout_11->addWidget(leftFileReadTimeDateEdit);

        rightFileReadTimeLable = new QLabel(HighFindDialog);
        rightFileReadTimeLable->setObjectName(QString::fromUtf8("rightFileReadTimeLable"));
        rightFileReadTimeLable->setMaximumSize(QSize(50, 16777215));
        rightFileReadTimeLable->setLayoutDirection(Qt::LeftToRight);
        rightFileReadTimeLable->setAlignment(Qt::AlignCenter);

        horizontalLayout_11->addWidget(rightFileReadTimeLable);

        rightFileReadTimeCheckBox = new QCheckBox(HighFindDialog);
        rightFileReadTimeCheckBox->setObjectName(QString::fromUtf8("rightFileReadTimeCheckBox"));
        rightFileReadTimeCheckBox->setMaximumSize(QSize(30, 30));
        rightFileReadTimeCheckBox->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_11->addWidget(rightFileReadTimeCheckBox);

        rightFileReadTimeDateEdit = new QDateEdit(HighFindDialog);
        rightFileReadTimeDateEdit->setObjectName(QString::fromUtf8("rightFileReadTimeDateEdit"));
        rightFileReadTimeDateEdit->setReadOnly(true);
        rightFileReadTimeDateEdit->setCalendarPopup(true);

        horizontalLayout_11->addWidget(rightFileReadTimeDateEdit);


        gridLayout->addLayout(horizontalLayout_11, 10, 0, 1, 2);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        partitionLable = new QLabel(HighFindDialog);
        partitionLable->setObjectName(QString::fromUtf8("partitionLable"));
        partitionLable->setEnabled(true);
        partitionLable->setMinimumSize(QSize(60, 30));
        partitionLable->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(partitionLable);

        partitionComboBox = new QComboBox(HighFindDialog);
        partitionComboBox->setObjectName(QString::fromUtf8("partitionComboBox"));
        partitionComboBox->setMinimumSize(QSize(100, 0));

        horizontalLayout_2->addWidget(partitionComboBox);


        horizontalLayout_12->addLayout(horizontalLayout_2);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_12->addItem(horizontalSpacer_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        fileFlagLable = new QLabel(HighFindDialog);
        fileFlagLable->setObjectName(QString::fromUtf8("fileFlagLable"));
        fileFlagLable->setMinimumSize(QSize(60, 30));
        fileFlagLable->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout->addWidget(fileFlagLable);

        fileFlagComboBox = new QComboBox(HighFindDialog);
        fileFlagComboBox->setObjectName(QString::fromUtf8("fileFlagComboBox"));
        fileFlagComboBox->setMinimumSize(QSize(100, 0));

        horizontalLayout->addWidget(fileFlagComboBox);


        horizontalLayout_12->addLayout(horizontalLayout);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_12->addItem(horizontalSpacer_3);

        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setObjectName(QString::fromUtf8("horizontalLayout_15"));
        extendLable = new QLabel(HighFindDialog);
        extendLable->setObjectName(QString::fromUtf8("extendLable"));
        extendLable->setMinimumSize(QSize(60, 30));
        extendLable->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_15->addWidget(extendLable);

        extendComboBox = new QComboBox(HighFindDialog);
        extendComboBox->setObjectName(QString::fromUtf8("extendComboBox"));
        extendComboBox->setMinimumSize(QSize(100, 0));

        horizontalLayout_15->addWidget(extendComboBox);


        horizontalLayout_12->addLayout(horizontalLayout_15);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_12->addItem(horizontalSpacer_4);

        resetPushButton = new QPushButton(HighFindDialog);
        resetPushButton->setObjectName(QString::fromUtf8("resetPushButton"));

        horizontalLayout_12->addWidget(resetPushButton);


        gridLayout->addLayout(horizontalLayout_12, 0, 0, 1, 2);

        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        CaseInsensitiveCheckBox = new QCheckBox(HighFindDialog);
        CaseInsensitiveCheckBox->setObjectName(QString::fromUtf8("CaseInsensitiveCheckBox"));
        CaseInsensitiveCheckBox->setMaximumSize(QSize(200, 16777215));

        horizontalLayout_14->addWidget(CaseInsensitiveCheckBox);

        completeCheckBox = new QCheckBox(HighFindDialog);
        completeCheckBox->setObjectName(QString::fromUtf8("completeCheckBox"));
        completeCheckBox->setMaximumSize(QSize(200, 16777215));

        horizontalLayout_14->addWidget(completeCheckBox);

        RegExpCheckBox = new QCheckBox(HighFindDialog);
        RegExpCheckBox->setObjectName(QString::fromUtf8("RegExpCheckBox"));
        RegExpCheckBox->setMaximumSize(QSize(200, 16777215));

        horizontalLayout_14->addWidget(RegExpCheckBox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_14->addItem(horizontalSpacer);


        gridLayout->addLayout(horizontalLayout_14, 6, 0, 1, 2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        PMftNumbeLable = new QLabel(HighFindDialog);
        PMftNumbeLable->setObjectName(QString::fromUtf8("PMftNumbeLable"));
        PMftNumbeLable->setMinimumSize(QSize(100, 30));

        horizontalLayout_4->addWidget(PMftNumbeLable);

        PMftLableIcon = new QLabel(HighFindDialog);
        PMftLableIcon->setObjectName(QString::fromUtf8("PMftLableIcon"));
        PMftLableIcon->setMaximumSize(QSize(30, 30));

        horizontalLayout_4->addWidget(PMftLableIcon);

        PMftNumbeLineEdit = new QLineEdit(HighFindDialog);
        PMftNumbeLineEdit->setObjectName(QString::fromUtf8("PMftNumbeLineEdit"));

        horizontalLayout_4->addWidget(PMftNumbeLineEdit);


        gridLayout->addLayout(horizontalLayout_4, 3, 0, 1, 2);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        FileNameLable = new QLabel(HighFindDialog);
        FileNameLable->setObjectName(QString::fromUtf8("FileNameLable"));
        FileNameLable->setMinimumSize(QSize(100, 30));

        horizontalLayout_6->addWidget(FileNameLable);

        FileNameLineEdit = new QLineEdit(HighFindDialog);
        FileNameLineEdit->setObjectName(QString::fromUtf8("FileNameLineEdit"));

        horizontalLayout_6->addWidget(FileNameLineEdit);


        gridLayout->addLayout(horizontalLayout_6, 5, 0, 1, 2);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        FileAttributeLable = new QLabel(HighFindDialog);
        FileAttributeLable->setObjectName(QString::fromUtf8("FileAttributeLable"));
        FileAttributeLable->setMinimumSize(QSize(100, 30));

        horizontalLayout_5->addWidget(FileAttributeLable);

        FileAttributeLine = new QLineEdit(HighFindDialog);
        FileAttributeLine->setObjectName(QString::fromUtf8("FileAttributeLine"));

        horizontalLayout_5->addWidget(FileAttributeLine);

        FileAttributeLineComboBox = new QComboBox(HighFindDialog);
        FileAttributeLineComboBox->setObjectName(QString::fromUtf8("FileAttributeLineComboBox"));
        FileAttributeLineComboBox->setMinimumSize(QSize(100, 0));

        horizontalLayout_5->addWidget(FileAttributeLineComboBox);


        gridLayout->addLayout(horizontalLayout_5, 4, 0, 1, 2);


        retranslateUi(HighFindDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), HighFindDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), HighFindDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(HighFindDialog);
    } // setupUi

    void retranslateUi(QDialog *HighFindDialog)
    {
        HighFindDialog->setWindowTitle(QApplication::translate("HighFindDialog", "Dialog", nullptr));
        leftFileRealSizeLable->setText(QApplication::translate("HighFindDialog", "\346\226\207\344\273\266\345\244\247\345\260\217:    ", nullptr));
        leftFileRealSizeComboBox->setItemText(0, QApplication::translate("HighFindDialog", "Bytes", nullptr));
        leftFileRealSizeComboBox->setItemText(1, QApplication::translate("HighFindDialog", "KB", nullptr));
        leftFileRealSizeComboBox->setItemText(2, QApplication::translate("HighFindDialog", "MB", nullptr));
        leftFileRealSizeComboBox->setItemText(3, QApplication::translate("HighFindDialog", "GB", nullptr));

        rightFileRealSizeLable->setText(QApplication::translate("HighFindDialog", "-", nullptr));
        rightFileRealSizeComboBox->setItemText(0, QApplication::translate("HighFindDialog", "Bytes", nullptr));
        rightFileRealSizeComboBox->setItemText(1, QApplication::translate("HighFindDialog", "KB", nullptr));
        rightFileRealSizeComboBox->setItemText(2, QApplication::translate("HighFindDialog", "MB", nullptr));
        rightFileRealSizeComboBox->setItemText(3, QApplication::translate("HighFindDialog", "GB", nullptr));

        leftFileCreationTimeLable->setText(QApplication::translate("HighFindDialog", "\346\226\207\344\273\266\345\210\233\345\273\272\346\227\266\351\227\264:", nullptr));
        leftFileCreationTimeCheckBox->setText(QString());
        leftFileCreationTimeDateEdit->setDisplayFormat(QApplication::translate("HighFindDialog", "yyyy-M-d", nullptr));
        rightFileCreationTimeLable->setText(QApplication::translate("HighFindDialog", "-", nullptr));
        rightFileCreationTimeCheckBox->setText(QString());
        rightFileCreationTimeDateEdit->setDisplayFormat(QApplication::translate("HighFindDialog", "yyyy-M-d", nullptr));
        leftFileChangeTimeLable->setText(QApplication::translate("HighFindDialog", "\346\226\207\344\273\266\344\277\256\346\224\271\346\227\266\351\227\264:", nullptr));
        leftFileChangeTimeCheckBox->setText(QString());
        leftFileChangeTimeDateEdit->setDisplayFormat(QApplication::translate("HighFindDialog", "yyyy-M-d", nullptr));
        rightFileChangeTimeLable->setText(QApplication::translate("HighFindDialog", "-", nullptr));
        rightFileChangeTimeCheckBox->setText(QString());
        rightFileChangeTimeDateEdit->setDisplayFormat(QApplication::translate("HighFindDialog", "yyyy-M-d", nullptr));
        MftNumbeLable->setText(QApplication::translate("HighFindDialog", "MFT:    ", nullptr));
        MftLableIcon->setText(QString());
        leftFileReadTimeLable->setText(QApplication::translate("HighFindDialog", "\346\226\207\344\273\266\350\256\277\351\227\256\346\227\266\351\227\264:", nullptr));
        leftFileReadTimeCheckBox->setText(QString());
        leftFileReadTimeDateEdit->setDisplayFormat(QApplication::translate("HighFindDialog", "yyyy-M-d", nullptr));
        rightFileReadTimeLable->setText(QApplication::translate("HighFindDialog", "-", nullptr));
        rightFileReadTimeCheckBox->setText(QString());
        rightFileReadTimeDateEdit->setDisplayFormat(QApplication::translate("HighFindDialog", "yyyy-M-d", nullptr));
        partitionLable->setText(QApplication::translate("HighFindDialog", "\347\233\230\347\254\246:", nullptr));
        fileFlagLable->setText(QApplication::translate("HighFindDialog", "\346\226\207\344\273\266\347\261\273\345\236\213:", nullptr));
        extendLable->setText(QApplication::translate("HighFindDialog", "\346\211\251\345\261\225\345\220\215:", nullptr));
        resetPushButton->setText(QApplication::translate("HighFindDialog", "\346\201\242\345\244\215\345\210\260\351\273\230\350\256\244", nullptr));
        CaseInsensitiveCheckBox->setText(QApplication::translate("HighFindDialog", "\345\214\272\345\210\206\345\244\247\345\260\217\345\206\231", nullptr));
        completeCheckBox->setText(QApplication::translate("HighFindDialog", "\345\211\215\347\275\256\345\256\214\345\205\250\345\214\271\351\205\215", nullptr));
        RegExpCheckBox->setText(QApplication::translate("HighFindDialog", "\344\275\277\347\224\250\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217", nullptr));
        PMftNumbeLable->setText(QApplication::translate("HighFindDialog", "PMFT:   ", nullptr));
        PMftLableIcon->setText(QString());
        FileNameLable->setText(QApplication::translate("HighFindDialog", "\346\226\207\344\273\266\345\220\215:  ", nullptr));
        FileAttributeLable->setText(QApplication::translate("HighFindDialog", "\346\226\207\344\273\266\345\261\236\346\200\247:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class HighFindDialog: public Ui_HighFindDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HIGHFINDDIALOG_H
