#ifndef SCREENSHOTWIDGET_H
#define SCREENSHOTWIDGET_H

#include <QWidget>
#include <QStack>
#include <QComboBox>
#include <QPushButton>
#include <QImage>

class ScreenShotWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ScreenShotWidget(QWidget *parent = 0);

signals:
protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
    void mouseReleaseEvent(QMouseEvent *);
public slots:
private:
    int x;
    int y;
    int width;
    int height;
    bool isInScreenShot;
    QImage backGroundImg;
    void drawWindow();
    void setAppStyle(const QString& path);

};

#endif // SCREENSHOTWIDGET_H
