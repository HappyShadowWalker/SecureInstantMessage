#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QSystemTrayIcon>
#include <QUdpSocket>
#include <QBitmap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QXmlStreamReader>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include "tinyaes.h"
#include "privatechat.h"
#include "sysButton.h"
#include "skinpushbutton.h"
#include "useritem.h"
#include "emotionwidget.h"//添加表情框
#include "filereceivewidget.h"
#include "SimSystemTray.h"
namespace Ui {
class mainWidget;
}

class mainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit mainWidget(QString userName=" ",QWidget *parent = 0);
    ~mainWidget();

private:
    Ui::mainWidget *ui;
    QString mainWidgetPicName;//主窗口背景图片的名称
    qint16 port;
    QPoint windowPos;
    QPoint mousePos;
    QPoint dPos;
    bool isMove;

    QSqlDatabase  myDb;//连接数据库
    QString serverName="ShadowWalker";
    QString dbName="InstantMessage";
    bool connectDatabase(QString serverName,QString databaseName);
    bool readUserSettings(QString currLoginAcc);
    bool readUserSettingsFromXml(QString currLoginAcc,QString settingXmlPath);
    void parseElementFromXml(QXmlStreamReader& xmlReader);
    void setTreeWidget();
    //从数据库读取好友的信息,更新好友列表
    void readFriendInfoFromDB(QString friendAcc,QString friendIP);

    //添加监听文件接收函数
    QTcpServer * fileListenServer;
    QTcpSocket * fileReceiveSocket;
    qint64 totalBytes;  //存放总大小信息
    qint64 bytesReceived;  //已收到数据的大小
    qint64 fileNameSize;  //文件名的大小信息
    QString fileName;   //存放文件名
    QFile *localFile;   //本地文件
    QByteArray inBlock;   //数据缓冲区
    FileReceiveWidget * fileReceiveWidget;

    //添加好友发送消息的监听，比如好友登录或下线或发送消息
    QUdpSocket * msgListenUdpSocket;
    QUdpSocket * msgSendUdpSocket;
    TinyAES aesCrypto;//进行消息加密
    //发送消息
    void sendMessage(MessageType,QString ,QString );
    //定义存储好友列表的数据结构
    QMap<QString ,QStringList> friendsInfoMap;
    QStringList friendsImgPath,friendsSignedName,friendsAcc,friendsIP;
    //实现自定义托盘菜单
    SimSystemTray* simSystemTray;
    //发送文件请求回复消息
    QHostAddress currentTargetIp;
    void sendFileRequestAnswer(MessageType msgType,QString msgContent,QHostAddress targetIP);

    //实施主界面的准备工作
    void prepareWork(QString currUser);
    //保存当前登录的账号信息
    QString currLoginAcc;
    QHostAddress currAccIp;
    QString currUserSignedName;
    QString currUserHeadImg;
    //必须定义一个当前用户信息的数据结构来组织信息了
    QMap<QString ,QStringList> curUserInfoMap;

    //最后定义个函数实现主窗体的美化吧
    void beaurifyMainWidget();




public:
    //定义主界面上的几个按钮包括换肤按钮，最小化按钮等
    skinPushButton* btnSkin;
    sysButton* btnMenuBar;
    sysButton* btnMin;
    sysButton* btnClose;
    bool isInGroupChat;
    QSystemTrayIcon* appTrayIcon;//托盘图标
    QString headImg;
    QString myFriendGroupName;
    QString myClassGroupName;
    QString myBlackListName;
    QString myStrangerName;

    void InitSetting();
    QTreeWidgetItem * itemMyFriend;
    QHostAddress getCurUserIp();//获取当前用户IP地址
    void newUser(QString userName,QString localHostName,QString ipAddress);//新用户加入
    void Userleave(QString userName,QString localHostName,QString time);//用户离开


protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    //窗体绘制函数
    void paintEvent(QPaintEvent *);
    //Enter键搜索
    bool eventFilter(QObject *, QEvent *);

private slots:
    void minMainWindow();
    void closeMainWindow();
    void changeMainWidgetPic(QString);
    void openSkinWidgetWindow();//点击换肤按钮
    //好友列表单击响应函数
    void onTreeItemDoubleClick(QTreeWidgetItem * itemClickedTW,int i);
    //实现文件传送功能
    void acceptTcpConnection();
    void denyTcpConnection();
    void beginReceiveFile();
    void receiveFile();
    void readMsg();

    //托盘菜单的响应函数
    void iconIsActived(QSystemTrayIcon::ActivationReason reason);
    //托盘菜单项的响应函数
    void restoreMainWidget();
    void selfStart();
    void openSettings();

    void on_searchToolBtn_clicked();
};

#endif // MAINWIDGET_H
