#-------------------------------------------------
#
# Project created by QtCreator 2014-03-16T00:26:35
#
#-------------------------------------------------

QT       += core gui sql network xml webkit webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SecureInstantMessage
TEMPLATE = app


SOURCES += main.cpp\
        loginwindow.cpp \
    mainwidget.cpp \
    sysButton.cpp \
    skinpushbutton.cpp \
    changeskinwidget.cpp \
    privatechat.cpp \
    shakewindow.cpp \
    fadeinout.cpp \
    accountitem.cpp \
    registerwindow.cpp \
    tipimgpushbtn.cpp \
    tiptitle.cpp \
    statechangewidget.cpp \
    useritem.cpp \
    emotionwidget.cpp \
    filereceivewidget.cpp \
    screenshotwidget.cpp \
    screenimgeditor.cpp \
    tinyaes.cpp \
    SimSystemTray.cpp \
    Voice/buffer.cpp \
    Voice/voiceio.cpp \
    Voice/voicesocket.cpp \
    changepsw.cpp

HEADERS  += loginwindow.h \
    mainwidget.h \
    sysButton.h \
    skinpushbutton.h \
    changeskinwidget.h \
    privatechat.h \
    sharedefine.h \
    shakewindow.h \
    fadeinout.h \
    accountitem.h \
    registerwindow.h \
    tipimgpushbtn.h \
    tiptitle.h \
    statechangewidget.h \
    useritem.h \
    emotionwidget.h \
    filereceivewidget.h \
    screenshotwidget.h \
    screenimgeditor.h \
    tinyaes.h \
    SimSystemTray.h \
    Voice/buffer.h \
    Voice/voiceio.h \
    Voice/voicesocket.h \
    changepsw.h

FORMS    += loginwindow.ui \
    mainwidget.ui \
    privatechat.ui \
    registerwindow.ui \
    useritem.ui \
    filereceivewidget.ui \
    changepsw.ui

RESOURCES += \
    simImgResource.qrc

RC_ICONS = SIM.ico
CONFIG +=static
