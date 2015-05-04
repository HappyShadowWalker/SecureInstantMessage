#ifndef EMOTIONWIDGET_H
#define EMOTIONWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QSignalMapper>
#include <QFile>
//实现聊天表情框
class EmotionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EmotionWidget(QWidget *parent = 0);

signals:
    void emotionTrigger(QString emotionImgPath);
    void emotionClick();

public slots:

private:
    QSignalMapper* signalMap;
    QPushButton* btnEmotion;
    QGridLayout* gridLayout;
    QStringList emotionImgPaths;
    QStringList emotionTipPaths;

    void readEmotionPathFromFile(QString emotionFilePath);


};

#endif // EMOTIONWIDGET_H
