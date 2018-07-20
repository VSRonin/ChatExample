include(../QtSimpleChat.pri)

QT += core network gui widgets

TARGET = QtSimpleChat
TEMPLATE = lib

DEFINES += QTSIMPLECHAT_LIBRARY QT_DEPRECATED_WARNINGS

SOURCES += \
    chatsession.cpp \
    chatmessage.cpp \
    chatserver.cpp \
    serverwindow.cpp

HEADERS += \
    qtsimplechat.h \
    chatsession.h \
    chatmessage.h \
    chatserver.h \
    serverwindow.h

FORMS += \
    serverwindow.ui
