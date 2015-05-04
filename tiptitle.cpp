#include "tiptitle.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QLinearGradient>//线性渐变
TipTitle::TipTitle(QWidget *parent) :
    QWidget(parent)
{
    titleLabel=new QLabel();
    btnClose=new TipImgPushBtn("://img/close.png");
    btnClose->setToolTip("关闭");
    connect(btnClose,SIGNAL(clicked()),this,SIGNAL(closeTitle()));

    QHBoxLayout *layout=new QHBoxLayout;
    layout->addWidget(titleLabel,0,Qt::AlignLeft);
    layout->addStretch();
    layout->addWidget(btnClose,0,Qt::AlignRight);
    layout->setMargin(0);
    setLayout(layout);
    setFixedHeight(22);

}

void TipTitle::setTitleText(QString title){
    titleLabel->setText(title);
}

void TipTitle::paintEvent(QPaintEvent *){
    QLinearGradient linear(rect().topLeft(),rect().bottomRight());
    linear.setColorAt(0,QColor(227,207,87));
    linear.setColorAt(0.5,QColor(245,222,179));
    linear.setColorAt(1,QColor(189,252,201));

    QPainter painter(this);
    painter.setBrush(QBrush(linear));
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());
}
