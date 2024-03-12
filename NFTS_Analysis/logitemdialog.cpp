#include "logitemdialog.h"
#include "ui_logitemdialog.h"

LogItemDialog::LogItemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogItemDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
    // 去掉问号和左上角图标，只保留关闭
    this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

    //正则验证器，只能输入十进制整数
    QRegExp rE("[0-9]+");
    QRegExpValidator *regs = new QRegExpValidator(rE, this);
    ui->MftNumbeLineEdit->setValidator(regs);
    ui->PMftNumbeLineEdit->setValidator(regs);

}

LogItemDialog::~LogItemDialog()
{
    delete ui;
}

void LogItemDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    S_USN_LOG_ITEM item;
    if(button == (QAbstractButton *)(ui->buttonBox->button(QDialogButtonBox::Ok)))
    {
        item.uuid = QUuid::createUuid();
        bool isSend = false;
        bool isOk;
        //1.盘符
        if(ui->partitionComboBox->currentText().isEmpty())
            item.partitionSerial = -1;
        else
        {
            item.partitionSerial = ui->partitionComboBox->currentText().split(QChar(' ')).last().toInt();
        }
        //2.mft

        item.mft = ui->MftNumbeLineEdit->text().toULongLong(&isOk);
        if(!isOk)
            item.mft = -1;
        //3.pmft
        item.pMft = ui->PMftNumbeLineEdit->text().toULongLong(&isOk);
        if(!isOk)
            item.pMft = -1;
        //4.文件名
        if(!ui->FileNameLineEdit->text().isEmpty())
            isSend = true;
        item.fileName = ui->FileNameLineEdit->text();
        //5.区分大小写
        item.CaseInsensitive = ui->CaseInsensitiveCheckBox->checkState() == Qt::Checked ? true : false;
        //6.使用正则
        item.RegExp = ui->RegExpCheckBox->checkState() == Qt::Checked ? true : false;
        //7.cmd指令
        if(!ui->CMDCommandlineEdit->text().isEmpty())
            isSend = true;
        item.cmdCommand = ui->CMDCommandlineEdit->text();

        if(item.partitionSerial != -1 || item.mft != -1 || item.pMft != -1 || isSend)
            emit SignalUsnLogItem(item);
    }
}
