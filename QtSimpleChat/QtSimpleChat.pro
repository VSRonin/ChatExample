include(../QtSimpleChat.pri)

QT -= gui
QT += network

TARGET = QtSimpleChat
TEMPLATE = lib

DEFINES += QTSIMPLECHAT_LIBRARY QT_DEPRECATED_WARNINGS

SOURCES += \
    chatsession.cpp \
    chatmessage.cpp

HEADERS += \
    qtsimplechat.h \
    chatsession.h \
    chatmessage.h
