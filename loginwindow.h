#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QPixmap>
#include <QXmlReader>
#include <QXmlStreamReader>
#include <QDomDocument>
#include "sysButton.h"
#include "mainwidget.h"
#include "changepsw.h"
namespace Ui {
class LoginWindow;
}

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = 0);
    ~LoginWindow();

private slots:
    void on_registerAccBtn_clicked();

    void on_loginPushBtn_clicked();
    void showAccount(QString accText);
    void removeAccount(QString accText);
    void closeLoginWindow();
    //checkBoxSlot
    void slotRememberPswState(int state);
    void slotAutoLogin(int state);

    void on_changePswPushBtn_clicked();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void enterEvent(QEvent * e);
    bool eventFilter(QObject *, QEvent *);

private:
    Ui::LoginWindow *ui;
    //最近登录过的账号列表
    QListWidget* accListWidget;
    QString currentLoginAcc;
    //背景图片
    QPixmap * loginBackGround;
    QString  bgPicPath;

    void simpleDecorateWindow();
    void drawBackgroundPic();
    QPoint windowPos;
    QPoint mousePos;
    QPoint dPos;
    bool isMove;
    //
    mainWidget* mw;
    sysButton * btnClose;

    //实现登录界面中的记住密码和自动登录功能
    bool isInRememberPsw;
    bool isInAutoLogin;
    QXmlStreamReader settingXmlReader;
    QFile settingFile;
    void readSettingFromXml(QString fileName);
    QString curMd5Psw;
    QString curAccFromXml;

    //读写配置文件
    QDomDocument configDomFile;
    QString configFilePath;
    void changeConfigValue(QString configFilePath,QString key ,QString value);

    //美化登录界面
    void beautifyLoginWindow();

    ChangePsw * changePsw;

};

#endif // LOGINWINDOW_H
