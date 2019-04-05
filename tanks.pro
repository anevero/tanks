QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tanks
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
    improvedbot.cpp

HEADERS += \
    mainwindow.h \
    map.h \
    tank.h \
    movable.h \
    rocket.h \
    bot.h \
    improvedbot.h

DISTFILES += \
    maps/map1.txt \
    maps/map2.txt \
    maps/map3.txt \
    maps/map4.txt

RESOURCES += \
    resources.qrc

RC_ICONS = app_icon.ico
