greaterThan(QT_MAJOR_VERSION, 4):
QT += widgets core gui multimedia

android {
QT += androidextras
}

DEFINES += QT_DEPRECATED_WARNINGS

TARGET = Tanks
TEMPLATE = app
CONFIG += c++17
QMAKE_CXXFLAGS += -std=c++17
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3


ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android-sources/res/values/libs.xml

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

RESOURCES += resources.qrc
CONFIG += resources_big

TRANSLATIONS += \
    translations/tanks_be_BY.ts \
    translations/tanks_en_US.ts \
    translations/tanks_ru_RU.ts

RC_ICONS = app_icon.ico

VERSION = 0.8.3.0
