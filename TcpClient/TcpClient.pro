QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    protocol.cpp \
    tcpclient.cpp \
    opewidget.cpp \
    online.cpp \
    friend.cpp \
    book.cpp \
    privatechat.cpp \
    sharefile.cpp

HEADERS += \
    protocol.h \
    tcpclient.h \
    opewidget.h \
    online.h \
    friend.h \
    book.h \
    privatechat.h \
    sharefile.h

FORMS += \
    tcpclient.ui \
    online.ui \
    privatechat.ui

TRANSLATIONS += \
    TcpClient_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    config.qrc \
    filetype.qrc
