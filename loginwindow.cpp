#include "loginwindow.h"
#include <QListWidgetItem>
#include <QCryptographicHash>
#include <QByteArray>
#include <QSqlQuery>
#include <QMessageBox>
#include <QPainter>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QDir>
#include "ui_loginwindow.h"
#include "registerwindow.h"
#include "accountitem.h"
#include "mainwidget.h"
LoginWindow::LoginWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    simpleDecorateWindow();
    configFilePath="userConfig.xml";
    // QMessageBox::information(this,"工作目录",QDir::currentPath());
    //美化登录界面
    beautifyLoginWindow();

    //设置密码框
    ui->pswLineEdit->installEventFilter(this);
    ui->pswLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pswLineEdit->setPlaceholderText("输入密码");
    ui->pswLineEdit->setEchoMode(QLineEdit::Password);
    ui->pswLineEdit->setStyleSheet("QLineEdit{border-width: 1px; border-radius: 4px; font-size:12px; color: black; border:1px solid gray;}"
                                   "QLineEdit:hover{border-width: 1px; border-radius: 4px; font-size:12px; color: black; border:1px solid rgb(70, 200, 50);}");;

    accListWidget=new QListWidget();
    ui->accountComboBox->setModel(accListWidget->model());
    ui->accountComboBox->setView(accListWidget);
    ui->accountComboBox->setEditable(true);

    //添加最近使用过的账号
    AccountItem* firstAcc=new AccountItem();
    firstAcc->setAccountText("ShadowWalker");
    connect(firstAcc,SIGNAL(showAccount(QString)),this,SLOT(showAccount(QString)));
    connect(firstAcc,SIGNAL(removeAccount(QString)),this,SLOT(removeAccount(QString)));
    QListWidgetItem * listWidgetItem=new QListWidgetItem(accListWidget);
    accListWidget->setItemWidget(listWidgetItem,firstAcc);


    //添加关闭按钮
    btnClose=new sysButton("sys_button_close.png","关闭");
    connect(btnClose,SIGNAL(clicked()),this,SLOT(closeLoginWindow()));//点击关闭按钮，响应函数
    ui->mainLayout->addWidget(btnClose);

    //设置界面出现动画效果
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();

    //默认初始化为False
    isInRememberPsw=false;
    isInAutoLogin=false;
    //读取当前用户的配置文件
    readSettingFromXml(configFilePath);
    ui->accountComboBox->setEditText(currentLoginAcc);
    if(isInRememberPsw){
        ui->remePswCheckBox->setCheckState(Qt::Checked);
        ui->pswLineEdit->setText(curMd5Psw);
    }
    if(isInAutoLogin){
        //emit()
        //ui->accountComboBox->setEditText("ShadowWalker");
        ui->remePswCheckBox->setChecked(Qt::Checked);
        ui->loginCheckBox->setChecked(Qt::Checked);
        on_loginPushBtn_clicked();
        closeLoginWindow();


    }
    connect(ui->remePswCheckBox,SIGNAL(stateChanged(int)),this,SLOT(slotRememberPswState(int)));
    connect(ui->loginCheckBox,SIGNAL(stateChanged(int)),this,SLOT(slotAutoLogin(int)));

}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_registerAccBtn_clicked()
{
    registerwindow * rw=new registerwindow();
    rw->show();

}

void LoginWindow::on_loginPushBtn_clicked()
{
    QSqlDatabase  myDb;
    //得到当前输入的账号值
    this->currentLoginAcc=ui->accountComboBox->currentText();
    //访问数据库进行登录验证
    QString currPsw=ui->pswLineEdit->text();
    //对密码进行哈希计算，Md5散列
    QByteArray pswData;
    pswData.append(currPsw);
    QByteArray hashResult= QCryptographicHash::hash(pswData,QCryptographicHash::Md5);
    QString strHashResult=hashResult.toHex();
    QString queryResult;
    //访问数据库
    QString serverName="SHADOWWALKER";
    QString dbName="InstantMessage";
    myDb = QSqlDatabase::addDatabase("QODBC");//连接到ODBC驱动

    QString dsn = QString("Driver={SQL Server};Server=%1;Database=%2;Trusted_Connection=yes").arg(serverName).arg(dbName);
    myDb.setDatabaseName(dsn);
    if(myDb.open()){
        QSqlQuery query;
        QString sqlQuery="select userPswMd5 from [InstantMessage].[dbo].[User] where userAcc=:curAcc;";
        //QString sqlQuery="select userPswMd5 from [InstantMessage].[dbo].[User] where userAcc= 'ShadowWalker';";
        query.prepare(sqlQuery);
        query.bindValue(0,this->currentLoginAcc);
        bool isSuc= query.exec();
        query.next();
        queryResult=query.value(0).toString();

    }
    myDb.close();
    if(queryResult==strHashResult){
        this->close();
        mw=new mainWidget(currentLoginAcc);
        //mw=new mainWidget("ShadowWalker");
        mw->show();
    }

}

void LoginWindow::showAccount(QString accText){
    ui->accountComboBox->setEditText(accText);
    currentLoginAcc=accText;
    ui->accountComboBox->hidePopup();
}

void LoginWindow::removeAccount(QString accText){
    int accListWidgetNum=accListWidget->count();
    for(int i=0;i<accListWidgetNum;i++){
        QListWidgetItem* item=accListWidget->item(i);
        AccountItem* accItem=(AccountItem *)(accListWidget->itemWidget(item));
        QString currAccText=accItem->getAccountText();
        if(currAccText==accText){
            accListWidget->takeItem(i);
            delete item;
            break;
        }
    }

}

//简单装饰一下窗口，使窗口四角不规则
void LoginWindow::simpleDecorateWindow(){
    this->setWindowFlags(Qt::FramelessWindowHint);
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
}

//
void LoginWindow::drawBackgroundPic(){
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
    brush.setTextureImage(QImage(bgPicPath));
    mainWidgetPainter.setBrush(brush);
    mainWidgetPainter.setPen(Qt::black);
    mainWidgetPainter.drawRoundedRect(rect(),5,5);
    mainWidgetPainter.end();
    mainWidgetPainter.begin(this);
    mainWidgetPainter.drawPixmap(this->rect(),pixmap);
}

//
void LoginWindow::paintEvent(QPaintEvent *){
    drawBackgroundPic();
}

//支持LoginWindow自由拖动
void LoginWindow::mousePressEvent(QMouseEvent * e){
    windowPos=this->pos();
    mousePos=e->globalPos();
    dPos=mousePos-windowPos;

}

void LoginWindow::mouseMoveEvent(QMouseEvent * e){
    this->move(e->globalPos() - this->dPos);
}

void LoginWindow::mouseReleaseEvent(QMouseEvent * e){
    if(isMove){
        isMove=false;
    }
}

//
void LoginWindow::closeLoginWindow(){
    //this->close();
    //界面动画，改变透明度的方式消失1 - 0渐变
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->start();
    connect(animation, SIGNAL(finished()), this, SLOT(close()));
}

//响应checkBox的状态变化
void LoginWindow::slotRememberPswState(int state){
    switch (state) {
    case Qt::Unchecked:{
        isInRememberPsw=false;
        changeConfigValue(configFilePath,"isRememberPsw","false");
        break;
    }
    case Qt::Checked:{
        isInRememberPsw=true;
        changeConfigValue(configFilePath,"isRememberPsw","true");
        break;
    }

    default:
        break;
    }

}

void LoginWindow::slotAutoLogin(int state){
    switch (state) {
    case Qt::Unchecked:{
        // isInRememberPsw=false;
        isInAutoLogin=false;
        changeConfigValue(configFilePath,"isAutoLogin","false");
        break;
    }
    case Qt::Checked:{
        //isInRememberPsw=true;
        ui->remePswCheckBox->setCheckState(Qt::Checked);
        isInRememberPsw=true;
        isInAutoLogin=true;
        changeConfigValue(configFilePath,"isAutoLogin","true");
        break;
    }
    default:
        break;
    }

}

void LoginWindow::readSettingFromXml(QString fileName){
    //settingFile.open()
    settingFile.setFileName(fileName);
    if(!settingFile.open(QIODevice::ReadOnly)){
        //处理打开配置文件失败的情况

    }
    settingXmlReader.setDevice(&settingFile);
    while(!settingXmlReader.atEnd()){
        QXmlStreamReader::TokenType type=settingXmlReader.readNext();
        if(type==QXmlStreamReader::StartElement){
            if(settingXmlReader.name().toString()=="curUserAcc"){
                settingXmlReader.readNext();
                currentLoginAcc=settingXmlReader.text().toString();

            }
            if(settingXmlReader.name().toString()=="isRememberPsw"){
                settingXmlReader.readNext();
                if(settingXmlReader.text().toString()=="true"){
                    isInRememberPsw=true;
                }

            }
            if(settingXmlReader.name().toString()=="isAutoLogin"){
                settingXmlReader.readNext();
                if(settingXmlReader.text().toString()=="true"){
                    //isInRememberPsw=true;
                    isInAutoLogin=true;
                }
            }
            if(settingXmlReader.name().toString()=="curUserPsw"){
                settingXmlReader.readNext();
                QString tempPsw=settingXmlReader.text().toString();
                if(!tempPsw.isNull()){
                    curMd5Psw=tempPsw;
                }
            }

        }
    }
    settingFile.close();
}

//当记住密码和自动登录被勾选后,更改配置文件中的值
void LoginWindow::changeConfigValue(QString configFilePath, QString key, QString value){
    QFile file(configFilePath );
    if( !file.open( QFile::ReadOnly|QFile::Text ) )
    {
        QMessageBox::information(this,"ReadConfigFile","Error");
        qDebug() <<"Can't open config file";
        //return false;
    }

    //    if(!configDomFile.setContent(&file))
    //    {
    //        qDebug() <<"Parse Xml file error";
    //        QMessageBox::information(this,"Parse","Error");
    //        file.close();

    //    }
    QString error;
    int row,column;
    if(!configDomFile.setContent(&file, false, &error, &row, &column))
    {
        QMessageBox::information(NULL, QString("title"), QString("parse file failed at line row and column") + QString::number(row, 10) + QString(",") + QString::number(column, 10));
        return;
    }
    file.close();
    QDomElement docElem =configDomFile.documentElement();
    QDomNodeList nodeList = docElem.elementsByTagName(key);
    int count=nodeList.count();
    if (count >0 )
    {
        QDomElement el = nodeList.at(0).toElement();  //tag为key的节点
        QDomNode oldnode = el.firstChild();           //标签之间的内容作为节点的子节点出现，得到原来的子节点
        el.firstChild().setNodeValue(value);   //用提供的value值来设置子节点的内容
        QDomNode newnode = el.firstChild();               //值修改过后的子节点
        el.replaceChild(newnode,oldnode);                 //调用节点的replaceChild方法实现修改功能


        //定义打开文件错误信息
        QString errorInfo;
        QFileDevice::FileError err = QFileDevice::NoError;
        QFile writefile(configFilePath);
        if (!writefile.open(QFile::WriteOnly | QFile::Text))
        {
            errorInfo=writefile.errorString();
            err=writefile.error();
            QMessageBox::information(this,"更改ConfigFile",errorInfo);

        }
        QTextStream out(&writefile);
        configDomFile.save(out,4);
    }


}

//重写LoginWindow的Enter事件
void LoginWindow::enterEvent(QEvent *e){
    //  on_loginPushBtn_clicked();

}

//Enter键登录
bool LoginWindow::eventFilter(QObject * targetObj, QEvent * e){
    Q_ASSERT(targetObj==ui->pswLineEdit);
    if(e->type()==QEvent::KeyPress){
        QKeyEvent* event=static_cast<QKeyEvent*>(e);
        if(event->key()==Qt::Key_Return){
            on_loginPushBtn_clicked();
            return true;
        }
    }
    return QWidget::eventFilter(targetObj,e);

}


//美化登录界面实现
void LoginWindow::beautifyLoginWindow(){
    ui->registerAccBtn->setFlat(true);
    ui->changePswPushBtn->setFlat(true);
    bgPicPath="://img/LoginWidget/noon.jpg";
    //ui->headImgLabel->setPixmap(QPixmap(":/HeadImg/img/headImg/ShadowWalker.png"));
    ui->headImgLabel->setIcon(QIcon(":/HeadImg/img/headImg/ShadowWalker.png"));
    ui->headImgLabel->setIconSize(QSize(80,80));
}
//更改密码
void LoginWindow::on_changePswPushBtn_clicked()
{
    changePsw=new ChangePsw();
    changePsw->show();

}
