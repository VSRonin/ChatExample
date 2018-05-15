QT += core gui widgets network

TARGET = QtSimpleChat
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
    serversocket.cpp

HEADERS += \
    serversocket.h

