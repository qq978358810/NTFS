#include "usnmonitordialog.h"
#include "ui_usnmonitordialog.h"
#include <QRegExpValidator>
#include <QMessageBox>
UsnMonitorDialog::UsnMonitorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UsnMonitorDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
    // 去掉问号和左上角图标，只保留关闭
    this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    // this->setWindowModality(Qt::WindowModal);
    this->setWindowIcon(QIcon(":/Images/quickRun.png"));

    QStringList sL = QStringList()<<""<<"A:"<<"B:"<<"C:"<<"D:"<<"E:"<<"F:"<<"G:"<<"H:"<<"I:"<<"J:"<<"K:"<<"L:"<<"M:"<<"N:"<<"O:"<<"P:"<<"Q:"<<"R:"<<"S:"<<"T:"<<"U:"<<"V:"<<"W:"<<"X:"<<"Y:"<<"Z:";
    ui->partitonComboBox->addItems(sL);

    //正则验证器，只能输入十进制整数
    QRegExp rE("-?[0-9]+");
    QRegExpValidator *regs = new QRegExpValidator(rE, this);
    ui->mftLineEdit_2->setValidator(regs);
    ui->pMftLineEdit_3->setValidator(regs);
    ui->reasonLineEdit_4->setValidator(regs);
    ui->attrLineEdit_5->setValidator(regs);

    //悬浮提示
    ui->mftLineEdit_2->setToolTip("监控文件中的MFT号,请以十进制\n"
                                  "MFT号：一个盘中只能有一个mft号,\n"
                                  "一个mft号表示一个文件，\n"
                                  "或者表示多个文件(共用一块内存数据,\n"
                                  "一个文件内容被修改，相当于其他文件内容一样修改)\n"
                                  "留空则监控全部文件\n"
                                  "mft范围0~4294967295");
    ui->pMftLineEdit_3->setToolTip("监控文件中的PMFT号,请以十进制\n"
                                   "PMFT号：当前mft号上一层，\n"
                                   "一般指的都是文件夹\n"
                                   "-1则监控全部文件\n"
                                   "pMft范围0~4294967295");
    ui->reasonLineEdit_4->setToolTip("标识自文件或目录打开以来在此文件或目录日志记录中累积的更改原因的标志。\n"
                                     "当文件或目录关闭时，将使用设置USN_REASON_CLOSE标志生成最终 USN 记录。下一个更改（例如，在下一个打开操作或删除操作之后）将使用一组新的原因标志启动一条新记录。\n"
                                     " 重命名或移动操作会生成两条 USN 记录，一条记录项目的旧父目录，另一条记录新的父目录。\n"
                                     "32768 (0x00008000): 用户已更改一个或多个文件或目录属性（例如，只读、隐藏、系统、存档或稀疏属性），或者更改了一个或多个时间戳。\n"
                                     "2147483648 (0x80000000):文件或目录已关闭。\n"
                                     "131072 (0x00020000):文件或目录的压缩状态从 或 更改为 压缩。\n"
                                     "2 (0x00000002):文件或目录已扩展（添加到）。\n"
                                     "1 (0x00000001):文件或目录中的数据将被覆盖。\n"
                                     "4 (0x00000004):文件或目录被截断。\n"
                                     "1024 (0x00000400):用户对文件或目录的扩展属性进行了更改。基于 Windows 的应用程序无法访问这些 NTFS 文件系统属性。\n"
                                     "262144 (0x00040000):文件或目录已加密或解密。\n"
                                     "256 (0x00000100):文件或目录是首次创建。\n"
                                     "512 (0x00000200):文件或目录即被删除。\n"
                                     "65536 (0x00010000):NTFS 文件系统硬链接将添加到文件或目录中，或从中删除。NTFS 文件系统硬链接（类似于 POSIX 硬链接）是查看同一文件或目录的几个目录条目之一。\n"
                                     "16384 (0x00004000):用户更改FILE_ATTRIBUTE_NOT_CONTENT_INDEXED属性。也就是说，用户将文件或目录从可将内容编入索引的文件或目录更改为无法为内容编制索引的文件或目录，反之亦然。内容索引允许通过构建所选内容的数据库来快速搜索数据。\n"
                                     "8388608 (0x00800000):用户更改了给定流的 FILE_ATTRIBUTE_INTEGRITY_STREAM 属性的状态。在 ReFS 文件系统上，完整性流维护该流的所有数据的校验和，以便在读取或写入操作期间可以验证文件的内容。\n"
                                     "32 (0x00000020):文件的一个或多个命名数据流被扩展（添加到）。\n"
                                     "16 (0x00000010):文件的一个或多个命名数据流中的数据将被覆盖。\n"
                                     "64 (0x00000040):文件的一个或多个命名数据流将被截断。\n"
                                     "524288 (0x00080000):文件或目录的对象标识符已更改。\n"
                                     "8192 (0x00002000):将重命名文件或目录，并且USN_RECORD_V2结构中的文件名为新名称。\n"
                                     "4096 (0x00001000):文件或目录将被重命名，USN_RECORD_V2结构中的文件名是以前的名称。\n"
                                     "1048576 (0x00100000):文件或目录中包含的重新分析点已更改，或者向文件或目录中添加或删除了重新分析点。\n"
                                     "2048 (0x00000800):对文件或目录的访问权限进行更改。\n"
                                     "2097152 (0x00200000):在文件中添加或删除命名流，或重命名命名流。\n"
                                     "4194304 (0x00400000):给定的流通过 TxF 事务进行修改。\n"
                                     "-1则监控全部文件");

    ui->attrLineEdit_5->setToolTip("属性是以下的组合形式出现 --（比如：属性A | 属性B）\n\
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
                                        "-1则监控全部文件");
    ui->fileNameLineEdit_6->setToolTip("监控完全相等的文件名\n"
                                       "留空则监控全部文件");
    ui->commandTextEdit->setToolTip("设置符合Windows下cmd指令,同时匹配以上设置的参数,启用执行,按照设置的最低时长,运行当前指令.\n当前指令不做检查,直接执行,请确保指令的正确性.");

    this->clearArgs();

}

UsnMonitorDialog::~UsnMonitorDialog()
{
    delete ui;
}
//设置参数
void UsnMonitorDialog::setArgs(S_MONITOR_ITEM args)
{
    ui->partitonComboBox->setCurrentText(args.usnLogItem.partition);
    ui->mftLineEdit_2->setText(QString::number(args.usnLogItem.mft));
    ui->pMftLineEdit_3->setText(QString::number(args.usnLogItem.pMft));
    ui->reasonLineEdit_4->setText(QString::number(args.usnLogItem.reason));
    ui->attrLineEdit_5->setText(QString::number(args.usnLogItem.attributes));
    ui->fileNameLineEdit_6->setText(args.usnLogItem.fileName);
    ui->CaseInsensitiveCheckBox->setCheckState(args.CaseInsensitive == false ? Qt::Unchecked : Qt::Checked );
    ui->completeCheckBox->setCheckState(args.Complete == false ? Qt::Unchecked : Qt::Checked );
    ui->RegExpCheckBox->setCheckState(args.RegExp == false ? Qt::Unchecked : Qt::Checked );
    ui->commandTextEdit->setText(args.command);
    ui->execCheckBox->setCheckState(args.flag == false ? Qt::Unchecked : Qt::Checked );

}
//获取参数
S_MONITOR_ITEM UsnMonitorDialog::getArgs()
{
    S_MONITOR_ITEM args;
    bool isOk;
    args.usnLogItem.partition = ui->partitonComboBox->currentText();
    args.usnLogItem.mft = ui->mftLineEdit_2->text().toLongLong(&isOk);
    if(!isOk)
        args.usnLogItem.mft = -1;
    args.usnLogItem.pMft = ui->pMftLineEdit_3->text().toLongLong(&isOk);
    if(!isOk)
        args.usnLogItem.pMft = -1;
    args.usnLogItem.reason = ui->reasonLineEdit_4->text().toLongLong(&isOk);
    if(!isOk)
        args.usnLogItem.reason = -1;
    args.usnLogItem.attributes = ui->attrLineEdit_5->text().toLongLong(&isOk);
    if(!isOk)
        args.usnLogItem.attributes = -1;
    args.usnLogItem.fileName = ui->fileNameLineEdit_6->text().toUtf8();
    args.CaseInsensitive = ui->CaseInsensitiveCheckBox->checkState() == Qt::Checked ? true : false;
    args.Complete = ui->completeCheckBox->checkState() == Qt::Checked ? true : false;
    args.RegExp = ui->RegExpCheckBox->checkState() == Qt::Checked ? true : false;
    args.command = ui->commandTextEdit->toPlainText();
    args.flag = ui->execCheckBox->checkState() == Qt::Checked ? true : false;
    return args;
}
//清空参数
void UsnMonitorDialog::clearArgs()
{
    ui->partitonComboBox->setCurrentText("");
    ui->mftLineEdit_2->setText("-1");
    ui->pMftLineEdit_3->setText("-1");
    ui->reasonLineEdit_4->setText("-1");
    ui->attrLineEdit_5->setText("-1");
    ui->fileNameLineEdit_6->setText("");
    ui->commandTextEdit->setText("");
    ui->CaseInsensitiveCheckBox->setCheckState(Qt::Unchecked);
    ui->completeCheckBox->setCheckState(Qt::Unchecked);
    ui->RegExpCheckBox->setCheckState(Qt::Unchecked);
    ui->execCheckBox->setCheckState(Qt::Unchecked);
    m_ok = false;

}
bool  UsnMonitorDialog::isOk()
{
    return m_ok;
}
void UsnMonitorDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button == (QAbstractButton *)(ui->buttonBox->button(QDialogButtonBox::Ok))) {
        m_ok = true;
        //emit SignalUpdateWidget(setArgs());//更新界面
        //clearArgs();

    }  else {
        m_ok=false;
        //clearArgs();
    }
}

//正则完全匹配选中 前置取消选择
void UsnMonitorDialog::on_RegExpCheckBox_stateChanged(int arg1)
{
    if(arg1 ==  Qt::Checked) {
        QString fileName = ui->fileNameLineEdit_6->text();
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
void UsnMonitorDialog::on_completeCheckBox_stateChanged(int arg1)
{
    if(arg1 ==  Qt::Checked) {
        ui->RegExpCheckBox->blockSignals(true);
        ui->RegExpCheckBox->setCheckState(Qt::Unchecked);
        ui->RegExpCheckBox->blockSignals(false);

    }
}
//文件名框发生编辑操作
void UsnMonitorDialog::on_fileNameLineEdit_6_textEdited(const QString &arg1)
{
    if(ui->RegExpCheckBox->checkState() == Qt::Checked) {
        ui->RegExpCheckBox->blockSignals(true);
        ui->RegExpCheckBox->setCheckState(Qt::Unchecked);
        ui->RegExpCheckBox->blockSignals(false);
    }
}
