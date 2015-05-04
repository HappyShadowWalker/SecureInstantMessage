#ifndef USERITEM_H
#define USERITEM_H

#include <QWidget>
//此类被QTreeWidgetItem使用,用于实现好友列表,代表好友列表中的单个好友
namespace Ui {
class UserItem;
}

class UserItem : public QWidget
{
    Q_OBJECT

public:
    explicit UserItem(QString userAcc,QString userSignedName,QString userImgPath,QWidget *parent = 0);
    //返回UserAcc;
    QString getUserAcc();
    ~UserItem();

private:
    Ui::UserItem *ui;
    QString userAcc;
};

#endif // USERITEM_H
