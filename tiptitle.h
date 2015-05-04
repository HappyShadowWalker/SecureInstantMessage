#ifndef TIPTITLE_H
#define TIPTITLE_H

#include "tipimgpushbtn.h"
#include <QWidget>
#include <QLabel>


class TipTitle : public QWidget
{
    Q_OBJECT
public:
    explicit TipTitle(QWidget *parent = 0);
    void setTitleText(QString title);

signals:
    void closeTitle();
protected:
    void paintEvent(QPaintEvent *);

public slots:
private:
    QLabel * titleLabel;
    TipImgPushBtn * btnClose;

};

#endif // TIPTITLE_H
