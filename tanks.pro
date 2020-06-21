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
    map_objects/objectonmap.cpp \
    map_objects/portal.cpp \
    movables/boom.cpp \
    movables/bot.cpp \
    movables/cleverbot.cpp \
    movables/improvedbot.cpp \
    movables/movable.cpp \
    movables/rocket.cpp \
    movables/tank.cpp

HEADERS += \
    game_core/mainwindow.h \
    game_core/map.h \
    map_objects/objectonmap.h \
    map_objects/portal.h \
    movables/boom.h \
    movables/bot.h \
    movables/cleverbot.h \
    movables/improvedbot.h \
    movables/movable.h \
    movables/rocket.h \
    movables/tank.h

RESOURCES += resources/resources.qrc
CONFIG += resources_big

TRANSLATIONS += \
    resources/translations/tanks_be_BY.ts \
    resources/translations/tanks_en_US.ts \
    resources/translations/tanks_ru_RU.ts

RC_ICONS = resources/app_icon.ico

VERSION = 0.8.3.0
