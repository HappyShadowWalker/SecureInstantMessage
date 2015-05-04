#include "useritem.h"
#include "ui_useritem.h"
#include <QPixmap>

UserItem::UserItem(QString userAcc,QString userSignedName,QString userImgPath,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserItem)
{
    ui->setupUi(this);
    this->setFixedSize(270,50);
    this->userAcc=userAcc;
    QPixmap headPix(userImgPath);
    ui->headImgPushBtn->setIcon(QIcon(headPix));
    ui->headImgPushBtn->setIconSize(QSize(30,30));
    ui->friendAccLabel->setText(userAcc);
    ui->signedNameLabel->setText(userSignedName);
  //  ui->signedNameLabel->setText(userAcc+"说:"+userSignedName);

}

UserItem::~UserItem()
{
    delete ui;
}

QString UserItem::getUserAcc(){
    return userAcc;
}
