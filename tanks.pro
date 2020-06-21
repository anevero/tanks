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
    game_core/mainwindow.cpp \
    game_core/map.cpp \
    static_objects/objectonmap.cpp \
    static_objects/portal.cpp \
    movable_objects/boom.cpp \
    movable_objects/bot.cpp \
    movable_objects/cleverbot.cpp \
    movable_objects/improvedbot.cpp \
    movable_objects/movable.cpp \
    movable_objects/rocket.cpp \
    movable_objects/tank.cpp

HEADERS += \
    game_core/mainwindow.h \
    game_core/map.h \
    static_objects/objectonmap.h \
    static_objects/portal.h \
    movable_objects/boom.h \
    movable_objects/bot.h \
    movable_objects/cleverbot.h \
    movable_objects/improvedbot.h \
    movable_objects/movable.h \
    movable_objects/rocket.h \
    movable_objects/tank.h

RESOURCES += resources/resources.qrc
CONFIG += resources_big

TRANSLATIONS += \
    resources/translations/tanks_be_BY.ts \
    resources/translations/tanks_en_US.ts \
    resources/translations/tanks_ru_RU.ts

RC_ICONS = resources/app_icon.ico

VERSION = 0.8.3.0
