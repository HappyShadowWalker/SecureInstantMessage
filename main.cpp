#include "loginwindow.h"
#include <QApplication>
#include <statechangewidget.h>
#include "SimSystemTray.h"
#include<QFile>
#include "mainwidget.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginWindow lw;
    lw.show();
    return a.exec();
}
