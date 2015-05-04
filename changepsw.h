#ifndef CHANGEPSW_H
#define CHANGEPSW_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QCryptographicHash>
#include <QByteArray>
//实现用户更改密码功能
namespace Ui {
class ChangePsw;
}

class ChangePsw : public QWidget
{
    Q_OBJECT

public:
    explicit ChangePsw(QWidget *parent = 0);
    ~ChangePsw();

private slots:
    void on_oldPswLineEdit_editingFinished();

    void on_confirmNewPswLineEdit_editingFinished();

    void on_changePswPushBtn_clicked();

    void on_accLineEdit_editingFinished();

private:
    Ui::ChangePsw *ui;
    //连接数据库 得到当前用户帐户的密码的Md5值
    QString  getUserPsw(QString queryAcc,QString serverName,QString dbName);
    QString serverName;
    QString dbName;
    QString queryAcc;
    QSqlDatabase connectSql;
    //QSqlQuery sqlQuery;
    bool isOldPswRight;
    bool isNewPswRight;
    QString newPsw;

    QString calcMd5FromStr(QString plainPsw);


};

#endif // CHANGEPSW_H
