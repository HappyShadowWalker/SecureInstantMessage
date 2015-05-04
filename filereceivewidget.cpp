#include "filereceivewidget.h"
#include "ui_filereceivewidget.h"

FileReceiveWidget::FileReceiveWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileReceiveWidget)
{
    ui->setupUi(this);
    ui->receiveFileProgressBar->setValue(0);
   // ui->denyPushBtn->setHidden(true);
    //ui->receivePushBtn->setHidden(true);
    ui->receiveStateLabel->setText("好友给你发送了一份文件");
    this->setWindowTitle("接收文件");
    connect(ui->denyPushBtn,SIGNAL(clicked()),this,SIGNAL(denyClicked()));
    connect(ui->receivePushBtn,SIGNAL(clicked()),this,SIGNAL(receiveClicked()));
}

FileReceiveWidget::~FileReceiveWidget()
{
    delete ui;
}

void FileReceiveWidget::setProgressBarMaxValue(int max){
    ui->receiveFileProgressBar->setMaximum(max);
}

void FileReceiveWidget::setProgressBarValue(int value){
    ui->receiveFileProgressBar->setValue(value);
}
