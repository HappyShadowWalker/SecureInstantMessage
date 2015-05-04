#ifndef SIMSYSTEMTRAY_H
#define SIMSYSTEMTRAY_H
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QWidgetAction>
#include <QLabel>
//实现SIM程序自定义托盘菜单
class SimSystemTray: public QSystemTrayIcon
{
    Q_OBJECT
public:
    SimSystemTray(QWidget *parent=0) ;
    void showMyself();
signals:
    void selfStart();
    void restore();
    void setting();
    //void exitSIM();
protected:
    bool eventFilter(QObject *, QEvent *);
private:
    QMenu* trayMenu;
    //定义顶部
    QWidget * topWidget;
    QWidgetAction * topWidgetAction;
    QLabel * topLabel;
    //
    QAction * selfStartAction;
    QAction * restoreAction;
    QAction * settingAction;
    QAction * exitAction;
    void createTopWidget();
    void createContextMenu();


};
#endif // SIMSYSTEMTRAY_H
