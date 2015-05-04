#ifndef STATECHANGEWIDGET_H
#define STATECHANGEWIDGET_H

#include <QWidget>
#include <QTimer>
#include "tipimgpushbtn.h"
#include "tiptitle.h"

//当用户登入或退出时显示提示消息的Widget
class StateChangeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StateChangeWidget(QString userName,QWidget *parent = 0);
    void showInLowLeft();//在右下角显示

signals:

protected:
    void paintEvent(QPaintEvent *);

public slots:
    void showTipMsg(QString title,QString content);
private slots:
    void smoothMove();
    void smoothStay();
    void smoothClose();
signals:
    void emitLook();

private:
    TipTitle* tipTitle;
    QLabel* contentLabel;
    TipImgPushBtn * btnImg;
    QPoint topLeftPoint;
    QRect deskRect;

    QTimer* smoothShowTimer;
    QTimer* smoothStayTimer;
    QTimer* smoothCloseTimer;

};

#endif // STATECHANGEWIDGET_H
