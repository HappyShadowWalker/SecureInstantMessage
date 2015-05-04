#include "changepsw.h"
#include <QMessageBox>
#include "ui_changepsw.h"

ChangePsw::ChangePsw(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChangePsw)
{
    ui->setupUi(this);
    this->setWindowTitle("更改密码");
    ui->oldPswLineEdit->setEchoMode(QLineEdit::Password);
    ui->newPswLineEdit->setEchoMode(QLineEdit::Password);
    ui->confirmNewPswLineEdit->setEchoMode(QLineEdit::Password);

}

ChangePsw::~ChangePsw()
{
    delete ui;
}

QString ChangePsw::getUserPsw(QString queryAcc, QString serverName, QString dbName){

    QString queryResult;
    connectSql = QSqlDatabase::addDatabase("QODBC");//连接到ODBC驱动
    QString connectDsn = QString("Driver={SQL Server};Server=%1;Database=%2;Trusted_Connection=yes").arg(serverName).arg(dbName);
    connectSql.setDatabaseName(connectDsn);
    if(connectSql.open()){
        QString sqlQueryStr="select userPswMd5 from [InstantMessage].[dbo].[User] where userAcc=:curAcc;";
        QSqlQuery sqlQuery(connectSql);
        sqlQuery.prepare(sqlQueryStr);
        sqlQuery.bindValue(0,queryAcc);
        bool isSuc= sqlQuery.exec();
        sqlQuery.next();
        queryResult=sqlQuery.value(0).toString();
        return queryResult;
    }
    connectSql.close();
}

void ChangePsw::on_oldPswLineEdit_editingFinished()
{
    QString oldPswMd5=getUserPsw(queryAcc,"SHADOWWALKER","InstantMessage");
    //QMessageBox::information(this,"OldPswMd5",oldPsw);
    QString inputOldPsw=ui->oldPswLineEdit->text();
    QString inputOldPswMd5=calcMd5FromStr(inputOldPsw);
    if(oldPswMd5==inputOldPswMd5){
        //QMessageBox::information(this,"密码一致","原密码与输入密码一致");
        isOldPswRight=true;
    }
    else{
        isOldPswRight=false;
        QMessageBox::information(this,"密码不一致","原密码与输入密码不一致");
    }
}

void ChangePsw::on_confirmNewPswLineEdit_editingFinished()
{
    QString firstNewPsw=ui->newPswLineEdit->text();
    QString secondNewPsw=ui->confirmNewPswLineEdit->text();
    if(firstNewPsw==secondNewPsw){
        isNewPswRight=true;
        newPsw=firstNewPsw;

    }
    else{
        isNewPswRight=false;
        QMessageBox::information(this,"密码不一致","两次输入密码不一致");

    }

}

void ChangePsw::on_changePswPushBtn_clicked()
{
    //更新密码
    QString newPswMd5=calcMd5FromStr(newPsw);
    QString serverName="ShadowWalker";
    QString dbName="InstantMessage";
    if(isOldPswRight&&isNewPswRight){
        connectSql = QSqlDatabase::addDatabase("QODBC");//连接到ODBC驱动
        QString connectDsn = QString("Driver={SQL Server};Server=%1;Database=%2;Trusted_Connection=yes").arg(serverName).arg(dbName);
        connectSql.setDatabaseName(connectDsn);
        if(connectSql.open()){
            QString sqlQueryStr="update [InstantMessage].[dbo].[User] set userPswMd5=:newPswMd5 where userAcc=:queryAcc;";
            QSqlQuery sqlQuery(connectSql);
            sqlQuery.prepare(sqlQueryStr);
            sqlQuery.bindValue(0,newPswMd5);
            sqlQuery.bindValue(1,queryAcc);
            bool isSuc= sqlQuery.exec();
            if(isSuc){
                QMessageBox::information(this,"更改密码","成功更改密码");
            }

        }
        connectSql.close();

    }

}


void ChangePsw::on_accLineEdit_editingFinished()
{
    queryAcc=ui->accLineEdit->text();

}

//计算密码的Md5值
QString ChangePsw::calcMd5FromStr(QString plainPsw){
    QByteArray pswData;
    pswData.append(plainPsw);
    QByteArray hashResult= QCryptographicHash::hash(pswData,QCryptographicHash::Md5);
    QString strHashResult=hashResult.toHex();
    return strHashResult;
}
