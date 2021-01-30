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
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

SOURCES += \
    main.cpp \
    game_core/about_dialog.cpp \
    game_core/game_object.cpp \
    game_core/mainwindow.cpp \
    game_core/map.cpp \
    game_core/new_game_dialog.cpp \
    game_core/settings_dialog.cpp \
    static_objects/portal.cpp \
    static_objects/static_object.cpp \
    movable_objects/boom.cpp \
    movable_objects/bot.cpp \
    movable_objects/clever_bot.cpp \
    movable_objects/improved_bot.cpp \
    movable_objects/movable.cpp \
    movable_objects/rocket.cpp \
    movable_objects/tank.cpp

HEADERS += \
    game_core/about_dialog.h \
    game_core/constants.h \
    game_core/game_object.h \
    game_core/mainwindow.h \
    game_core/map.h \
    game_core/new_game_dialog.h \
    game_core/settings_dialog.h \
    static_objects/portal.h \
    static_objects/static_object.h \
    movable_objects/boom.h \
    movable_objects/bot.h \
    movable_objects/clever_bot.h \
    movable_objects/improved_bot.h \
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

VERSION = 0.8.6.0
