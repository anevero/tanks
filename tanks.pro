QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Tanks
TEMPLATE = app
CONFIG += c++11
DEFINES += QT_DEPRECATED_WARNINGS

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
    obstaclandbonus.cpp

HEADERS += \
    mainwindow.h \
    map.h \
    tank.h \
    movable.h \
    rocket.h \
    bot.h \
    improvedbot.h \
    cleverbot.h \
    obstaclandbonus.h

RESOURCES += \
    resources.qrc

RC_ICONS = app_icon.ico

VERSION = 0.3.0.0
