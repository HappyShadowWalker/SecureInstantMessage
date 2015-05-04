#include "shakewindow.h"

shakewindow::shakewindow(QWidget* target,QObject *parent) :
    QObject(parent)
{
    this->target=target;
    shakeTimer=new QTimer(this);
    connect(shakeTimer,SIGNAL(timeout()),this,SLOT(timerOut()));
    position=MaxShakedTimes;

}

void shakewindow::startShake(){
    //如果正在抖动则返回
    if(position < MaxShakedTimes)
        return;
    position=1;
    curPos=this->target->pos();
    shakeTimer->start(ShakeSpeed);

}

void shakewindow::timerOut(){
    //还没有抖动完毕
    if(position < MaxShakedTimes)
    {
        ++position;
        switch(position%4)
        {
        case 1:
        {
            QPoint tmpPos(curPos.x(),curPos.y()-MaxShakeSpace);
            this->target->move(tmpPos);
        }
            break;
        case 2:
        {
            QPoint tmpPos(curPos.x()-MaxShakeSpace,curPos.y()-MaxShakeSpace);
            this->target->move(tmpPos);
        }
            break;
        case 3:
        {
            QPoint tmpPos(curPos.x()-MaxShakeSpace,curPos.y());
            this->target->move(tmpPos);
        }
            break;
        case 0:
        default:
            this->target->move(curPos);
            break;
        }
    }
    //抖动完毕，关闭定时器
    else
    {
        shakeTimer->stop();
    }


}
