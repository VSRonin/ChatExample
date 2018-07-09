QT -= gui
QT += network

TARGET = QtSimpleChat
TEMPLATE = lib

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

DEFINES += QTSIMPLECHAT_LIBRARY QT_DEPRECATED_WARNINGS
SOURCES += \
    chatsession.cpp \
    chatmessage.cpp

HEADERS += \
        qtsimplechat.h \
    chatsession.h \
    chatmessage.h
