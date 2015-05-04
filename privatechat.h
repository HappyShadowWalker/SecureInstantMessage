#ifndef PRIVATECHAT_H
#define PRIVATECHAT_H

#include <QWidget>
#include <QPoint>
#include <QUdpSocket>
#include <QString>
#include <QHostAddress>
#include <QFont>
#include <QWebView>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QDragEnterEvent>
#include <QDropEvent>
#include "shakewindow.h"
#include "sharedefine.h"
#include "sysButton.h"
#include "fadeinout.h"
#include "emotionwidget.h"
#include "tinyaes.h"
//添加实现语音通信的头文件
#include "Voice/buffer.h"
#include "Voice/voiceio.h"
#include "Voice/voicesocket.h"
namespace Ui {
class privatechat;
}

class privatechat : public QWidget
{
    Q_OBJECT

public:
    explicit privatechat(QString targetAcc,QHostAddress targetIp,QString targetImgPath, QWidget *parent = 0);


    ~privatechat();

protected:
    //实现窗体的可以自由拖放
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    bool eventFilter(QObject * targetObj, QEvent * e);//安装事件过滤器实现Ctrl Enter发送消息

    //实现文件的拖放发送
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent * event);
private slots:
    void closeMainWidget();
    void minMainWidget();
    void receiveMsg();
    void btnSendClick();
    void on_shakeToolBtn_clicked();
    void on_fontComboBox_currentFontChanged(const QFont &f);
    void on_blackToolBtn_clicked();
    void on_inclineToolBtn_clicked();
    void on_snapScreenToolBtn_clicked();
    void insertEmotion(QString emotionImgPath);
    void on_emotionToolBtn_clicked();
    void closeEmotionWidget();
    void on_sendFileBtn_clicked();
    //更新发送文件进度条进度
    void updateFileSendProgress(qint64);
    void startSendFile();
    void displayError(QAbstractSocket::SocketError);

    void on_voiceChatToolBtn_clicked();
    void on_fontSizeComboBox_currentIndexChanged(int index);

    void on_closePushBtn_clicked();

private:
    Ui::privatechat *ui;
    //为了实现AES加密解密
    TinyAES aesCrypto;
    //QString keyString="587b14e9ef30e95b64dc5ec71230197a";
    //为了配合窗体移动记录窗体的位置
    QPoint windowPos;
    QPoint mousePos;
    QPoint moveDisPos;

    //添加窗体右上角的最小化 最大化 关闭等按钮
    sysButton* btnMin;
    sysButton* btnClose;
    sysButton* btnMax;

    //为了实现消息接收和发送定义UdpSocket
    QUdpSocket* msgSendUdpSocket;
    QUdpSocket* msgReceiveUdpSocket;
    //定义变量保存当前聊天对方的信息
    QHostAddress targetIp;
    QString targetAcc;
    QString targetImgPath;

    QString msgEditContent;//当前编辑框里的内容
    void sendMsg(MessageType msgType);
    QString formateSendMsgToShow(QString plainMsg);
    QString formateReceiveMsgToShow(QString plainMsg);

    // 为了实现PrivateChat里的工具按钮定义的私有变量
    QFont currentFont;
    int fontSize;
    bool isBlack=false;
    bool isIncline=false;
    //抖动窗口
    shakewindow * shake;
    //实现窗口淡入淡出
    fadeinout * fadeInOutWidget;
    //添加表情框实现代码
    EmotionWidget * emotionWidget;
    QString plainEditText;
    //添加实现文件传递的代码
    QTcpServer* fileListenServer;
    QTcpSocket* fileSendSocket;
    QTcpSocket* fileReceiveSocket;
    QString fileName;
    QFile* localFile;
    qint64 totalBytes;
    qint64 bytesWritten;
    qint64 bytesToWrite;
    qint64 loadSize;
    QByteArray outBlock;
    //更改聊天窗口工具栏的样式，使其美观
    void changePrivateChatStyle();
    //添加实现语音通信的成员变量
    VoiceIO *vio;
    VoiceSocket *vso;
    Buffer *buf1;
    Buffer *buf2;
    //初始化语音通信的变量,在PrivateChat 构造函数中
    void IniVoiceChatSetting();
    //定义一个Bool变量来存储用户是否在语音通话状态中
    bool isInVoiceTalk;
    QString curLoginAcc="ShadowWalker";
    //美化私聊界面窗口
    void beautifyMainWidget();
    void addTooBtns();
    void iniPrivateChatSetting();//初始化私聊界面设置

};

#endif // PRIVATECHAT_H
