#ifndef SKINPUSHBUTTON_H
#define SKINPUSHBUTTON_H

#include <QPushButton>
#include <QPixmap>

class skinPushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit skinPushButton(QString picName,QString tipInfo,QWidget* parent=0);
private:
    QPixmap pixMap;
    int iconWidth;
    int iconHeight;

protected:
    void enterEvent(QEvent *);
    void mousePressEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent *e);
    void leaveEvent(QEvent *);
};

#endif // SKINPUSHBUTTON_H
