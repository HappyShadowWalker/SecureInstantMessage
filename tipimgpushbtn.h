#ifndef TIPIMGPUSHBTN_H
#define TIPIMGPUSHBTN_H

#include <QPushButton>
//TipImgPushButton 是为了呈现用户登录和退出的提示消息而设计的
class TipImgPushBtn : public QPushButton
{
    Q_OBJECT
public:
    //TipImgPushBtn();
    explicit TipImgPushBtn(QPushButton* parent=0);
    explicit TipImgPushBtn(QString iconStr,QPushButton * parent=0);
private:
    void setBackGroundPalette(int transparency);

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
};

#endif // TIPIMGPUSHBTN_H
