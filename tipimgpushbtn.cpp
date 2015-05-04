#include "tipimgpushbtn.h"
#include <QPixmap>
#include <QPalette>
TipImgPushBtn::TipImgPushBtn(QPushButton *parent):QPushButton(parent){

}

TipImgPushBtn::TipImgPushBtn(QString imgPath, QPushButton *parent):QPushButton(parent){
    QPixmap pix=QPixmap(imgPath);
    this->setIcon(QIcon(pix));
    this->setIconSize(pix.size());
    this->resize(pix.size());
    this->setBackGroundPalette(0);//设置完全透明
    this->setFlat(true);
    this->setAutoFillBackground(true);

}

void TipImgPushBtn::setBackGroundPalette(int transparency){
    QPalette pal;
    pal.setBrush(QPalette::Button,QBrush(QColor(255,255,255,transparency)));
    setPalette(pal);
}

void TipImgPushBtn::enterEvent(QEvent *){
    setBackGroundPalette(120);
}

void TipImgPushBtn::leaveEvent(QEvent *){
    setBackGroundPalette(0);
}

void TipImgPushBtn::mousePressEvent(QMouseEvent *){
    setBackGroundPalette(150);
}

void TipImgPushBtn::mouseReleaseEvent(QMouseEvent *){
    setBackGroundPalette(120);
    emit clicked();
}
