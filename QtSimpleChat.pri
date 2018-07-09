CONFIG(release, debug|release): DESTDIR = $$PWD/release
CONFIG(debug, debug|release): DESTDIR = $$PWD/debug

OBJECTS_DIR = $$DESTDIR/.obj/$$TARGET
MOC_DIR = $$DESTDIR/.moc/$$TARGET
RCC_DIR = $$DESTDIR/.rcc/$$TARGET
UI_DIR = $$DESTDIR/.ui/$$TARGET
