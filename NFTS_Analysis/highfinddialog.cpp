#include "highfinddialog.h"
#include "ui_highfinddialog.h"
#include <QMessageBox>
HighFindDialog::HighFindDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HighFindDialog)
{
    ui->setupUi(this);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
    // 去掉问号和左上角图标，只保留关闭
    this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    this->setWindowTitle("高级搜索");
    this->setWindowIcon(QIcon(":/Images/quickRun.png"));
    QIcon icon =  qApp->style()->standardIcon(QStyle::SP_MessageBoxWarning);
    ui->MftLableIcon->setPixmap(icon.pixmap(25,25));
    ui->PMftLableIcon->setPixmap(icon.pixmap(25,25));

    //日期设置当前时间
    ui->leftFileCreationTimeDateEdit->setDate(QDate::currentDate());
    ui->rightFileCreationTimeDateEdit->setDate(QDate::currentDate());
    ui->leftFileChangeTimeDateEdit->setDate(QDate::currentDate());
    ui->rightFileChangeTimeDateEdit->setDate(QDate::currentDate());
    ui->leftFileReadTimeDateEdit->setDate(QDate::currentDate());
    ui->rightFileReadTimeDateEdit->setDate(QDate::currentDate());

    //正则验证器，只能输入十进制整数
    QRegExp rE("[0-9]+");
    QRegExpValidator *regs = new QRegExpValidator(rE, this);
    ui->MftNumbeLineEdit->setValidator(regs);
    ui->PMftNumbeLineEdit->setValidator(regs);
    ui->FileAttributeLine->setValidator(regs);
    ui->leftFileRealSizeLine->setValidator(regs);
    ui->rightFileRealSizeLine->setValidator(regs);

    //悬浮提示
    ui->MftLableIcon->setToolTip("搜索文件中的MFT号,请以十进制搜索\n"
                                 "MFT号：一个盘中只能有一个mft号,\n"
                                 "一个mft号表示一个文件，\n"
                                 "或者表示多个文件(共用一块内存数据,\n"
                                 "一个文件内容被修改，相当于其他文件内容一样修改)\n"
                                 "留空则搜索全部文件");
    ui->PMftLableIcon->setToolTip("搜索文件中的PMFT号,请以十进制搜索\n"
                                  "PMFT号：当前mft号上一层，\n"
                                  "一般指的都是文件夹\n"
                                  "留空则搜索全部文件");
    ui->MftNumbeLineEdit->setToolTip("mft范围0~4294967295");
    ui->PMftNumbeLineEdit->setToolTip("pMft范围0~4294967295");
    ui->FileAttributeLine->setToolTip("请以十进制搜索\n\
                                      属性是以下的组合形式出现 --（比如：属性A | 属性B）\n\
                                      32 （0x20）：作为存档文件或目录的文件或目录。应用程序通常使用此属性来标记要备份或删除的文件。\n\
                                      2048 （0x800）：压缩的文件或目录。对于文件，将压缩文件中的所有数据。对于目录，压缩是新创建的文件和子目录的默认设置。\n\
                                      64 （0x40）：此值保留供系统使用。\n\
                                      16 （0x10）：标识目录的句柄。\n\
                                      16384 （0x4000）：已加密的文件或目录。对于文件，文件中的所有数据流都已加密。对于目录，加密是新创建的文件和子目录的默认设置。\n\
                                      2 （0x2）：文件或目录处于隐藏状态。它不包括在普通目录列表中。\n\
                                      32768 （0x8000）：目录或用户数据流配置为完整性（仅在 ReFS 卷上受支持）。它不包括在普通目录列表中。如果文件已重命名，则完整性设置将保留在文件中。如果复制了文件，则如果源文件或目标目录设置了完整性，则目标文件将设置完整性。\n\
                                      128 （0x80）：未设置其他属性的文件。此属性仅在单独使用时有效。\n\
                                      8192 （0x2000）：内容索引服务不会对文件或目录编制索引。\n\
                                      131072 （0x20000）：用户数据流不被后台数据完整性扫描程序（AKA 清理器）读取。在目录上设置时，它仅提供继承。此标志仅在存储空间和 ReFS 卷上受支持。它不包括在普通目录列表中。\n\
                                      4096 （0x1000）：文件的数据不会立即可用。此属性指示文件数据已物理移动到脱机存储。此属性由远程存储（分层存储管理软件）使用。应用程序不应任意更改此属性。\n\
                                      1 （0x1）：只读文件。应用程序可以读取该文件，但不能写入或删除它。在目录中不采用此属性。\n\
                                      4194304 （0x400000）：设置此属性时，表示文件或目录未在本地完全存在。对于一个文件，这意味着并非所有数据都在本地存储上（例如，它可能很稀疏，有些数据仍在远程存储中）。对于目录，这意味着某些目录内容正在从另一个位置进行虚拟化。读取文件/枚举目录将比正常情况更昂贵，例如，它将导致至少一些文件/目录内容从远程存储中获取。只有内核模式调用方可以设置此位。\n\
                                      262144 （0x40000）：设置此属性时，表示文件或目录在本地系统上没有物理表示形式;该项目是虚拟的。打开物品将比平时更昂贵，例如，它将导致至少一些物品从远程商店获取。\n\
            1024 （0x400）：具有关联重分析点的文件或目录，或者作为符号链接的文件。\n\
            512 （0x200）：稀疏文件的文件。\n\
            4 （0x4）：操作系统使用其中一部分或独占使用的文件或目录。\n\
            256 （0x100）：用于临时存储的文件。如果有足够的缓存内存可用，文件系统会避免将数据写回大容量存储，因为通常情况下，应用程序会在句柄关闭后删除临时文件。在这种情况下，系统可以完全避免写入数据。否则，将在句柄关闭后写入数据。\n\
            65536 （0x10000）：此值保留供系统使用。\n"
                                        "留空则搜索全部文件");
    ui->FileNameLineEdit->setToolTip("搜索包含文本的文件名\n"
                                     "留空则搜索全部文件");
    QString strSize = "文件大小范围.\n"
                      "留空则搜索全部文件.\n"
                      "仅设置初始大小则搜索此大小即以上的文件.\n"
                      "仅设置截止大小则搜索此大小即以下的文件.\n"
                      "设置初始和截止大小则搜索此大小范围的文件.";
    ui->leftFileRealSizeLine->setToolTip(strSize);
    ui->rightFileRealSizeLine->setToolTip(strSize);


    QString strTime ="留空表示搜索全部文件.\n"
                     "仅设置起始时间则搜索此创建时间点及之后的文件.\n"
                     "仅设置截止时间则搜索此创建时间点及之前的文件.\n"
                     "设置起始和截止时间则搜索此创建时间段的文件.";
    ui->leftFileCreationTimeDateEdit->setToolTip("创建时间范围.\n"+strTime);
    ui->rightFileCreationTimeDateEdit->setToolTip("创建时间范围.\n"+strTime);
    ui->leftFileChangeTimeDateEdit->setToolTip("修改时间范围.\n"+strTime);
    ui->rightFileChangeTimeDateEdit->setToolTip("修改时间范围.\n"+strTime);
    ui->leftFileReadTimeDateEdit->setToolTip("访问时间范围.\n"+strTime);
    ui->rightFileReadTimeDateEdit->setToolTip("访问时间范围.\n"+strTime);

    //绑定model
    ui->partitionComboBox->setModel(MemoryPool::getInstance()->getPartitionModel());
    ui->fileFlagComboBox->setModel(MemoryPool::getInstance()->getFlagModel());
    ui->extendComboBox->setModel(MemoryPool::getInstance()->getIconModel());
    ui->FileAttributeLineComboBox->setModel(MemoryPool::getInstance()->getAttModel());


}

HighFindDialog::~HighFindDialog()
{
    delete ui;
}

//日期比较 -  左大于右:右=左    右小于左:左=右
void HighFindDialog::on_leftFileCreationTimeDateEdit_dateChanged(const QDate &date)
{
    if(date > ui->rightFileCreationTimeDateEdit->date())
        ui->rightFileCreationTimeDateEdit->setDate(date);
}

void HighFindDialog::on_rightFileCreationTimeDateEdit_dateChanged(const QDate &date)
{
    if(date < ui->leftFileCreationTimeDateEdit->date())
        ui->leftFileCreationTimeDateEdit->setDate(date);
}

void HighFindDialog::on_leftFileChangeTimeDateEdit_dateChanged(const QDate &date)
{
    if(date > ui->rightFileChangeTimeDateEdit->date())
        ui->rightFileChangeTimeDateEdit->setDate(date);
}

void HighFindDialog::on_rightFileChangeTimeDateEdit_dateChanged(const QDate &date)
{
    if(date < ui->leftFileChangeTimeDateEdit->date())
        ui->leftFileChangeTimeDateEdit->setDate(date);
}

void HighFindDialog::on_leftFileReadTimeDateEdit_dateChanged(const QDate &date)
{
    if(date > ui->rightFileReadTimeDateEdit->date())
        ui->rightFileReadTimeDateEdit->setDate(date);
}

void HighFindDialog::on_rightFileReadTimeDateEdit_dateChanged(const QDate &date)
{
    if(date < ui->leftFileReadTimeDateEdit->date())
        ui->leftFileReadTimeDateEdit->setDate(date);
}

//从ComboBox赋值到行编辑

void HighFindDialog::on_FileAttributeLineComboBox_currentIndexChanged(const QString &arg1)
{
    ui->FileAttributeLine->setText(arg1);
}

void HighFindDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if(!MemoryPool::getInstance()->getIsFinish())
        return;
    S_HIGHFIND_UPDATE_VIEW highFindStruct;

    if(button == (QAbstractButton *)(ui->buttonBox->button(QDialogButtonBox::Ok))){
        bool isOk;
        //1.盘符
        if(ui->partitionComboBox->currentText().isEmpty())
            highFindStruct.partitionSerial = -1;
        else{
            highFindStruct.partitionSerial = ui->partitionComboBox->currentText().split(QChar(' ')).last().toInt();
        }
        //2.文件类型
        if(ui->fileFlagComboBox->currentText().isEmpty())
            highFindStruct.fileFlag = -1;
        else{
            highFindStruct.fileFlag = g_FileFlagL.indexOf(ui->fileFlagComboBox->currentText());
        }
        //3.文件扩展名
        highFindStruct.ExtendName = ui->extendComboBox->currentText();
        //4.mft

        highFindStruct.mft = ui->MftNumbeLineEdit->text().toULongLong(&isOk);
        if(!isOk)
            highFindStruct.mft = -1;
        //5.pmft
        highFindStruct.pMft = ui->PMftNumbeLineEdit->text().toULongLong(&isOk);
        if(!isOk)
            highFindStruct.pMft = -1;
        //5.文件属性
        highFindStruct.fileAtt = ui->FileAttributeLine->text().toULongLong(&isOk);
        if(!isOk)
            highFindStruct.fileAtt = -1;
        //6.文件名
        highFindStruct.searchName = ui->FileNameLineEdit->text();
        //7.区分大小写
        highFindStruct.CaseInsensitive = ui->CaseInsensitiveCheckBox->checkState() == Qt::Checked ? true : false;
        //8.前置完全匹配
        highFindStruct.Complete = ui->completeCheckBox->checkState() == Qt::Checked ? true : false;
        //8.使用正则
        highFindStruct.RegExp = ui->RegExpCheckBox->checkState() == Qt::Checked ? true : false;
        //9.文件占用空间范围
        highFindStruct.fileRealRange.leftValue = ui->leftFileRealSizeLine->text().toULongLong(&isOk);
        if(!isOk)
            highFindStruct.fileRealRange.leftValue = 0;
        else{
            switch(ui->leftFileRealSizeComboBox->currentIndex())
            {
            case 1:
                highFindStruct.fileRealRange.leftValue *= 1024; // kb
                break;
            case 2:
                highFindStruct.fileRealRange.leftValue *= (1024 * 1024); // mb
                break;
            case 3:
                highFindStruct.fileRealRange.leftValue *= (1024 * 1024 *1024); //gb
                break;
            default:
                break;
            }
        }
        highFindStruct.fileRealRange.rightValue = ui->rightFileRealSizeLine->text().toULongLong(&isOk);
        if(!isOk)
            highFindStruct.fileRealRange.rightValue = -1;
        else{
            switch(ui->rightFileRealSizeComboBox->currentIndex())
            {
            case 1:
                highFindStruct.fileRealRange.rightValue *= 1024;
                break;
            case 2:
                highFindStruct.fileRealRange.rightValue *= (1024 * 1024);
                break;
            case 3:
                highFindStruct.fileRealRange.rightValue *= (1024 * 1024 *1024);
                break;
            default:
                break;
            }
        }
        //10.文件创建时间范围
        if(ui->leftFileCreationTimeDateEdit->isReadOnly())
            highFindStruct.fileCreationTimeRange.leftValue = 0;
        else{
            highFindStruct.fileCreationTimeRange.leftValue = GlobalFunction::getInstance()->TimestampToFILETIME(ui->leftFileCreationTimeDateEdit->dateTime().toSecsSinceEpoch());
        }
        if(ui->rightFileCreationTimeDateEdit->isReadOnly())
            highFindStruct.fileCreationTimeRange.rightValue = -1;
        else{
            highFindStruct.fileCreationTimeRange.rightValue = GlobalFunction::getInstance()->TimestampToFILETIME(ui->rightFileCreationTimeDateEdit->dateTime().toSecsSinceEpoch());
        }
        //11.文件修改时间范围
        if(ui->leftFileChangeTimeDateEdit->isReadOnly())
            highFindStruct.fileChangeTimeRange.leftValue = 0;
        else{
            highFindStruct.fileChangeTimeRange.leftValue = GlobalFunction::getInstance()->TimestampToFILETIME(ui->leftFileChangeTimeDateEdit->dateTime().toSecsSinceEpoch());
        }
        if(ui->rightFileChangeTimeDateEdit->isReadOnly())
            highFindStruct.fileChangeTimeRange.rightValue = -1;
        else{
            highFindStruct.fileChangeTimeRange.rightValue = GlobalFunction::getInstance()->TimestampToFILETIME(ui->rightFileChangeTimeDateEdit->dateTime().toSecsSinceEpoch());
        }
        //12.文件访问时间范围
        if(ui->leftFileReadTimeDateEdit->isReadOnly())
            highFindStruct.fileReadTimeRange.leftValue = 0;
        else{
            highFindStruct.fileReadTimeRange.leftValue = GlobalFunction::getInstance()->TimestampToFILETIME(ui->leftFileReadTimeDateEdit->dateTime().toSecsSinceEpoch());
        }
        if(ui->rightFileReadTimeDateEdit->isReadOnly())
            highFindStruct.fileReadTimeRange.rightValue = -1;
        else{
            highFindStruct.fileReadTimeRange.rightValue = GlobalFunction::getInstance()->TimestampToFILETIME(ui->rightFileReadTimeDateEdit->dateTime().toSecsSinceEpoch());
        }

        emit SignalHighFind(highFindStruct);
    }
}

void HighFindDialog::on_resetPushButton_clicked()
{
    ui->partitionComboBox->setCurrentIndex(0);
    ui->fileFlagComboBox->setCurrentIndex(0);
    ui->extendComboBox->setCurrentIndex(0);
    ui->FileAttributeLineComboBox->setCurrentIndex(0);
    ui->FileAttributeLine->clear();
    ui->MftNumbeLineEdit->clear();
    ui->FileAttributeLine->clear();
    ui->PMftNumbeLineEdit->clear();
    ui->FileNameLineEdit->clear();
    ui->leftFileRealSizeLine->clear();
    ui->rightFileRealSizeLine->clear();
    ui->CaseInsensitiveCheckBox->setCheckState(Qt::Unchecked);
    ui->completeCheckBox->setCheckState(Qt::Unchecked);
    ui->RegExpCheckBox->setCheckState(Qt::Unchecked);
    ui->leftFileCreationTimeCheckBox->setCheckState(Qt::Unchecked);
    ui->rightFileCreationTimeCheckBox->setCheckState(Qt::Unchecked);
    ui->leftFileChangeTimeCheckBox->setCheckState(Qt::Unchecked);
    ui->rightFileChangeTimeCheckBox->setCheckState(Qt::Unchecked);
    ui->leftFileReadTimeCheckBox->setCheckState(Qt::Unchecked);
    ui->rightFileReadTimeCheckBox->setCheckState(Qt::Unchecked);

    ui->rightFileRealSizeComboBox->setCurrentIndex(0);
    ui->leftFileRealSizeComboBox->setCurrentIndex(0);
}
//文件大小单位发生改变，左右同时改变
void HighFindDialog::on_leftFileRealSizeComboBox_activated(int index)
{
    ui->leftFileRealSizeComboBox->setCurrentIndex(index);
    ui->rightFileRealSizeComboBox->setCurrentIndex(index);
}
//文件大小单位发生改变，左右同时改变
void HighFindDialog::on_rightFileRealSizeComboBox_activated(int index)
{
    ui->leftFileRealSizeComboBox->setCurrentIndex(index);
    ui->rightFileRealSizeComboBox->setCurrentIndex(index);
}
//日期框状态根据复选框状态改变
void HighFindDialog::on_leftFileCreationTimeCheckBox_stateChanged(int arg1)
{
    ui->leftFileCreationTimeDateEdit->setReadOnly(!arg1);
}

void HighFindDialog::on_rightFileCreationTimeCheckBox_stateChanged(int arg1)
{
    ui->rightFileCreationTimeDateEdit->setReadOnly(!arg1);
}

void HighFindDialog::on_leftFileChangeTimeCheckBox_stateChanged(int arg1)
{
    ui->leftFileChangeTimeDateEdit->setReadOnly(!arg1);
}

void HighFindDialog::on_rightFileChangeTimeCheckBox_stateChanged(int arg1)
{
    ui->rightFileChangeTimeDateEdit->setReadOnly(!arg1);
}

void HighFindDialog::on_leftFileReadTimeCheckBox_stateChanged(int arg1)
{
    ui->leftFileReadTimeDateEdit->setReadOnly(!arg1);
}

void HighFindDialog::on_rightFileReadTimeCheckBox_stateChanged(int arg1)
{
    ui->rightFileReadTimeDateEdit->setReadOnly(!arg1);
}


////前置完全匹配选中 正则取消选择
//void HighFindDialog::on_completeCheckBox_clicked(bool checked)
//{
//    if(checked)
//        ui->RegExpCheckBox->setChecked(false);
//}
////正则完全匹配选中 前置取消选择
//void HighFindDialog::on_RegExpCheckBox_clicked(bool checked)
//{
//    if(checked)
//        ui->completeCheckBox->setChecked(false);
//}
//正则完全匹配选中 前置取消选择
void HighFindDialog::on_RegExpCheckBox_stateChanged(int arg1)
{
    if(arg1 ==  Qt::Checked) {

        QString fileName = ui->FileNameLineEdit->text();
        QRegExp regExp;
        //区分大小写
        if(ui->CaseInsensitiveCheckBox->checkState() == Qt::Checked ? true : false) {
            regExp = QRegExp(fileName,Qt::CaseSensitive);
        } else {
            regExp = QRegExp(fileName,Qt::CaseInsensitive);
        }
        if(!regExp.isValid()) {
            QMessageBox::warning(this,"警告...","正则语法无效,请重新输入正确语法!");
            ui->RegExpCheckBox->blockSignals(true);
            ui->RegExpCheckBox->setCheckState(Qt::Unchecked);
            ui->RegExpCheckBox->blockSignals(false);
        } else {
            ui->completeCheckBox->blockSignals(true);
            ui->completeCheckBox->setCheckState(Qt::Unchecked);
            ui->completeCheckBox->blockSignals(false);
        }
    }
}
//前置完全匹配选中 正则取消选择
void HighFindDialog::on_completeCheckBox_stateChanged(int arg1)
{
    if(arg1 ==  Qt::Checked) {
        ui->RegExpCheckBox->blockSignals(true);
        ui->RegExpCheckBox->setCheckState(Qt::Unchecked);
        ui->RegExpCheckBox->blockSignals(false);

    }
}
//文件名框发生编辑操作
void HighFindDialog::on_FileNameLineEdit_textEdited(const QString &arg1)
{
    if(ui->RegExpCheckBox->checkState() == Qt::Checked) {
        ui->RegExpCheckBox->blockSignals(true);
        ui->RegExpCheckBox->setCheckState(Qt::Unchecked);
        ui->RegExpCheckBox->blockSignals(false);
    }
}
