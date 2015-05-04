#ifndef SYSBUTTON_H
#define SYSBUTTON_H
#include <QString>
#include <QPushButton>
#include <QWidget>
#include <QPixmap>

class sysButton:public QPushButton{

    Q_OBJECT
public:
    explicit sysButton(QString picName,QString tipInfo,QWidget* parent=0);
private:
    enum ButtonStates{NORMAL,ENTER,PRESS,NOSTATUS};
    ButtonStates btnStates;
    QPixmap pixMap;
    int btnWidth;
    int btnHeight;
protected:
    void paintEvent(QPaintEvent*);
    void enterEvent(QEvent*);
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void leaveEvent(QEvent*);


};
#endif // SYSBUTTON_H
