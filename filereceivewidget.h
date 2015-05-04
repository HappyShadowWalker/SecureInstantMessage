#ifndef FILERECEIVEWIDGET_H
#define FILERECEIVEWIDGET_H

#include <QWidget>
//实现文件接收提示
namespace Ui {
class FileReceiveWidget;
}

class FileReceiveWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileReceiveWidget(QWidget *parent = 0);
    void setProgressBarValue(int value);
    void setProgressBarMaxValue(int max);
    ~FileReceiveWidget();
signals:
   void  denyClicked();
   void receiveClicked();

private:
    Ui::FileReceiveWidget *ui;
};

#endif // FILERECEIVEWIDGET_H
