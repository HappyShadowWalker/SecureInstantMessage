#ifndef SHAKEWINDOW_H
#define SHAKEWINDOW_H

#include <QObject>
#include <QWidget>
#include <QPoint>
#include <QTimer>//定时器
#define MaxShakedTimes 20
#define MaxShakeSpace 8
#define ShakeSpeed 30

class shakewindow : public QObject
{
    Q_OBJECT
public:
    explicit shakewindow(QWidget* shakeTarget,QObject *parent = 0);
    void startShake();
signals:

public slots:
    void timerOut();

private:
    QWidget* target;
    QTimer* shakeTimer;
    int position;
    QPoint curPos;

};

#endif // SHAKEWINDOW_H
