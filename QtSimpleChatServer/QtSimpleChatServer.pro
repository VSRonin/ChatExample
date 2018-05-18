QT += core network

TARGET = QtSimpleChatServer
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += debug_and_release console

HEADERS += \
    chatserver.h \
    serverworker.h

SOURCES += \
    chatserver.cpp \
    servermain.cpp \
    serverworker.cpp
