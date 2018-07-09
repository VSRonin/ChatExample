QT += core network gui widgets

TARGET = QtSimpleChatServerThreaded
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG(release, debug|release){
    DESTDIR = release
    OBJECTS_DIR = release/.obj
    MOC_DIR = release/.moc
    RCC_DIR = release/.rcc
    UI_DIR = release/.ui
}
CONFIG(debug, debug|release){
    DESTDIR = debug
    OBJECTS_DIR = debug/.obj
    MOC_DIR = debug/.moc
    RCC_DIR = debug/.rcc
    UI_DIR = debug/.ui
}

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
