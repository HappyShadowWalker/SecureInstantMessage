#include "statechangewidget.h"
#include <QVBoxLayout>
#include <QDesktopWidget>
#include <QApplication>
#include <QBitmap>
#include <QPainter>
StateChangeWidget::StateChangeWidget(QString userName,QWidget *parent) :
    QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    tipTitle=new TipTitle();
    connect(tipTitle,SIGNAL(closeTitle()),this,SLOT(close()));

    contentLabel=new QLabel();
    contentLabel->setWordWrap(true);
    contentLabel->setAlignment(Qt::AlignTop);
    contentLabel->setFixedSize(300,100);

    btnImg=new TipImgPushBtn("://img/look.png");
    //当点击查看按钮之后,发送消息查看的信号
    connect(btnImg,SIGNAL(clicked()),this,SIGNAL(emitLook()));
    QVBoxLayout*mainLayout=new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->addWidget(tipTitle);
    mainLayout->addWidget(contentLabel);
    contentLabel->setMargin(5);
    mainLayout->addWidget(btnImg,0,Qt::AlignRight);
    setLayout(mainLayout);

    setFixedSize(sizeHint().width(),sizeHint().height());

    smoothShowTimer=new QTimer(this);
    smoothStayTimer=new QTimer(this);
    smoothCloseTimer=new QTimer(this);
    connect(smoothShowTimer,SIGNAL(timeout()),this,SLOT(smoothMove()));
    connect(smoothStayTimer,SIGNAL(timeout()),this,SLOT(smoothStay()));
    connect(smoothCloseTimer,SIGNAL(timeout()),this,SLOT(smoothClose()));


}

void StateChangeWidget::paintEvent(QPaintEvent *){
    QBitmap bitmap(this->size());
    bitmap.fill(Qt::white);
    QPainter painter(this);
    painter.setBrush(QBrush(QColor(250,240,230)));
    painter.setPen(QPen(QBrush(QColor(255,222,173)),4));
    painter.drawRoundedRect(bitmap.rect(),5,5);
    setMask(bitmap);
}

void StateChangeWidget::showInLowLeft(){
    QDesktopWidget* deskTop=QApplication::desktop();
    deskRect=deskTop->availableGeometry();
    topLeftPoint.setX(deskRect.width()-rect().width());
    topLeftPoint.setY(deskRect.height()-rect().height());
    this->move(topLeftPoint.x(),767);
    this->show();
    smoothShowTimer->start(5);

}

void StateChangeWidget::smoothMove(){
    static int beginY=QApplication::desktop()->height();
    beginY--;
    move(topLeftPoint.x(),beginY);
    if(beginY<=topLeftPoint.y())
    {
        smoothShowTimer->stop();
        smoothStayTimer->start(1000);

    }
}

void StateChangeWidget::smoothStay(){
    static int timeCount=0;//必须有static 关键字
    timeCount++;
    if(timeCount>=9)
    {
        smoothStayTimer->stop();
        smoothCloseTimer->start(200);
    }

}

void StateChangeWidget::smoothClose(){
    static double tran=1.0;
    tran-=0.1;
    if(tran<=0.0)
    {
        smoothCloseTimer->stop();
        //emit close();
        this->close();

    }
    else
        setWindowOpacity(tran);
}

void StateChangeWidget::showTipMsg(QString title, QString content){
    tipTitle->setTitleText(title);
    contentLabel->setText(content);
    showInLowLeft();
}
