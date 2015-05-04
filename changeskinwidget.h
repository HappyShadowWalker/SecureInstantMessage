#ifndef CHANGESKINWIDGET_H
#define CHANGESKINWIDGET_H

#include <QWidget>

class changeSkinWidget : public QWidget
{
    Q_OBJECT
public:
    explicit changeSkinWidget(QString picName,QWidget *parent=0 );

signals:
    void changeSkin(QString);


protected:
     void paintEvent(QPaintEvent *);
private slots:
    void setSkin(QString);

private:
    QString bgPicName;

};

#endif // CHANGESKINWIDGET_H
