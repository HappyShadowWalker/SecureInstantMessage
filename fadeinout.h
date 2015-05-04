#ifndef FADEINOUT_H
#define FADEINOUT_H

#include <QObject>
#include <QTimer>
#include <QGraphicsOpacityEffect>

//
#define FADEIN 0
#define FADEOUT 1
#define FADEOUT_EXIT 2
#define FADEOUT_HIDE 4
//每次改变透明度的时间间隔
const int timerRate=30;
//每次改变的透明度值
const double changeValue=0.05;

class fadeinout : public QObject
{
    Q_OBJECT
public:
    explicit fadeinout(QWidget* targetWidget,QObject *parent = 0);
    void startFadeInOut(int effectType);

signals:

private slots:
    void changeOpacity();

private:
    QTimer* timerOpacity;
    double opaLevel;
    QGraphicsOpacityEffect* opacityEffect;
    int effectType;
    QWidget* target;
    bool isWorking;

};

#endif // FADEINOUT_H
