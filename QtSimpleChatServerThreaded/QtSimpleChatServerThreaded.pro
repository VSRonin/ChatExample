QT += core network gui widgets

TARGET = QtSimpleChatServerThreaded
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        servermain.cpp \
    chatserver.cpp \
    serverworker.cpp \
    serverwindow.cpp

HEADERS += \
    chatserver.h \
    chatserver.h \
    serverworker.h \
    serverwindow.h

CONFIG += debug_and_release

FORMS += \
    serverwindow.ui
