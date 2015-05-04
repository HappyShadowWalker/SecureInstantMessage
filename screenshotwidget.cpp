#include "screenshotwidget.h"
#include <QPainter>
#include <QDesktopWidget>
#include <QApplication>
#include <QMouseEvent>
#include <QImageWriter>
#include <QPaintEvent>
#include <QDebug>
#include <QMessageBox>
#include "screenimgeditor.h"
#include <QDir>
//把点击右键退出整个程序的Bug解决
ScreenShotWidget::ScreenShotWidget(QWidget *parent):QWidget(parent,Qt::FramelessWindowHint|Qt::Tool|Qt::WindowStaysOnTopHint){
    resize(qApp->desktop()->size());
    //调试输出DeskTop Size
    int deskTopHeight=qApp->desktop()->size().height();
    int deskTopWidth=qApp->desktop()->size().width();

    backGroundImg = QImage(size(),QImage::Format_ARGB32_Premultiplied);
    setAttribute(Qt::WA_TranslucentBackground,true);
    setCursor(Qt::CrossCursor);
    drawWindow();
    isInScreenShot = true;
    QDir dir = QApplication::applicationDirPath();
    bool have = false;
    foreach(QString filename,dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot)){
        if(filename == "截图"){
            have = true;
            break;
        }
    }
    if(!have){
        dir.mkdir("截图");
    }
//    QFile style(":/ScreenShot/img/ScreenShot/ScreenShot.qss");
//    if(!style.open(QFile::ReadOnly))
//        return;
//    qApp->setStyleSheet(style.readAll());
}


void ScreenShotWidget::paintEvent(QPaintEvent *e){
    QPainter painter(this);
    painter.drawImage(rect(),backGroundImg);
}

void ScreenShotWidget::drawWindow(){
    QPainter painter(&backGroundImg);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(backGroundImg.rect(),QColor(0,0,0,70));
    int penWidth = 2;
    QPen pen = painter.pen();
    pen.setWidth(penWidth);
    painter.setPen(pen);
    painter.fillRect(x,y,width,height,Qt::transparent);
    painter.drawRect(x - penWidth,y - penWidth,width + 2 * penWidth,height + 2 * penWidth);

    qDebug()<<x<<y<<width<<height;
    repaint();//即时强制重绘
}

void ScreenShotWidget::mouseMoveEvent(QMouseEvent *e){
    if(!isInScreenShot){
        QWidget::mouseMoveEvent(e);
        return;
    }
    if(e->buttons() & Qt::LeftButton){
        QPoint pos = e->pos();
        width = pos.x() - x;
        height = pos.y() - y;
        drawWindow();
    }
}

void ScreenShotWidget::mousePressEvent(QMouseEvent *e){
    if(e->button() == Qt::LeftButton){
        qDebug()<<e->pos();
        if(!isInScreenShot){
            QWidget::mousePressEvent(e);
            return;
        }
        x = e->pos().x();
        y = e->pos().y();
    }else if(e->button() == Qt::RightButton){
        //qApp->quit();
        this->close();
    }
}


void ScreenShotWidget::mouseReleaseEvent(QMouseEvent *e){
    if(!isInScreenShot){
        QWidget::mouseReleaseEvent(e);
        return;
    }
    if(e->button() == Qt::LeftButton){
        isInScreenShot = false;
        setCursor(Qt::ArrowCursor);
        WId id = qApp->desktop()->winId();
        QRect rect = QRect(x,y,width,height).normalized();
        //qDebug()<<rect.width();
        QPixmap pix = QPixmap();
        pix = QPixmap::grabWindow(id,rect.x(),rect.y(),rect.width(),rect.height());

        ScreenImgEditor* editor = new ScreenImgEditor(pix,this);
        connect(editor,SIGNAL(signalCloseScrennShot()),this,SLOT(close()));
        editor->move(x,y);
        editor->show();
    }
}
