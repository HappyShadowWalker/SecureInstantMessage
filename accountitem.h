#ifndef ACCOUNTITEM_H
#define ACCOUNTITEM_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QString>
class AccountItem : public QWidget
{
    Q_OBJECT
public:
    explicit AccountItem(QWidget *parent = 0);
    void setAccountText(QString accText);
    QString getAccountText();
signals:
    void showAccount(QString );
    void removeAccount(QString);

protected:
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent * e);

public slots:
     void removeAccount();
private:
    bool isMousePress;
    QLabel* labelAcc;
    QPushButton* btnDelete;
    //
};

#endif // ACCOUNTITEM_H
