QT += core network gui widgets

TARGET = QtSimpleChatServer
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += console

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
INCLUDEPATH += $$PWD/../QtSimpleChat
LIBS += -L$$OUT_PWD/../QtSimpleChat/$$DESTDIR -lQtSimpleChat

HEADERS += \
    chatserver.h \
    serverwindow.h

SOURCES += \
    chatserver.cpp \
    servermain.cpp \
    serverwindow.cpp

FORMS += \
    serverwindow.ui
