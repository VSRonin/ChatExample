QT += core network

TARGET = QtSimpleChatServerThreaded
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        servermain.cpp \
    chatserver.cpp \
    serverworker.cpp

HEADERS += \
    chatserver.h \
    chatserver.h \
    serverworker.h

CONFIG += debug_and_release console
