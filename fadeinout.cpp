#include "fadeinout.h"
//实现窗体的淡入和淡出效果
#include <QWidget>
fadeinout::fadeinout(QWidget* targetWidget,QObject *parent) :
    QObject(parent)
{
    this->target=targetWidget;
    opacityEffect=new QGraphicsOpacityEffect();
    target->setGraphicsEffect(opacityEffect);
    opacityEffect->setOpacity(0.0);

    //初始化定时器
    timerOpacity=new QTimer(this);
    connect(timerOpacity,SIGNAL(timeout()),this,SLOT(changeOpacity()));
    isWorking=false;

}

//开始淡入淡出

void fadeinout::startFadeInOut(int effectType){
    if(isWorking && effectType==this->effectType)
            return;

        this->effectType=effectType;
        opaLevel=1.0;
        if(effectType==FADEIN)
        {
            opaLevel=0.0;
        }
        timerOpacity->start(timerRate);
        isWorking=true;

}

void fadeinout::changeOpacity(){
    if(effectType==FADEIN)
        {
            if (opaLevel >= 1.0)
            {
                timerOpacity->stop();
                isWorking=false;
                return;
            }
            opacityEffect->setOpacity(opaLevel+=changeValue);
        }
        else
        {
            if (opaLevel <= 0.0)
            {
                timerOpacity->stop();
                isWorking=false;
                if(effectType==FADEOUT)
                    target->close();
                else if(effectType==FADEOUT_HIDE)
                    target->hide();
                else if(effectType==FADEOUT_EXIT)
                    exit(1);
                return;
            }
            opacityEffect->setOpacity(opaLevel-=changeValue);
        }

}
