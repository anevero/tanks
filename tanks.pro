greaterThan(QT_MAJOR_VERSION, 4):
QT += widgets core gui multimedia

TARGET = Tanks
TEMPLATE = app
CONFIG += c++11
DEFINES += QT_DEPRECATED_WARNINGS

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android-sources
DISTFILES += \
android-sources/AndroidManifest.xml

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    map.cpp \
    tank.cpp \
    movable.cpp \
    rocket.cpp \
    bot.cpp \
    improvedbot.cpp \
    cleverbot.cpp \
    objectonmap.cpp \
    boom.cpp \
    portal.cpp

HEADERS += \
    mainwindow.h \
    map.h \
    tank.h \
    movable.h \
    rocket.h \
    bot.h \
    improvedbot.h \
    cleverbot.h \
    objectonmap.h \
    boom.h \
    portal.h

RESOURCES += \
    resources.qrc

TRANSLATIONS += \
    translations/tanks_be_BY.ts \
    translations/tanks_en_US.ts \
    translations/tanks_ru_RU.ts

RC_ICONS = app_icon.ico

VERSION = 0.8.0.0
