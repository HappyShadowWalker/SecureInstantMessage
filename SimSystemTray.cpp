#include "SimSystemTray.h"
#include <QEvent>
#include <QVBoxLayout>
#include <QPainter>
#include <QIcon>
#include <QApplication>
#include <QFile>
#include <QTextStream>
SimSystemTray::SimSystemTray(QWidget *parent):QSystemTrayIcon(parent){
    trayMenu=new QMenu();
    QFile menuStyleFile(":/StyleSheet/img/StyleSheet/SimStyleSheet.qss");
    if (!menuStyleFile.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

    QTextStream fileData(&menuStyleFile);
    QString menuStyleStr=fileData.readAll();

    trayMenu->setStyleSheet(menuStyleStr);
    this->setIcon(QIcon("://img/qq.ico"));
    createTopWidget();
    createContextMenu();
    this->setContextMenu(trayMenu);

    //定义信号连接
    connect(restoreAction,SIGNAL(toggled(bool)),this,SIGNAL(restore()));
    connect(settingAction,SIGNAL(triggered(bool)),this,SIGNAL(setting()));
    connect(exitAction,SIGNAL(triggered(bool)),qApp,SLOT(quit()));
    connect(selfStartAction,SIGNAL(triggered()),this,SIGNAL(selfStart()));
}
//重新绘制顶层部件
bool SimSystemTray::eventFilter(QObject * target, QEvent * e){
    if(target==topWidget&&e->type()==QEvent::Paint){
        QPainter painter(topWidget);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(42, 120, 192));
        painter.drawRect(topWidget->rect());
    }
    return QSystemTrayIcon::eventFilter(target,e);
}

void SimSystemTray::createTopWidget(){

    topWidget=new QWidget();
    topWidgetAction=new QWidgetAction(this->trayMenu);
    topLabel=new QLabel("Secure Instant Message");
    QVBoxLayout* m_topLayout = new QVBoxLayout();

    m_topLayout->addWidget(topLabel, 0, Qt::AlignLeft|Qt::AlignVCenter);

    topWidget->setLayout(m_topLayout);
    topWidget->installEventFilter(this);
    topWidgetAction->setDefaultWidget(topWidget);
    trayMenu->addAction(topWidgetAction);
    trayMenu->addSeparator();

}

void SimSystemTray::createContextMenu(){
     selfStartAction=new QAction("开机启动",this);
     selfStartAction->setIcon(QIcon(":/SIMSystemTray/img/SystemTray/SelfStart.png"));
     restoreAction=new QAction("打开主面板",this);
     restoreAction->setIcon(QIcon(":/SIMSystemTray/img/SystemTray/Restore.png"));
     settingAction=new QAction("设置",this);
     settingAction->setIcon(QIcon(":/SIMSystemTray/img/SystemTray/Setting.png"));
     exitAction=new QAction("退出",this);
     exitAction->setIcon(QIcon(":/SIMSystemTray/img/SystemTray/Exit.png"));

     trayMenu->addAction(selfStartAction);
     trayMenu->addSeparator();
     trayMenu->addAction(restoreAction);
     trayMenu->addAction(settingAction);
     trayMenu->addSeparator();
     trayMenu->addAction(exitAction);

}

void SimSystemTray::showMyself(){
    this->show();
}
