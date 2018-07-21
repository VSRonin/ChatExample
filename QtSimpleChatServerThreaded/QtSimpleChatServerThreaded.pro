include(../QtSimpleChat.pri)

QT += core network gui widgets

TARGET = QtSimpleChatServerThreaded
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += $$PWD/../QtSimpleChat
LIBS += -L$$DESTDIR -lQtSimpleChat

SOURCES += \
    serverthreadedmain.cpp \
    chatserverthreaded.cpp \
    threadpool.cpp

HEADERS += \
    chatserverthreaded.h \
    threadpool.h
