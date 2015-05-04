#include "registerwindow.h"
#include <QCryptographicHash>
#include <QSqlQuery>
#include <QBitmap>
#include <QPainter>
#include <QMouseEvent>
#include <QEvent>
#include <QMessageBox>
#include "ui_registerwindow.h"

registerwindow::registerwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::registerwindow)
{
    ui->setupUi(this);
    this->setWindowTitle("SIM Register");
    isMove=false;
    this->mainWidgetPicName=":/RegisterWidget/img/RegisterWidget/Register.jpg";

    frameLessWidget();

    //this->autoFillBackground();
    btnClose=new sysButton("sys_button_close.png","关闭");

    connect(btnClose,SIGNAL(clicked()),this,SLOT(closeRWWiget()));//点击关闭按钮，响应函数
    ui->mainHorLayout->addWidget(btnClose);

    iniVerCode();

    paintVerCode(this->verCode,ui->verCodeLabel);

    //设置密码输入框为密码输入模式
    ui->pswLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->pswLineEdit->setPlaceholderText("输入密码,密码需大于6个字符");
    ui->pswLineEdit->setEchoMode(QLineEdit::Password);
    ui->pswLineEdit->setStyleSheet("QLineEdit{border-width: 1px; border-radius: 4px; font-size:12px; color: black; border:1px solid gray;}"
                                   "QLineEdit:hover{border-width: 1px; border-radius: 4px; font-size:12px; color: black; border:1px solid rgb(70, 200, 50);}");;

    ui->repeatPswLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->repeatPswLineEdit->setPlaceholderText("输入密码,密码需大于6个字符");
    ui->repeatPswLineEdit->setEchoMode(QLineEdit::Password);
    ui->repeatPswLineEdit->setStyleSheet("QLineEdit{border-width: 1px; border-radius: 4px; font-size:12px; color: black; border:1px solid gray;}"
                                   "QLineEdit:hover{border-width: 1px; border-radius: 4px; font-size:12px; color: black; border:1px solid rgb(70, 200, 50);}");;

    ui->acountLineEdit->setStyleSheet("QLineEdit{border-width: 1px; border-radius: 4px; font-size:12px; color: black; border:1px solid gray;}"
                                   "QLineEdit:hover{border-width: 1px; border-radius: 4px; font-size:12px; color: black; border:1px solid rgb(70, 200, 50);}");;
    ui->verCodeLineEdit->setStyleSheet("QLineEdit{border-width: 1px; border-radius: 4px; font-size:12px; color: black; border:1px solid gray;}"
                                   "QLineEdit:hover{border-width: 1px; border-radius: 4px; font-size:12px; color: black; border:1px solid rgb(70, 200, 50);}");;

    //
    isValidAcc=false;
    isValidPsw=false;
    isRightVerCode=false;
}

registerwindow::~registerwindow()
{
    delete ui;
}

void registerwindow::on_registerPushBtn_clicked()
{
    if(isRightVerCode&&isValidAcc&&isValidPsw){

        this->serverName="ShadowWalker";
        this->dbName="InstantMessage";
        //读取注册信息写入数据库
        QString acc=ui->acountLineEdit->text();
        QString psw=ui->pswLineEdit->text();
        //对密码进行Md5哈希计算

        //对密码进行哈希计算，Md5散列
        QByteArray pswData;
        pswData.append(psw);
        QByteArray hashResult= QCryptographicHash::hash(pswData,QCryptographicHash::Md5);

        QString strHashResult=hashResult.toHex();

        connectDataBase(this->serverName,this->dbName);
        QSqlQuery query;
        query.prepare("insert into [InstantMessage].[dbo].[User] (userAcc,userPswMd5) values (:userID,:userPsw);");
        query.bindValue(0,acc);
        query.bindValue(1,strHashResult);
        bool execResult=query.exec();
        if(execResult){
            QMessageBox::about(this,"注册成功","恭喜你注册成功!");


        }
        else{
            QMessageBox::critical(this,"注册错误","很抱歉注册错误，请重试");
        }
        myDb.close();
    }
    else{

    }

}

bool registerwindow::connectDataBase(QString serverName, QString dbName){
    myDb = QSqlDatabase::addDatabase("QODBC");//连接到ODBC驱动
    QString dsn = QString("Driver={SQL Server};Server=%1;Database=%2;Trusted_Connection=yes").arg(serverName).arg(dbName);
    myDb.setDatabaseName(dsn);
    if(myDb.open()){
        return true;
    }
    else{
        return false;
    }

}

void registerwindow::frameLessWidget(){
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


void registerwindow::mousePressEvent(QMouseEvent * e){
    windowPos=this->pos();
    mousePos=e->globalPos();
    dPos=mousePos-windowPos;

}

void registerwindow::mouseMoveEvent(QMouseEvent * e){
    this->move(e->globalPos() - this->dPos);
}

void  registerwindow::mouseReleaseEvent(QMouseEvent * e){
    if(isMove)
         isMove=false;
}


void registerwindow::paintEvent(QPaintEvent * ){
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
    brush.setTextureImage(QImage(this->mainWidgetPicName));
    mainWidgetPainter.setBrush(brush);
    mainWidgetPainter.setPen(Qt::black);
    mainWidgetPainter.drawRoundedRect(rect(),5,5);
    mainWidgetPainter.end();
    mainWidgetPainter.begin(this);
    mainWidgetPainter.drawPixmap(this->rect(),pixmap);

}


void registerwindow::closeRWWiget(){
    this->close();
}

void registerwindow::paintVerCode(QString verCode, QLabel *target){
    //设置字号
     QFont ft;
     ft.setPointSize(42);
     target->setFont(ft);
    //设置颜色
     QPalette pa;
     pa.setColor(QPalette::WindowText,Qt::red);
     target->setPalette(pa);

     target->setText(verCode);


}

void registerwindow::iniVerCode(){
    QTime time;
    time=QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    this->verCode="";
    this->verCode+=QString::number(qrand()%10);
    this->verCode+=QString::number(qrand()%10);
    this->verCode+=QString::number(qrand()%10);
    this->verCode+=QString::number(qrand()%10);
}



void registerwindow::on_changeVerCodePushBtn_clicked()
{
    iniVerCode();
    paintVerCode(verCode,ui->verCodeLabel);
}

void registerwindow::on_acountLineEdit_editingFinished()
{
    QString accText=ui->acountLineEdit->text();
    if(accText.size()<4){
        //设置字号
         QFont ft;
         ft.setPointSize(10);
         ui->hintAccLabel->setFont(ft);

        //设置颜色
         QPalette pa;
         pa.setColor(QPalette::WindowText,Qt::red);
         ui->hintAccLabel->setPalette(pa);
         ui->hintAccLabel->setText("账户名称不能小于四个字符");
    }
    else{
        isValidAcc=true;
        ui->hintAccLabel->setText("");
    }

}

void registerwindow::on_pswLineEdit_editingFinished()
{
   // ui->hintAccLabel->setText("");
    QString pswText=ui->pswLineEdit->text();

    if(pswText.size()<6){
        //设置字号
         QFont ft;
         ft.setPointSize(10);
         ui->hintPswLabel->setFont(ft);

        //设置颜色
         QPalette pa;
         pa.setColor(QPalette::WindowText,Qt::red);
         ui->hintPswLabel->setPalette(pa);
         ui->hintPswLabel->setText("密码不能小于6个字符");
    }
    else{
        ui->hintPswLabel->setText("");
    }

}

void registerwindow::on_repeatPswLineEdit_editingFinished()
{
    //ui->hintPswLabel->setText("");
    QString pswText=ui->pswLineEdit->text();
    QString repeatPswText=ui->repeatPswLineEdit->text();
    if(pswText!=repeatPswText){
        //设置字号
         QFont ft;
         ft.setPointSize(10);
         ui->hintRepeatLabel->setFont(ft);

        //设置颜色
         QPalette pa;
         pa.setColor(QPalette::WindowText,Qt::red);
         ui->hintRepeatLabel->setPalette(pa);
         ui->hintRepeatLabel->setText("两次输入的密码不一致");

    }
    else{
        isValidPsw=true;
        ui->hintRepeatLabel->setText("");
    }

}

void registerwindow::on_verCodeLineEdit_editingFinished()
{
   // ui->hintRepeatLabel->setText("");
    QString getVerCode=ui->verCodeLineEdit->text();
    if(getVerCode!=this->verCode){
        //设置字号
         QFont ft;
         ft.setPointSize(10);
         ui->hintVerCodeLabel->setFont(ft);

        //设置颜色
         QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::red);
        ui->hintVerCodeLabel->setPalette(pa);
        ui->hintVerCodeLabel->setText("验证码输入错误");

    }
    else{
        isRightVerCode=true;
        ui->hintVerCodeLabel->setText("");
    }
}
