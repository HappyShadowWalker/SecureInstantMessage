#ifndef SCREENIMGEDITOR_H
#define SCREENIMGEDITOR_H

#include <QWidget>
#include <QStack>
#include <QToolButton>
#include <QPixmap>
#include <QToolBar>
#include <QPainter>
#include <QPen>
class QToolButton;
class QToolBar;
enum Option{NoOption,DrawFree,DrawCircle,DrawRect};
//实现截图的编辑
class ScreenImgEditor : public QWidget
{
    Q_OBJECT
public:
    explicit ScreenImgEditor(QPixmap img ,QWidget *parent = 0);

signals:
    //定义一个窗体关闭信号告诉ScreenShot Widget 关闭
    void signalCloseScrennShot();
protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void moveEvent(QMoveEvent *);
public slots:
private:
    QToolBar* toolBar;
   // QToolButton* saveAsBtn;
    QToolButton* cancelBtn;
    QToolButton* completeBtn;
    QToolButton* circleBtn;
    QToolButton* rectBtn;
    QToolButton* undoBtn;
    QToolButton* textBtn;
    QToolButton* penBtn;

    Option curOption;
    QPixmap image;
    QImage tempImg;
    QImage bufferImg;
    QStack<QImage> oprImages;
    bool bDrawing;


    int x;
    int y;
    int w;
    int h;
    QPoint curPos;
    QPoint oldPos;

    void initToolBar();
    void paint();
    bool save(const QString& path);
private slots:
    void curOptionChanged();
    void undo();
    //void saveAs();
    void complete();


};

#endif // SCREENIMGEDITOR_H
