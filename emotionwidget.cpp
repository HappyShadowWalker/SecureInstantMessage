#include "emotionwidget.h"
#include <QDebug>
#include <QIcon>
#include<QByteArray>
#include <QPixmap>
EmotionWidget::EmotionWidget(QWidget *parent) :
    QWidget(parent)
{
    readEmotionPathFromFile("://EmotionPath.txt");
    emotionTipPaths<<"调皮"<<"劝告"<<"好色"<< "生气"<<"好蠢"<< "坏的"<<"打败"<<"愚蠢"
                   << "瞥眼"<<"拜拜"<< "平静"<< "鼓掌"<< "可爱"<<"跳舞"<<"白日梦"<<"邪恶"
                   <<"失望"<<"小狗"<<"不知道"<<"吃货"<<"尴尬"<<"抓狂"<< "走开"<<"非常好"<<"握手"
                   << "大笑"<<"爱心"<<"听歌"<<"筋疲力尽"<<"微笑";
    signalMap=new QSignalMapper(this);
    gridLayout=new QGridLayout();
    gridLayout->setSpacing(0);
    int row=0,col=0;
    for(int i=0;i<emotionImgPaths.size();i++){
        btnEmotion=new QPushButton();
        QPixmap pix;
        QString eachImgPath=emotionImgPaths[i].left(emotionImgPaths[i].size()-1);
        pix.load(eachImgPath);
        QIcon *btnIcon=new QIcon(pix);
        btnEmotion->setIcon(*btnIcon);
        btnEmotion->setIconSize(QSize(24,24));
        btnEmotion->setToolTip(emotionTipPaths[i]);
        connect(btnEmotion,SIGNAL(clicked()),signalMap,SLOT(map()));
        connect(btnEmotion,SIGNAL(clicked()),this,SIGNAL(emotionClick()));
        signalMap->setMapping(btnEmotion,emotionImgPaths[i]);
        if(i%10==0){
            row++;
            col=0;
        }
        gridLayout->addWidget(btnEmotion,row,col++,1,1,0);

    }
    connect(signalMap,SIGNAL(mapped(QString)),this,SIGNAL(emotionTrigger(QString)));
    this->setLayout(gridLayout);
    //this->setWindowFlags(Qt::Popup);


}

void EmotionWidget::readEmotionPathFromFile(QString emotionFilePath){
    QFile emotionFile(emotionFilePath);
    if(!emotionFile.open(QIODevice::ReadOnly|QIODevice::Text)){
        qDebug("Can't open emotionFile");
    }
    while(!emotionFile.atEnd()) {
        QByteArray line = emotionFile.readLine();
        QString eachEmotionPath(line);
        emotionImgPaths.append(eachEmotionPath);

    }
    emotionFile.close();

}
