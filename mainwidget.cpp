#include "mainwidget.h"
#include <QPainter>
#include <QBitmap>
#include <QMouseEvent>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>
#include <QNetworkInterface>
#include "ui_mainwidget.h"
#include "changeskinwidget.h"
#include "statechangewidget.h"
mainWidget::mainWidget(QString userName,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainWidget)
{
    ui->setupUi(this);
    beaurifyMainWidget();
    ui->searchTextEdit->installEventFilter(this);
    currLoginAcc=userName;
    prepareWork(currLoginAcc);
    //在主界面上动态添加 换肤按钮 最小化按钮 关闭按钮等
    btnSkin=new skinPushButton("SkinButton.png","换肤");
    btnMin=new sysButton("sys_button_min.png","最小化");
    btnClose=new sysButton("sys_button_close.png","关闭");

    connect(btnClose,SIGNAL(clicked()),this,SLOT(closeMainWindow()));//点击关闭按钮，响应函数
    connect(btnMin,SIGNAL(clicked()),this,SLOT(minMainWindow()));
    connect(btnSkin,SIGNAL(clicked()),this,SLOT(openSkinWidgetWindow()));

    ui->mainLayout->addStretch();
    ui->mainLayout->addWidget(btnSkin,0,Qt::AlignVCenter);
    btnSkin->setContentsMargins(0,0,5,0);
    ui->mainLayout->addWidget(btnMin);
    ui->mainLayout->addWidget(btnClose);
    ui->mainLayout->setSpacing(0);
    ui->mainLayout->setContentsMargins(0,0,0,0);

    readUserSettings(this->currLoginAcc);

    //树形控件来显示好友列表信息
    readUserSettingsFromXml("ShadowWalker",":/userSetting.xml");
    setTreeWidget();
    connect(ui->friendTreeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(onTreeItemDoubleClick(QTreeWidgetItem*,int)));

    //添加文件监听功能
    fileListenServer=new QTcpServer(this);
    connect(fileListenServer,SIGNAL(newConnection()),this,SLOT(acceptTcpConnection()));
    totalBytes=0;
    bytesReceived=0;
    fileNameSize=0;
    fileListenServer->listen(QHostAddress::Any,SendFileDataPort);

    //添加监听好友登录或下线或发送消息的UdpSocket
    msgListenUdpSocket=new QUdpSocket(this);
    msgListenUdpSocket->bind(QHostAddress::LocalHost,BroadcastPort);
    connect(msgListenUdpSocket,SIGNAL(readyRead()),this,SLOT(readMsg()));

    //当你登录成功后需要广播自己已经成功登陆
    msgSendUdpSocket=new QUdpSocket(this);
    sendMessage(NewParticipant,currLoginAcc,currAccIp.toString());

    //添加系统托盘图标
    simSystemTray=new SimSystemTray(this);
    connect(simSystemTray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(iconIsActived(QSystemTrayIcon::ActivationReason)));
    connect(simSystemTray,SIGNAL(restore()),this,SLOT(restoreMainWidget()));
    connect(simSystemTray,SIGNAL(selfStart()),this,SLOT(selfStart()));
    connect(simSystemTray,SIGNAL(setting()),this,SLOT(openSettings()));
    this->setWindowTitle("Secure Instant Message");
    simSystemTray->showMyself();

}

mainWidget::~mainWidget()
{
    delete ui;
}


//关闭主窗口
void mainWidget::closeMainWindow()
{
    this->close();
    sendMessage(ParticipantLeft,currLoginAcc,"Leave");
}
//最小化主窗口
void mainWidget::minMainWindow(){
    // this->showMinimized();
    //simSystemTray->setToolTip("Secure Instant Message 在后台运行");
    //simSystemTray->showMyself();
    this->hide();
    simSystemTray->showMessage("SIM","SIM 在后台运行");
}

//窗体绘制函数
void mainWidget::paintEvent(QPaintEvent *){
    QBitmap mainWidgetBitmap(this->size());
    mainWidgetBitmap.fill();
    QPainter mainWidgetPainter(&mainWidgetBitmap);
    mainWidgetPainter.setBrush(Qt::black);
    mainWidgetPainter.drawRoundedRect(mainWidgetBitmap.rect(),5,5);

    setMask(mainWidgetBitmap);

    QPixmap pixmap(this->size());
    pixmap.fill();
    mainWidgetPainter.end();
    mainWidgetPainter.begin(&pixmap);
    mainWidgetPainter.setRenderHints(QPainter::Antialiasing,true);
    QBrush brush;
    brush.setStyle(Qt::TexturePattern);
    brush.setTextureImage(QImage(mainWidgetPicName));
    mainWidgetPainter.setBrush(brush);
    mainWidgetPainter.setPen(Qt::black);
    mainWidgetPainter.drawRoundedRect(rect(),5,5);
    mainWidgetPainter.end();
    mainWidgetPainter.begin(this);
    mainWidgetPainter.drawPixmap(this->rect(),pixmap);
}

// 点击换肤按钮的消息响应函数
void mainWidget::openSkinWidgetWindow(){
    changeSkinWidget *skinWindow=new changeSkinWidget(mainWidgetPicName);
    skinWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(skinWindow,SIGNAL(changeSkin(QString)),this,SLOT(changeMainWidgetPic(QString)));

    QPoint p=rect().topRight();
    p.setX(p.x()-150-350);
    p.setY(p.y()+30);
    skinWindow->move(this->mapToGlobal(p));
    skinWindow->show();

}

void mainWidget::changeMainWidgetPic(QString picName){
    mainWidgetPicName=picName;
    update();

}

//重写窗体的鼠标拖动函数，使窗体支持拖动
void mainWidget::mousePressEvent(QMouseEvent * e){
    windowPos=this->pos();
    mousePos=e->globalPos();
    dPos=mousePos-windowPos;

}

void mainWidget::mouseMoveEvent(QMouseEvent * e){
    this->move(e->globalPos() - this->dPos);
}

void  mainWidget::mouseReleaseEvent(QMouseEvent * e){
    if(isMove)
        isMove=false;
}

bool mainWidget::readUserSettings(QString currLoginAcc){
    if(connectDatabase(this->serverName,this->dbName)){
        QSqlQuery query(myDb);
        QString sqlQuery="select userSignedName, userHeadImg from [InstantMessage].[dbo].[User] where userAcc=:curAcc;";

        query.prepare(sqlQuery);
        query.bindValue(0,this->currLoginAcc);
        bool isSuc= query.exec();
        if(isSuc){
            query.next();
            QString userSignedName=query.value(0).toString();
            QString userHeadImg=query.value(1).toString();
            currUserHeadImg=userHeadImg;
            currUserSignedName=userSignedName;
            ui->signNameTextEdit->setText(userSignedName);
            ui->headImgToolBtn->setIcon(QIcon(userHeadImg));
            ui->headImgToolBtn->setIconSize(QSize(70,70));
        }
        else{
            QMessageBox::about(this,"读取数据库失败","无法读取数据库");
        }


    }
    myDb.close();
}

bool mainWidget::connectDatabase(QString serverName, QString databaseName){
    myDb = QSqlDatabase::addDatabase("QODBC");//连接到ODBC驱动
    QString dsn = QString("Driver={SQL Server};Server=%1;Database=%2;Trusted_Connection=yes").arg(serverName).arg(databaseName);
    myDb.setDatabaseName(dsn);
    if(myDb.open()){
        return true;
    }
    else{
        return false;
    }
}

//从xml文件中读取当前登录用户的配置信息比如其好友信息
bool mainWidget::readUserSettingsFromXml(QString currLoginAcc,QString settingXmlPath){
    QFile xmlFile(settingXmlPath);
    if(!xmlFile.open(QFile::ReadOnly|QFile::Text)){
        qDebug("Error:can't open xml file");
        return false;
    }
    QXmlStreamReader xmlReader;
    //定义变量存储读取结果
    QString curAccImgPath,curAccSignedName,curAcc;
    //QStringList friendsImgPath,friendsSignedName,friendsAcc;
    xmlReader.setDevice(&xmlFile);
    while(!xmlReader.atEnd()){
        QXmlStreamReader::TokenType type=xmlReader.readNext();
        if(type==QXmlStreamReader::StartElement){
            if(xmlReader.name().toString()=="friendAcc"){
                xmlReader.readNext();
                QString tempAcc=xmlReader.text().toString();
                friendsAcc.append(tempAcc);

            }
            if(xmlReader.name().toString()=="friendImgPath"){
                xmlReader.readNext();
                friendsImgPath.append(xmlReader.text().toString());

            }
            if(xmlReader.name().toString()=="friendSignedName"){
                xmlReader.readNext();
                friendsSignedName.append(xmlReader.text().toString());

            }
            if(xmlReader.name().toString()=="friendIP"){
                xmlReader.readNext();
                friendsIP.append(xmlReader.text().toString());

            }

        }
    }
    //把读取出来的数据结构化到friendsInfoMap中
    for(int i=0;i<friendsAcc.size();i++){
        //QString tempFriendAcc,tempFirendImgPath,tempFriendSignedName,tempFriendIP;
        QStringList tempFriendInfo;
        tempFriendInfo.append(friendsIP.at(i));
        tempFriendInfo.append(friendsImgPath.at(i));
        tempFriendInfo.append(friendsSignedName.at(i));
        friendsInfoMap.insert(friendsAcc.at(i),tempFriendInfo);
        tempFriendInfo.clear();

    }

}

void mainWidget::parseElementFromXml(QXmlStreamReader &xmlReader){

}

void mainWidget::setTreeWidget(){
    ui->friendTreeWidget->setHeaderHidden(true);

    ui->friendTreeWidget->setColumnCount(1); //
    QList<QTreeWidgetItem *> rootList;
    QTreeWidgetItem *friendGroupWidgetItem=new QTreeWidgetItem(ui->friendTreeWidget,QStringList(QString("我的好友")));
    rootList.append(friendGroupWidgetItem);
    int friendCounts=friendsInfoMap.size();
    //定义迭代器
    QMap<QString,QStringList>::Iterator i;
    for(i=friendsInfoMap.begin();i!=friendsInfoMap.end();i++){
        QTreeWidgetItem * tempFriendItem=new QTreeWidgetItem(friendGroupWidgetItem);
        //UserItem* tempUserItem=new UserItem(friendsAcc.at(i),friendsSignedName.at(i),friendsImgPath.at(i));
        UserItem* tempUserItem=new UserItem(i.key(),((QStringList)(i.value())).at(2),((QStringList)(i.value())).at(1));
        friendGroupWidgetItem->addChild(tempFriendItem);
        ui->friendTreeWidget->setItemWidget(tempFriendItem,0,tempUserItem);
    }
    ui->friendTreeWidget->insertTopLevelItems(0,rootList);
}

//好友列表项的双击响应函数
void mainWidget::onTreeItemDoubleClick(QTreeWidgetItem *itemClickedTW, int i){
    UserItem * clickedUserItem;
    clickedUserItem=dynamic_cast<UserItem*> (ui->friendTreeWidget->itemWidget(itemClickedTW,0));
    QString clickedUserAcc=clickedUserItem->getUserAcc();
    QString clickedUserHostAddr=this->friendsInfoMap[clickedUserAcc].at(0);
    QString clickedUserImgPath=this->friendsInfoMap[clickedUserAcc].at(1);
    privatechat* chatTarget=new privatechat("ShadowWalker",QHostAddress(clickedUserHostAddr),clickedUserImgPath);
    chatTarget->show();
}

//实现文件传输功能
void mainWidget::acceptTcpConnection(){
    /*  fileReceiveWidget=new FileReceiveWidget();
    connect(fileReceiveWidget,SIGNAL(denyClicked()),this,SLOT(denyTcpConnection()));
    connect(fileReceiveWidget,SIGNAL(receiveClicked()),this,SLOT(beginReceiveFile()));
    fileReceiveWidget->show()*/;

    fileReceiveSocket=fileListenServer->nextPendingConnection();
    connect(fileReceiveSocket,SIGNAL(readyRead()),this,SLOT(receiveFile()));

    // sendFileRequestAnswer(SENDFILE,"AccFile",currentTargetIp);


}

void mainWidget::denyTcpConnection(){
    //  fileReceiveSocket=fileListenServer->nextPendingConnection();
    // fileReceiveSocket->close();
    fileReceiveWidget->close();
    //发送文件请求拒绝消息
    sendFileRequestAnswer(SENDFILE,"DenyFile",currentTargetIp);
}

void mainWidget::beginReceiveFile(){
    sendFileRequestAnswer(SENDFILE,"AccFile",currentTargetIp);//发送文件请求应答消息
    //    fileReceiveSocket=fileListenServer->nextPendingConnection();
    //    connect(fileReceiveSocket,SIGNAL(readyRead()),this,SLOT(receiveFile()));

}

void mainWidget::receiveFile(){
    QDataStream in(fileReceiveSocket);
    in.setVersion(QDataStream::Qt_5_2);
    if(bytesReceived <= sizeof(qint64)*2)
    { //如果接收到的数据小于16个字节，那么是刚开始接收数据，我们保存到//来的头文件信息
        qint64 eachBytesSize=fileReceiveSocket->bytesAvailable();
        if((eachBytesSize>= sizeof(qint64)*2)&& (fileNameSize == 0))
        { //接收数据总大小信息和文件名大小信息
            in >> totalBytes >> fileNameSize;
            bytesReceived += sizeof(qint64) * 2;

        }
        if((eachBytesSize >= fileNameSize)&& (fileNameSize != 0))
        {  //接收文件名，并建立文件
            in >> fileName;
            bytesReceived += fileNameSize;
            localFile = new QFile(fileName);
            if(!localFile->open(QFile::WriteOnly))
            {
                qDebug() << "open file error!";
                return;
            }
        }
        else return;
    }
    if(bytesReceived < totalBytes)
    {  //如果接收的数据小于总数据，那么写入文件
        bytesReceived += fileReceiveSocket->bytesAvailable();
        inBlock = fileReceiveSocket->readAll();
        localFile->write(inBlock);
        inBlock.resize(0);
    }
    fileReceiveWidget->setProgressBarMaxValue(totalBytes);
    fileReceiveWidget->setProgressBarValue(bytesReceived);
    //更新进度条
    if(bytesReceived == totalBytes)
    { //接收数据完成时
        fileReceiveSocket->close();
        localFile->close();
    }
}

//UdpSocket 读取消息
void mainWidget::readMsg(){
    while(msgListenUdpSocket->hasPendingDatagrams()){
        QByteArray receivedData;
        QHostAddress senderHostAddress;
        receivedData.resize(msgListenUdpSocket->pendingDatagramSize());
        msgListenUdpSocket->readDatagram(receivedData.data(),receivedData.size(),&senderHostAddress);
        QByteArray aesKey=aesCrypto.HexStringToByte(keyString);
        QByteArray decryptedData=aesCrypto.Decrypt(receivedData,aesKey);
        QString senderUserName;
        int msgType;
        QString MsgContent;
        QDataStream receivedDataStream(&decryptedData,QIODevice::ReadOnly);
        receivedDataStream>>msgType;
        StateChangeWidget* msgToast;
        receivedDataStream>>senderUserName;//读取发送者的用户名也就是发送者的账号ID
        currentTargetIp=senderHostAddress;
        switch (msgType) {
        case Message:{
            receivedDataStream>>MsgContent;
            msgToast=new StateChangeWidget(senderUserName);
            msgToast->showInLowLeft();
            msgToast->showTipMsg("新消息","你的好友:"+senderUserName+"给你说:"+MsgContent);

        }
            break;
        case NewParticipant:
        {
            //处理新用户加入
            msgToast=new StateChangeWidget(senderUserName);
            msgToast->showInLowLeft();
            msgToast->showTipMsg("登录消息","你的好友:"+senderUserName+"已经上线了");
            //在好友列表中添加这个好友 还没完成
            readFriendInfoFromDB(senderUserName,currentTargetIp.toString());//更新好友列表

        }
            break;
        case ParticipantLeft:{
            //在好友列表中把这个好友删除
            //通知好友下线
            msgToast=new StateChangeWidget(senderUserName);
            msgToast->showInLowLeft();
            msgToast->showTipMsg("下线消息","你的好友:"+senderUserName+"已经下线了");

        }
            break;
        case SENDFILE:{
            receivedDataStream>>MsgContent;
            if(MsgContent=="SendFile"){
                fileReceiveWidget=new FileReceiveWidget();
                //QMessageBox::about(this,"FileReceive",MsgContent);
                connect(fileReceiveWidget,SIGNAL(denyClicked()),this,SLOT(denyTcpConnection()));
                connect(fileReceiveWidget,SIGNAL(receiveClicked()),this,SLOT(beginReceiveFile()));
                fileReceiveWidget->show();

            }

        }
            break;
        default:
            break;
        }
    }

}

//实现消息的发送主要发送两种消息,第一种是新用户登录,另一种是用户离开
void mainWidget::sendMessage(MessageType msgType, QString senderAcc,QString msgContent){
    QByteArray msgByte;
    QDataStream msgDataStream(&msgByte,QIODevice::WriteOnly);
    msgDataStream<<msgType<<senderAcc<<msgContent;
    //测试先发送数据到本机端口
    QByteArray aesKey=aesCrypto.HexStringToByte(keyString);
    QByteArray encryptData=aesCrypto.Encrypt(msgByte,aesKey);
    msgSendUdpSocket->writeDatagram(encryptData,encryptData.length(),QHostAddress::Broadcast,BroadcastPort);//广播用户登录和退出消息
}

//托盘菜单项的响应函数
void mainWidget::iconIsActived(QSystemTrayIcon::ActivationReason reason){
    switch (reason) {
    case QSystemTrayIcon::Trigger:{
        this->showNormal();
        break;
    }
    case QSystemTrayIcon::DoubleClick:{
        this->showNormal();
        break;
    }
    default:
        break;
    }
}

void mainWidget::selfStart(){

}

void mainWidget::openSettings(){

}
void mainWidget::restoreMainWidget(){
    this->showNormal();
}


//实现文件请求信息的发送
void mainWidget::sendFileRequestAnswer(MessageType msgType, QString msgContent, QHostAddress targetIP){
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out<<SENDFILE<<currLoginAcc<<msgContent;
    //测试先发送数据到本机端口
    QByteArray aesKey=aesCrypto.HexStringToByte(keyString);
    QByteArray encryptData=aesCrypto.Encrypt(data,aesKey);
    msgSendUdpSocket->writeDatagram(encryptData,targetIP,PrivateChatPort);//回复文件请求
}

//获取当前用户的IP地址
QHostAddress mainWidget::getCurUserIp(){
    QList<QHostAddress> listHostAddress =QNetworkInterface::allAddresses();
    foreach (QHostAddress  address, listHostAddress) {
        if(address.protocol()==QAbstractSocket::IPv4Protocol){
            return address;
        }
    }

}

//实施主界面的准备工作
void mainWidget::prepareWork(QString currUser){
    this->setWindowIcon(QIcon(":img/qq.ico"));
    this->setWindowTitle("SecureInstantMessage");
    this->setWindowFlags(Qt::FramelessWindowHint);
    //设置默认窗体背景图片名称
    mainWidgetPicName=":/img/skin/pinkLove.jpg";
    QBitmap bmp(this->size());
    bmp.fill();
    QPainter p(&bmp);
    p.setRenderHint(QPainter::Antialiasing);    //抗锯齿
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
    p.drawPath(path);
    p.fillPath(path, QBrush(Qt::red));
    setMask(bmp);

    //设置当前用户的IP地址
    currAccIp=getCurUserIp();
}

//设计一个函数 根据用户名 访问数据库返回 用户的信息 签名 头像路径
void mainWidget::readFriendInfoFromDB(QString friendAcc,QString friendIP){
    QStringList friendQueryResult;
    QString friendSignedName,friendHeadImg;
    if(connectDatabase(this->serverName,this->dbName)){
        QSqlQuery query(myDb);
        QString sqlQuery="select userSignedName, userHeadImg from [InstantMessage].[dbo].[User] where userAcc=:friendAcc;";

        query.prepare(sqlQuery);
        query.bindValue(0,friendAcc);
        bool isSuc= query.exec();
        if(isSuc){
            query.next();
            friendSignedName=query.value(0).toString();
            friendHeadImg=query.value(1).toString();

        }
        else{
            QMessageBox::about(this,"读取数据库失败","无法读取数据库");
        }

    }
    //得到数据之后更新好友列表,更新好友数据结构
    friendQueryResult<<friendIP<<friendHeadImg<<friendSignedName;
    friendsInfoMap.insert(friendAcc,friendQueryResult);
    //更新好友列表显示
    QTreeWidgetItem* parentTreeWidgetItem=ui->friendTreeWidget->itemAt(0,0);
    QTreeWidgetItem * newFriendWidgetItem=new QTreeWidgetItem(parentTreeWidgetItem);
    UserItem* tempUserItem=new UserItem(friendAcc,friendSignedName,friendHeadImg);
    parentTreeWidgetItem->addChild(newFriendWidgetItem);
    ui->friendTreeWidget->setItemWidget(newFriendWidgetItem,0,tempUserItem);
    myDb.close();


}

//实现对于好友的搜索
void mainWidget::on_searchToolBtn_clicked()
{
    QString searchKeyWord=ui->searchTextEdit->toPlainText();
    QStringList searchAccContainer;
    QMap<QString,QStringList>::Iterator i;

    for(i=friendsInfoMap.begin();i!=friendsInfoMap.end();i++){
        searchAccContainer<<(QString)i.key();
    }
    int resultIndex=-1;
    QString searchResult;
    QString searchResultIP;
    QString searchResultImgPath;
    for(int j=0;j<searchAccContainer.size();j++){
        if(searchAccContainer.at(j).contains(searchKeyWord,Qt::CaseInsensitive)){
            resultIndex=j;
            searchResult=searchAccContainer.at(j);
        }
    }
    searchResultIP=friendsInfoMap[searchResult].at(0);
    searchResultImgPath=friendsInfoMap[searchResult].at(1);
    if(resultIndex==-1){
        QMessageBox::information(this,"搜索好友","当前没有找到这个好友");
    }
    else{
        privatechat* chatTarget=new privatechat(searchResult,QHostAddress(searchResultIP),searchResultImgPath);
        chatTarget->show();
    }

}

//Enter键进行搜索
bool mainWidget::eventFilter(QObject * targetObj, QEvent * e){
    Q_ASSERT(targetObj==ui->searchTextEdit);
    if(e->type()==QEvent::KeyPress){
        QKeyEvent* event=static_cast<QKeyEvent*>(e);
        if(event->key()==Qt::Key_Return){
            on_searchToolBtn_clicked();
            return true;
        }
    }
    return QWidget::eventFilter(targetObj,e);
}

//
void mainWidget::beaurifyMainWidget(){
    ui->searchToolBtn->setIcon(QIcon(":/MainWidget/img/MainWidget/Search.png"));
    ui->searchToolBtn->setIconSize(QSize(30,30));
    ui->searchTextEdit->setPlaceholderText("搜索好友进行聊天");

    ui->sysSettingToolBtn->setIcon(QIcon(":/MainWidget/img/MainWidget/SimMainSetting.png"));
    ui->sysSettingToolBtn->setIconSize(QSize(30,30));
    ui->sysSettingToolBtn->setToolTip("设置");

    ui->sysHelpToolBtn->setIcon(QIcon(":/MainWidget/img/MainWidget/SimHelp.png"));
    ui->sysHelpToolBtn->setIconSize(QSize(30,30));
    ui->sysHelpToolBtn->setToolTip("帮助");


}
