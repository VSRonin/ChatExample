QT += core network gui widgets

TARGET = QtSimpleChatServer
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += debug_and_release

HEADERS += \
    chatserver.h \
    serverworker.h \
    serverwindow.h

SOURCES += \
    chatserver.cpp \
    servermain.cpp \
    serverworker.cpp \
    serverwindow.cpp

FORMS += \
    serverwindow.ui
