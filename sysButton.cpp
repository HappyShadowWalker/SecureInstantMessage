#include "sysButton.h"
#include <QPainter>

sysButton::sysButton(QString picName, QString tipInfo, QWidget *parent):QPushButton(parent){
    pixMap.load(":/img/sysButton/"+picName);
    this->setWindowOpacity(0);
    this->setFlat(true);
    btnWidth=pixMap.width()/4;
    btnHeight=pixMap.height();
    this->setFixedSize(btnWidth,btnHeight);
    this->setToolTip(tipInfo);
    this->btnStates=NORMAL;


}

void sysButton::enterEvent(QEvent *){
    btnStates=ENTER;
    update();
}

void sysButton::mousePressEvent(QMouseEvent *e){
    btnStates=PRESS;
    update();
}

void sysButton::mouseReleaseEvent(QMouseEvent *e){
    btnStates=ENTER;
    update();
    emit clicked();
}

void sysButton::leaveEvent(QEvent *){
    btnStates=NORMAL;
    update();
}

void sysButton::paintEvent(QPaintEvent *){
    QPainter painter(this);
    painter.drawPixmap(rect(),pixMap.copy(btnWidth*btnStates,0,btnWidth,btnHeight));

}
