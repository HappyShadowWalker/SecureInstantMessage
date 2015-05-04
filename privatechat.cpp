#include "privatechat.h"
#include <QPalette>
#include <QBitmap>
#include <QPainter>
#include <QBrush>
#include <QPixmap>
#include <QMouseEvent>
#include <QByteArray>
#include <QDataStream>
#include <QWebFrame>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include "sharedefine.h"
#include <QFile>
#include "ui_privatechat.h"
#include "screenshotwidget.h"
#include <QPropertyAnimation>
#include <QFontDatabase>

privatechat::privatechat(QString targetAcc,QHostAddress targetIp,QString targetImgPath,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::privatechat)
{
    ui->setupUi(this);
    beautifyMainWidget();
    addTooBtns();
    this->targetAcc=targetAcc;
    this->targetIp=targetIp;
    this->targetImgPath=targetImgPath;
    iniPrivateChatSetting();
    ui->msgTextEdit->setAcceptDrops(false);
    this->setAcceptDrops(true);

    //实现消息通信
    msgReceiveUdpSocket=new QUdpSocket();
    msgSendUdpSocket=new QUdpSocket();
    bool udpBindResult=msgReceiveUdpSocket->bind(QHostAddress::LocalHost,PrivateChatPort);
    if(!udpBindResult){

    }
    connect(msgReceiveUdpSocket,SIGNAL(readyRead()),this,SLOT(receiveMsg()));//处理接收消息
    //得到当前聊天对方的IP地址
    this->targetIp=targetIp;

    connect(ui->sendPushBtn,SIGNAL(clicked()),this,SLOT(btnSendClick()));
    ui->msgTextEdit->installEventFilter(this);
    //设置WebView样式
    QFile webviewFile("://img/WebviewStyle/MsgShowStyle.html");
    webviewFile.open(QIODevice::ReadOnly);
    ui->msgShowWebView->setHtml(QString::fromUtf8(webviewFile.readAll().constData()));
    webviewFile.close();

    //实现文件发送功能
    loadSize=4*1024;
    totalBytes=0;
    bytesWritten=0;
    bytesToWrite=0;
    ui->sendFileProgressBar->setValue(0);
    this->setWindowTitle("好友聊天");

    changePrivateChatStyle();
    IniVoiceChatSetting();

    //设置发送文件的进度条和状态信息隐藏
    ui->sendFileStateLabel->setHidden(true);
    ui->sendFileProgressBar->setHidden(true);
}

privatechat::~privatechat()
{
    delete ui;
}

void privatechat::beautifyMainWidget(){
    QPalette mainWidgetPalette;
    mainWidgetPalette.setBrush(QPalette::Background,QBrush(QPixmap(":/img/privatechat.jpg")));
    this->setPalette(mainWidgetPalette);
    this->setAutoFillBackground(true);
    this->setWindowFlags(Qt::FramelessWindowHint);
    QBitmap mainWidgetBitmap(this->size());
    mainWidgetBitmap.fill();
    QPainter mainWidgetPainter(&mainWidgetBitmap);
    mainWidgetPainter.setRenderHint(QPainter::Antialiasing);
    int arcR = 10;    //弧度
    QRect rect = this->rect();
    QPainterPath path;
    //逆时针
    path.moveTo(arcR, 0);
    path.arcTo(0, 0, arcR * 2, arcR * 2, 90.0f, 90.0f);
    path.lineTo(0, rect.height());
    path.lineTo(rect.width(), rect.height());
    path.lineTo(rect.width(), arcR);
    path.arcTo(rect.width() - arcR * 2, 0, arcR * 2, arcR * 2, 0.0f, 90.0f);
    path.lineTo(arcR, 0);
    mainWidgetPainter.drawPath(path);
    mainWidgetPainter.fillPath(path, QBrush(Qt::red));
    setMask(mainWidgetBitmap);

}

void privatechat::mousePressEvent(QMouseEvent * e){
    windowPos=this->pos();
    mousePos=e->globalPos();
    moveDisPos=mousePos-windowPos;
}

void privatechat::mouseMoveEvent(QMouseEvent * e){
    this->move(e->globalPos() - this->moveDisPos);
}

void privatechat::addTooBtns(){
    btnClose=new sysButton("sys_button_close.png","关闭");
    btnMin=new sysButton("sys_button_min.png","最小化");
    ui->mainToolLayout->addStretch();
    //
    connect(btnClose,SIGNAL(clicked()),this,SLOT(closeMainWidget()));
    connect(btnMin,SIGNAL(clicked()),this,SLOT(minMainWidget()));
    ui->mainToolLayout->addWidget(btnMin);
    ui->mainToolLayout->addWidget(btnClose);
    ui->mainToolLayout->setSpacing(0);
    ui->mainToolLayout->setContentsMargins(0,0,0,0);

}

void privatechat::closeMainWidget(){
    //this->close();
//    fadeInOutWidget=new fadeinout(this);
//    fadeInOutWidget->startFadeInOut(FADEOUT_EXIT);
    //以渐隐方式退出聊天窗口
    //界面动画，改变透明度的方式消失1 - 0渐变
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->start();
    connect(animation, SIGNAL(finished()), this, SLOT(close()));

}

void privatechat::minMainWidget(){
    this->showMinimized();
}

//实现消息接收函数
void privatechat::receiveMsg(){
    //以循环方式接收消息
    while(msgReceiveUdpSocket->hasPendingDatagrams()){
        QByteArray bufferData;
        bufferData.resize(msgReceiveUdpSocket->pendingDatagramSize());
        msgReceiveUdpSocket->readDatagram(bufferData.data(),bufferData.size());
        QByteArray aesKey=aesCrypto.HexStringToByte(keyString);
        QByteArray decryptedData=aesCrypto.Decrypt(bufferData,aesKey);
        QDataStream inData(&decryptedData,QIODevice::ReadOnly);
        QString senderAcc;
        int msgType;
        inData>>msgType;
        QString msgContent;
        switch(msgType){
        case Message:{
            inData>>senderAcc;
            inData>>msgContent;
            QString html = QString("document.getElementById(\"content\").insertAdjacentHTML(\"beforeEnd\",\"<div style='overflow:hidden;'><p class='triangle-left left'>%1</p></div>\")").arg(msgContent);
            ui->msgShowWebView->page()->mainFrame()->evaluateJavaScript(html);

        }
        break;
        case SENDFILE:{
            inData>>senderAcc;
            inData>>msgContent;
            if(msgContent=="AccFile"){
                if(!fileName.isEmpty()){
                    fileSendSocket=new QTcpSocket(this);
                    connect(fileSendSocket,SIGNAL(connected()),this,SLOT(startSendFile()));
                    connect(fileSendSocket,SIGNAL(bytesWritten(qint64)),this,SLOT(updateFileSendProgress(qint64)));
                    connect(fileSendSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
                    fileSendSocket->connectToHost(QHostAddress::LocalHost,SendFileDataPort);

                    //在界面上展现状态信息
                    ui->sendFileProgressBar->setHidden(false);
                    ui->sendFileStateLabel->setHidden(false);
                    ui->sendFileStateLabel->setText(QString("开始发送%1文件").arg(fileName));

                }

            }
            else if(msgContent=="DenyFile"){
                //QMessageBox::critical(this,"拒绝接收文件","你的好友拒绝了你发送的文件");
                //在界面上展现状态信息
               // ui->sendFileProgressBar->setHidden(false);
                ui->sendFileStateLabel->setHidden(false);
                ui->sendFileStateLabel->setText(QString("对方拒绝接收%1文件").arg(fileName));

            }

        }
        break;
        case Shake:{
            //接收到窗口抖动，抖动自己的窗口
            shake=new shakewindow(this);
            shake->startShake();
           // QMessageBox::about(this,"Shake","Shake");

        }
        break;
        }
    }

}

//实现消息发送
void privatechat::sendMsg(MessageType msgType){
    switch(msgType){
    case Message:
    {
        if(ui->msgTextEdit->toPlainText()==""){
            QMessageBox::warning(this,"消息为空","发送消息不能为空");
        }
        //保存自己的当前账号
        //QString curAccount="ShadowWalker";
        QString plainMsg=ui->msgTextEdit->toPlainText();
        plainMsg=formateSendMsgToShow(plainMsg);
        QString htmlMSG="";
        htmlMSG.append(QString("document.getElementById(\"content\").insertAdjacentHTML(\"beforeEnd\",\"<div style='overflow:hidden;'><p class='triangle-right right'>%1</p></div>\")").arg(plainMsg));
        ui->msgShowWebView->page()->mainFrame()->evaluateJavaScript(htmlMSG);
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out<<Message<<curLoginAcc<<plainMsg;
        //测试先发送数据到本机端口
        QByteArray aesKey=aesCrypto.HexStringToByte(keyString);
        QByteArray encryptData=aesCrypto.Encrypt(data,aesKey);
        msgSendUdpSocket->writeDatagram(encryptData,targetIp,BroadcastPort);
        msgSendUdpSocket->writeDatagram(encryptData,targetIp,PrivateChatPort);

    }
    break;
    case Shake:
    {
        //发送窗口抖动
        QByteArray data;
        QString curAccount="ShadowWalker";
        QDataStream out(&data, QIODevice::WriteOnly);
        QString shakeContent="ShakeRequest";
        out<<Shake<<curAccount<<shakeContent;
        //测试先发送数据到本机端口
        QByteArray aesKey=aesCrypto.HexStringToByte(keyString);
        QByteArray encryptData=aesCrypto.Encrypt(data,aesKey);
        msgSendUdpSocket->writeDatagram(encryptData,targetIp,PrivateChatPort);

    }
    break;
    case SENDFILE:
    {
        //发送文件请求代码 在这里我自己定义三个特征码分别为SendFile AccFile DenyFile
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        QString msgSendContenet="SendFile";
        out<<SENDFILE<<curLoginAcc<<msgSendContenet;
        //测试先发送数据到本机端口
        QByteArray aesKey=aesCrypto.HexStringToByte(keyString);
        QByteArray encryptData=aesCrypto.Encrypt(data,aesKey);
        msgSendUdpSocket->writeDatagram(encryptData,targetIp,BroadcastPort);//一个是广播端口,一个是私聊端口
        msgSendUdpSocket->writeDatagram(encryptData,targetIp,PrivateChatPort);

    }
     break;
    }

}

void privatechat::btnSendClick(){
    sendMsg(Message);
}

//实现Ctrl +Enter 键发送消息
bool privatechat::eventFilter(QObject * targetObj, QEvent * e){
    Q_ASSERT(targetObj==ui->msgTextEdit);
    if(e->type()==QEvent::KeyPress){
        QKeyEvent* event=static_cast<QKeyEvent*>(e);
        if(event->key()==Qt::Key_Return&&(event->modifiers()==Qt::ControlModifier)){
            sendMsg(Message);
            return true;
        }
    }
    return QWidget::eventFilter(targetObj,e);
}

//实现聊天窗口的抖动
void privatechat::on_shakeToolBtn_clicked()
{
    shake=new shakewindow(this);
    shake->startShake();
    //发送窗口抖动
    sendMsg(Shake);
}

void privatechat::on_fontComboBox_currentFontChanged(const QFont &f)
{
    currentFont=f;
    ui->msgTextEdit->setFont(f);
}

void privatechat::on_blackToolBtn_clicked()
{
    isBlack=!isBlack;
    if(isBlack){
        ui->msgTextEdit->setFontWeight(QFont::Black);
    }
}

void privatechat::on_inclineToolBtn_clicked()
{
    isIncline=!isIncline;
    ui->msgTextEdit->setFontItalic(isIncline);
}

void privatechat::on_snapScreenToolBtn_clicked()
{
    ScreenShotWidget * screenShotWidget=new ScreenShotWidget();
    screenShotWidget->show();
}

//表情框响应函数
void privatechat::insertEmotion(QString emotionImgPath){
    // ui->msgTextEdit->textCursor().insertImage(emotionImgPath.left(emotionImgPath.size()-1));//图片路径多了个换行符
    QString trueEmotionImgPath=emotionImgPath.left(emotionImgPath.size()-1);
    int index=trueEmotionImgPath.lastIndexOf("/");
    QString EmotionID=trueEmotionImgPath.right(trueEmotionImgPath.size()-index-1);
    QString EmotionText="["+EmotionID+"]";
    //ui->msgTextEdit->append(EmotionText);
    ui->msgTextEdit->textCursor().insertText(EmotionText);

}

void privatechat::on_emotionToolBtn_clicked()
{
        emotionWidget=new EmotionWidget;
        //设置emotionWidget的位置
        QPoint emotionBtnGP=ui->emotionToolBtn->mapToGlobal(QPoint(0,0));
        int emotionBtnGPX=emotionBtnGP.x();
        int emotionBtnGPY=emotionBtnGP.y();
        int trueX=emotionBtnGPX-120;
        int trueY=emotionBtnGPY-200;
        emotionWidget->setGeometry(trueX,trueY,400,120);
        emotionWidget->show();
        connect(emotionWidget,SIGNAL(emotionTrigger(QString)),this,SLOT(insertEmotion(QString)));
        connect(emotionWidget,SIGNAL(emotionClick()),this,SLOT(closeEmotionWidget()));


}

void privatechat::closeEmotionWidget(){
    emotionWidget->close();
    delete emotionWidget;
}

//实现文件发送按钮响应
void privatechat::on_sendFileBtn_clicked()
{
    fileName=QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty()){
        sendMsg(SENDFILE);

    }

    //如果打开的文件名不为空就执行发送,在这里文件名应该包含路径信息
//    if(!fileName.isEmpty()){
//        fileSendSocket=new QTcpSocket(this);
//        connect(fileSendSocket,SIGNAL(connected()),this,SLOT(startSendFile()));
//        connect(fileSendSocket,SIGNAL(bytesWritten(qint64)),this,SLOT(updateFileSendProgress(qint64)));
//        connect(fileSendSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
//        fileSendSocket->connectToHost(QHostAddress::LocalHost,6666);
//    }
}

//实现文件大小等信息的发送
void privatechat::startSendFile(){
    localFile=new QFile(fileName);
    if(!localFile->open(QFile::ReadOnly)){
        //QDebug("open send file error");
        return ;
    }
    totalBytes=localFile->size();
    QDataStream sendout(&outBlock,QIODevice::WriteOnly);
    sendout.setVersion(QDataStream::Qt_5_2);
    QString currentFileName=fileName.right(fileName.size() - fileName.lastIndexOf('/')-1);
    sendout << qint64(0) << qint64(0) << currentFileName;
    totalBytes+=outBlock.size();
    sendout.device()->seek(0);
    sendout<<totalBytes<<(qint64)(outBlock.size()-sizeof(qint64)*2);
    bytesToWrite=totalBytes-fileSendSocket->write(outBlock);
    outBlock.resize(0);

}

//更新进度条数据
void privatechat::updateFileSendProgress(qint64 numBytes){
    bytesWritten+=(int)numBytes;
    //已经发送数据的大小
    if(bytesToWrite > 0)
    {
        //每次发送loadSize大小的数据，这里设置为4KB，如果剩余的数据不足4KB，
        //就发送剩余数据的大小
        outBlock = localFile->read(qMin(bytesToWrite,loadSize)); 
        //发送完一次数据后还剩余数据的大小
        bytesToWrite -= (int)fileSendSocket->write(outBlock);
        //清空发送缓冲区
        outBlock.resize(0);

    }
    else
    {
        localFile->close(); //如果没有发送任何数据，则关闭文件
    }
    ui->sendFileProgressBar->setMaximum(totalBytes);
    ui->sendFileProgressBar->setValue(bytesWritten);
    //如果数据已经发送完成更改状态信息
    if(bytesWritten==totalBytes){
        ui->sendFileStateLabel->setText("文件已经发送成功");
    }


}

void privatechat::displayError(QAbstractSocket::SocketError){
    qDebug()<<fileSendSocket->errorString();
}

QString privatechat::formateSendMsgToShow(QString plainMsg){
    plainMsg.replace("[act-up.png]","<img src=qrc:/img/Emotion/act-up.png />");
    plainMsg.replace("[admonish.png]","<img src=qrc:/img/Emotion/admonish.png />");
    plainMsg.replace("[amorous.png]","<img src=qrc:/img/Emotion/amorous.png />");
    plainMsg.replace("[angry.png]","<<img src=qrc:/img/Emotion/angry.png />");
    plainMsg.replace("[ass.png]","<img src=qrc:/img/Emotion/ass.png />");
    plainMsg.replace("[bad.png]","<img src=qrc:/img/Emotion/bad.png />");
    plainMsg.replace("[beat-up.png]","<img src=qrc:/img/Emotion/beat-up.png />");
    plainMsg.replace("[bloomer.png]","<img src=qrc:/img/Emotion/bloomer.png />");
    plainMsg.replace("[bulgy-eyes.png]","<img src=qrc:/img/Emotion/bulgy-eyes.png />");
    plainMsg.replace("[bye.png]","<img src=qrc:/img/Emotion/bye.png />");
    plainMsg.replace("[calmly.png]","<<img src=qrc:/img/Emotion/calmly.png />");
    plainMsg.replace("[clap.png]","<img src=qrc:/img/Emotion/clap.png />");
    plainMsg.replace("[cute.png]","<img src=qrc:/img/Emotion/cute.png />");
    plainMsg.replace("[dance.png]","<img src=qrc:/img/Emotion/dance.png />");
    plainMsg.replace("[daydreamer.png]","<img src=qrc:/img/Emotion/daydreamer.png />");
    plainMsg.replace("[devil.png]","<img src=qrc:/img/Emotion/devil.png />");
    plainMsg.replace("[disappointed.png]","<img src=qrc:/img/Emotion/disappointed.png />");
    plainMsg.replace("[dog.png]","<img src=qrc:/img/Emotion/dog.png />");
    plainMsg.replace("[dont-know.png]","<img src=qrc:/img/Emotion/dont-know.png />");
    plainMsg.replace("[eat.png]","<img src=qrc:/img/Emotion/eat.png />");
    plainMsg.replace("[embarrassed.png]","<img src=qrc:/img/Emotion/embarrassed.png />");
    plainMsg.replace("[freaked-out.png]","<img src=qrc:/img/Emotion/freaked-out.png />");
    plainMsg.replace("[go-away.png]","<img src=qrc:/img/Emotion/go-away.png />");
    plainMsg.replace("[dont-know.png]","<img src=qrc:/img/Emotion/dont-know.png />");
    plainMsg.replace("[good.png]","<img src=qrc:/img/Emotion/good.png />");
    plainMsg.replace("[handshake.png]","<img src=qrc:/img/Emotion/handshake.png />");
    plainMsg.replace("[laugh.png]","<img src=qrc:/img/Emotion/laugh.png />");
    plainMsg.replace("[love.png]","<img src=qrc:/img/Emotion/love.png />");
    plainMsg.replace("[music.png]","<img src=qrc:/img/Emotion/music.png />");
    plainMsg.replace("[poop.png]","<img src=qrc:/img/Emotion/poop.png />");
    plainMsg.replace("[smile.png]","<img src=qrc:/img/Emotion/smile.png />");
    return plainMsg;
}

//更改聊天窗口的样式,使其美观
void privatechat::changePrivateChatStyle(){
    //初始化字体大小选择框
    QFontDatabase fontDB;
    foreach(int fontSize,fontDB.standardSizes()){
        ui->fontSizeComboBox->addItem(QString::number(fontSize));
    }

    ui->blackToolBtn->setIcon(QIcon(":/PrivateChat/img/PrivateChat/Bold.png"));
    ui->blackToolBtn->setToolTip("加粗");

    ui->emotionToolBtn->setIcon(QIcon(":/PrivateChat/img/PrivateChat/Emotion.png"));
    ui->emotionToolBtn->setToolTip("表情");

    ui->inclineToolBtn->setIcon(QIcon(":/PrivateChat/img/PrivateChat/Italic.png"));
    ui->inclineToolBtn->setToolTip("倾斜");

    ui->shakeToolBtn->setIcon(QIcon(":/PrivateChat/img/PrivateChat/Shake.png"));
    ui->shakeToolBtn->setToolTip("发送窗口抖动");

    ui->sendFileBtn->setIcon(QIcon(":/PrivateChat/img/PrivateChat/SendFile.ico"));
    ui->sendFileBtn->setToolTip("传送文件");

    ui->snapScreenToolBtn->setIcon(QIcon(":/PrivateChat/img/PrivateChat/ScreenShot.png"));
    ui->snapScreenToolBtn->setToolTip("截图");

    ui->voiceChatToolBtn->setIcon(QIcon(":/PrivateChat/img/PrivateChat/Voice.png"));
    ui->voiceChatToolBtn->setToolTip("点击开始语音通话,再次点击关闭语音通话");

}

//初始化语音通信设置
void privatechat::IniVoiceChatSetting(){
    isInVoiceTalk=false;
    quint16 buffersize = 0;
    buf1 = new Buffer(buffersize);
    buf2 = new Buffer(buffersize);
    vio = new VoiceIO();
    vso = new VoiceSocket();
    vso->startListen();
    connect(vio,  SIGNAL(readVoice(QByteArray)), buf1, SLOT(input(QByteArray)));//当数据录入设备有数据录入时
    connect(buf1, SIGNAL(output(QByteArray)),    vso,  SLOT(writeData(QByteArray)));//把数据写入Socket发送出去
    connect(vso,  SIGNAL(readData(QByteArray)),  buf2, SLOT(input(QByteArray)));
    connect(buf2, SIGNAL(output(QByteArray)),    vio,  SLOT(writeVoice(QByteArray)));
}

void privatechat::on_voiceChatToolBtn_clicked()
{
    //在这里先用本机地址进行测试是否能够通信
    if(!isInVoiceTalk){
        QHostAddress adr = QHostAddress::LocalHost;
        vso->connectToHost(adr);//我把Ip地址更改为自己看一下效果
        //vso->connectToHost(QHostAddress::LocalHost);
        vso->setEnabled(true);
        vio->setEnabled(true);
    }
    else{
        vso->setEnabled(false);
        vio->setEnabled(false);

    }
    isInVoiceTalk=!isInVoiceTalk;

}

//改变编辑框中的字体大小
void privatechat::on_fontSizeComboBox_currentIndexChanged(int index)
{
     QString currentFontSize=ui->fontSizeComboBox->currentText();
     ui->msgTextEdit->setFontPointSize(currentFontSize.toFloat()+6);//随意加的一个数字

}

//初始化私聊界面设置
void privatechat::iniPrivateChatSetting(){
    ui->headImgToolBtn->setIcon(QIcon(targetImgPath));
    ui->headImgToolBtn->setIconSize(QSize(70,70));
    ui->targetAccStateLabel->setText(QString("当前正在与好友:%1聊天").arg(targetAcc));

}

//实现文件拖放发送
void privatechat::dragEnterEvent(QDragEnterEvent *event){
    //QMessageBox::information(this,"拖放","拖放");
    if(event->mimeData()->hasFormat("text/uri-list")){
        event->acceptProposedAction();
       // QMessageBox::information(this,"拖放","拖放");
    }

}

void privatechat::dropEvent(QDropEvent *event){
    QList<QUrl> urls=event->mimeData()->urls();
    if(urls.isEmpty()){
        return ;
    }
    fileName=urls.first().toLocalFile();
   // QMessageBox::information(this,"拖放",fileName);
    if(fileName.isEmpty()){
        return;
    }else{
        sendMsg(SENDFILE);
    }

}

void privatechat::on_closePushBtn_clicked()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->start();
}
