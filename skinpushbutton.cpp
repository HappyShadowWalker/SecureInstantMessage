#include "skinpushbutton.h"
#include <QIcon>
skinPushButton::skinPushButton(QString picName,QString tipInfo,QWidget* parent):QPushButton(parent)
{

    this->setToolTip(tipInfo);
    this->setFlat(true);
    this->setWindowOpacity(0);
    pixMap.load(":/img/sysButton/"+picName);
    iconWidth=pixMap.width()/3;
    iconHeight=pixMap.height();

    this->setIcon(QIcon(pixMap.copy(0,0,iconWidth,iconHeight)));
    this->setIconSize(QSize(iconWidth,iconHeight));

}

void skinPushButton::enterEvent(QEvent *){
    this->setWindowOpacity(0.1);
    this->setIcon(QIcon(pixMap.copy(iconWidth,0,iconWidth,iconHeight)));
}

void skinPushButton::mousePressEvent(QMouseEvent *e){
    this->setWindowOpacity(0.3);
    this->setIcon(QIcon(pixMap.copy(iconWidth*2,0,iconWidth,iconHeight)));
}

void skinPushButton::leaveEvent(QEvent *){
    this->setWindowOpacity(0);
    this->setIcon(QIcon(pixMap.copy(0,0,iconWidth,iconHeight)));
}

void skinPushButton::mouseReleaseEvent(QMouseEvent *e){
    this->setWindowOpacity(0.1);
    this->setIcon(QIcon(pixMap.copy(iconWidth,0,iconWidth,iconHeight)));
    emit clicked();
}
