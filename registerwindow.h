#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QWidget>
#include <QSqlDatabase>
#include <QString>
#include <QLabel>
#include <QTime>
#include "sysButton.h"

namespace Ui {
class registerwindow;
}

class registerwindow : public QWidget
{
    Q_OBJECT

public:
    explicit registerwindow(QWidget *parent = 0);
    ~registerwindow();
    //连接数据库
    bool connectDataBase(QString serverName,QString dbName);

    //实现窗体上边角圆弧
    void frameLessWidget();
protected:
   void mousePressEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * e);

   // bool eventFilter(QObject *, QEvent *);

    void paintEvent(QPaintEvent *);
private slots:
    void on_registerPushBtn_clicked();
    void closeRWWiget();

    void on_changeVerCodePushBtn_clicked();

    void on_acountLineEdit_editingFinished();

    void on_pswLineEdit_editingFinished();

    void on_repeatPswLineEdit_editingFinished();

    void on_verCodeLineEdit_editingFinished();

private:
    Ui::registerwindow *ui;
    QString serverName;
    QString dbName;
    QSqlDatabase myDb;

    //
    QPoint windowPos;
    QPoint mousePos;
    QPoint dPos;
    bool isMove;

    //
    sysButton * btnClose;
    QString mainWidgetPicName;//背景图片

    //
    QString verCode;
    void paintVerCode(QString verCode,QLabel * target);

    void iniVerCode();
    //定义两个Bool 变量维护输入的账号和密码是否有效
    bool isValidAcc;
    bool isValidPsw;
    bool isRightVerCode;
};

#endif // REGISTERWINDOW_H
