#include "accountitem.h"
#include <QPixmap>
#include <QHBoxLayout>
#include <QMouseEvent>
AccountItem::AccountItem(QWidget *parent) :
    QWidget(parent)
{
    isMousePress=false;
    labelAcc=new QLabel();
    btnDelete=new QPushButton();

    QPixmap pixMap(":/img/LoginWidget/delete.png");
    btnDelete->setIcon(pixMap);
    btnDelete->setIconSize(pixMap.size());
    btnDelete->setStyleSheet("background:transparent");
    connect(btnDelete,SIGNAL(clicked()),this,SLOT(removeAccount()));
    QHBoxLayout *main_layout = new QHBoxLayout();
    main_layout->addWidget(labelAcc);
    main_layout->addStretch();
    main_layout->addWidget(btnDelete);
    main_layout->setContentsMargins(5, 5, 5, 5);
    main_layout->setSpacing(5);
    this->setLayout(main_layout);

}

void AccountItem::setAccountText(QString accText){
    //labelAcc->text()=accText;
    labelAcc->setText(accText);
}

QString AccountItem::getAccountText(){
    QString tempAccText=labelAcc->text();
    return tempAccText;
}

void AccountItem::removeAccount(){
    QString tempAccText=labelAcc->text();
    emit removeAccount(tempAccText);
}

void AccountItem::mousePressEvent(QMouseEvent *e){
    if(e->button()==Qt::LeftButton){
        isMousePress=true;
    }
}

void AccountItem::mouseReleaseEvent(QMouseEvent *e){
    if(isMousePress){
        emit showAccount(labelAcc->text());
        isMousePress=false;
    }
}
