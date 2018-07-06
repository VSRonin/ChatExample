QT += core network gui widgets

TARGET = QtSimpleChatServer
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += console

INCLUDEPATH += $$PWD/../QtSimpleChat
LIBS += -L$$OUT_PWD -lQtSimpleChat

HEADERS += \
    chatserver.h \
    serverwindow.h

SOURCES += \
    chatserver.cpp \
    servermain.cpp \
    serverwindow.cpp

FORMS += \
    serverwindow.ui
