#include <QToolBar>
#include <QPainter>
#include <QToolButton>
#include "screenimgeditor.h"
#include <QPixmap>
#include <QMouseEvent>
#include <QButtonGroup>
#include <QApplication>
#include <QDebug>
#include <QDateTime>
#include <QFileDialog>
#include <QDesktopServices>
#include <QImageWriter>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QStandardPaths>
ScreenImgEditor::ScreenImgEditor(QPixmap img, QWidget *parent):QWidget(parent),image(img){
    resize(img.width(),img.height() + 40);
    setAttribute(Qt::WA_TranslucentBackground,true);
    setMouseTracking(true);
    tempImg = QImage(img.size(),QImage::Format_ARGB32_Premultiplied);
    tempImg.fill(Qt::transparent);
    bufferImg = tempImg;
    oprImages.push_back(tempImg);
    initToolBar();
}

void ScreenImgEditor::initToolBar(){
    toolBar = new QToolBar(this);
    QPixmap pix(50,50);
    QPainter painter(&pix);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    QIcon icon;

    QButtonGroup* group = new QButtonGroup;
    penBtn = new QToolButton;
    icon = QIcon(":/ScreenShot/img/ScreenShot/pen.png");
    penBtn->setIcon(icon);
    penBtn->setCursor(Qt::ArrowCursor);
    toolBar->addWidget(penBtn);
    penBtn->setCheckable(true);
    penBtn->setToolTip(tr("画笔"));
    group->addButton(penBtn);
    connect(penBtn,SIGNAL(clicked()),this,SLOT(curOptionChanged()));

    circleBtn = new QToolButton;
    painter.fillRect(pix.rect(),QColor(255,255,255,150));
    QPen pen = painter.pen();
    pen = painter.pen();
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawEllipse(5,5,pix.width() - 10,pix.height() - 10);
    icon = QIcon(pix);
    circleBtn->setIcon(icon);
    circleBtn->setCheckable(true);
    circleBtn->setCursor(Qt::ArrowCursor);
    circleBtn->setToolTip(tr("椭圆工具"));
    group->addButton(circleBtn);
    toolBar->addWidget(circleBtn);
    connect(circleBtn,SIGNAL(clicked()),this,SLOT(curOptionChanged()));

    rectBtn = new QToolButton;
    painter.fillRect(pix.rect(),QColor(255,255,255,150));
    painter.drawRect(8,8,pix.width() - 16,pix.height() - 16);
    icon = QIcon(pix);
    rectBtn->setIcon(icon);
    rectBtn->setCheckable(true);
    rectBtn->setCursor(Qt::ArrowCursor);
    rectBtn->setText(tr("矩形工具"));
    group->addButton(rectBtn);
    toolBar->addWidget(rectBtn);
    connect(rectBtn,SIGNAL(clicked()),this,SLOT(curOptionChanged()));

    undoBtn = new QToolButton;
    icon = QIcon(":/ScreenShot/img/ScreenShot/undo.png");
    undoBtn->setIcon(icon);
    undoBtn->setCursor(Qt::ArrowCursor);
    undoBtn->setToolTip(tr("撤销"));
    toolBar->addWidget(undoBtn);
    connect(undoBtn,SIGNAL(clicked()),this,SLOT(undo()));

//    saveAsBtn = new QToolButton;
//    icon = QIcon(":/ScreenShot/img/ScreenShot/saveas.png");
//    saveAsBtn->setIcon(icon);
//    saveAsBtn->setToolTip(tr("另存为"));
//    saveAsBtn->setCursor(Qt::ArrowCursor);
//    toolBar->addWidget(saveAsBtn);
//    connect(saveAsBtn,SIGNAL(clicked()),this,SLOT(saveAs()));

    cancelBtn = new QToolButton;
    icon = QIcon(":/ScreenShot/img/ScreenShot/cancel.png");
    cancelBtn->setIcon(icon);
    cancelBtn->setCursor(Qt::ArrowCursor);
    toolBar->addWidget(cancelBtn);
    cancelBtn->setToolTip(tr("取消"));
    connect(cancelBtn,SIGNAL(clicked()),this,SIGNAL(signalCloseScrennShot()));

    completeBtn = new QToolButton;
    icon = QIcon(":/ScreenShot/img/ScreenShot/complete.png");
    completeBtn->setIcon(icon);
    completeBtn->setToolTip(tr("保存"));
    completeBtn->setCursor(Qt::ArrowCursor);
    toolBar->addWidget(completeBtn);
    connect(completeBtn,SIGNAL(clicked()),this,SLOT(complete()));

    toolBar->move(0,image.height());
    setMinimumSize(250,image.height() + toolBar->height());
}

void ScreenImgEditor::moveEvent(QMoveEvent* e){
    if(frameGeometry().bottom() + toolBar->height() > qApp->desktop()->height()){
        resize(image.size());
        toolBar->move(0,image.height() - toolBar->height());
    }
}

void ScreenImgEditor::paint(){
    QPainter painter(&tempImg);
    painter.setRenderHint(QPainter::Antialiasing,true);
    QPen pen = painter.pen();
    pen.setWidth(2);
    pen.setColor(Qt::red);
    painter.setPen(pen);
    switch(curOption){
    case NoOption:
        break;
    case DrawFree:
        painter.drawLine(oldPos,curPos);
        break;
    case DrawRect:
        painter.drawRect(x,y,w,h);
        break;
    case DrawCircle:
       painter.drawEllipse(x,y,w,h);
       break;
    }
    update();
}

void ScreenImgEditor::mousePressEvent(QMouseEvent *e){

    if(e->button() == Qt::LeftButton){
        if(!image.rect().contains(e->pos())){
            QWidget::mousePressEvent(e);
            return;
        }
        bDrawing = true;
        x = e->pos().x();
        y = e->pos().y();
        curPos = e->pos();
        switch(curOption){
        case NoOption:break;
        case DrawRect:
        case DrawCircle:
            setCursor(Qt::CrossCursor);
            break;
        case DrawFree:
            QPixmap pix(":/ScreenShot/img/ScreenShot/pen.png");
            QCursor cursor(pix,0,pix.height());
            setCursor(cursor);
            break;
        }
    }else if(e->button() == Qt::RightButton){
       // qApp->quit();
        this->close();
    }

}

void ScreenImgEditor::mouseMoveEvent(QMouseEvent *e){
    if(e->buttons() & Qt::LeftButton){
        if(!image.rect().contains(e->pos())){
            QWidget::mouseMoveEvent(e);
            return;
        }
        switch(curOption){
        case DrawFree:
            oldPos = curPos;
            curPos = e->pos();
            break;
        case DrawRect:
        case DrawCircle:
            tempImg = bufferImg;
            w = e->pos().x() - x;
            h = e->pos().y() - y;
            break;
        case NoOption:
        default:
            break;
        }
        paint();
    }

}

void ScreenImgEditor::mouseReleaseEvent(QMouseEvent *e){
    if(e->button() == Qt::LeftButton){
        bDrawing = false;
        bufferImg = tempImg;
        w = 0;
        h = 0;
        oprImages.push_back(tempImg);
    }
}
void ScreenImgEditor::paintEvent(QPaintEvent *e){
    QPainter painter(this);
    painter.drawPixmap(0,0,image);
    if(bDrawing){
        painter.drawImage(0,0,tempImg);
    }else{
        painter.drawImage(0,0,bufferImg);
    }
}

void ScreenImgEditor::curOptionChanged(){
    if(sender() == penBtn){
        curOption = DrawFree;
        QPixmap pix(":/ScreenShot/img/ScreenShot/pen.png");
        QCursor cursor(pix,0,pix.height());
        setCursor(cursor);
    }else if(sender() == circleBtn){
        curOption = DrawCircle;
        setCursor(Qt::CrossCursor);
    }else if(sender() == rectBtn){
        curOption = DrawRect;
        setCursor(Qt::CrossCursor);
    }
}

void ScreenImgEditor::undo(){
    if(oprImages.size() <= 1) return;

    oprImages.pop_back();
    tempImg = oprImages.last();
    bufferImg = oprImages.last();
    update();
}

bool ScreenImgEditor::save(const QString& path){
    QPixmap pix = QPixmap();
    pix = QPixmap::grabWidget(this,image.rect());
    return pix.save(path);
}

//void ScreenImgEditor::saveAs(){
//    QString filter = "*.png;;*.jpg;;*.bmp;;*.gif;;*.jpeg";
//    QString dir = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).at(0) +QDir::separator() +  "grabbedImage.png";
//    QString path = QFileDialog::getSaveFileName(0,tr("Save"),dir,filter);
//    if(!path.isNull()){
//        if(!save(path)){
//            QMessageBox::warning(this,tr("failed to save"),tr("Sorry! Save failed!Please check savePath!"));
//        }else{
//            qApp->quit();
//        }
//    }
//}


void ScreenImgEditor::complete(){
    if(!save(QApplication::applicationDirPath() +QDir::separator() +  "截图" + QDir::separator() + QDateTime::currentDateTime().toString("yyMMddhhmmss") + ".png")){
        QMessageBox::warning(this,tr("failed to save"),tr("Sorry! Save failed!Please check savePath!"));
    }else{
        //qApp->quit();
        QMessageBox::information(this,"截图","截图已经成功自动保存到应用程序文件夹截图文件夹目录");
        this->close();
        emit signalCloseScrennShot();
    }
}
