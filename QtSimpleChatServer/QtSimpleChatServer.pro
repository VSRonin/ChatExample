include(../QtSimpleChat.pri)

QT += core network gui widgets

TARGET = QtSimpleChatServer
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += $$PWD/../QtSimpleChat
LIBS += -L$$DESTDIR -lQtSimpleChat

HEADERS += \
    chatserver.h \
    serverwindow.h

SOURCES += \
    chatserver.cpp \
    servermain.cpp \
    serverwindow.cpp

FORMS += \
    serverwindow.ui
