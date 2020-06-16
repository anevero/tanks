cmake_minimum_required(VERSION 3.10)
project(Tanks)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON) # Enable Qt MOC compiler for managing signals / slots
set(CMAKE_AUTORCC ON) # Enable Qt resources compiler
set(CMAKE_AUTOUIC ON) # Enable Qt UI compiler
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -Wextra -O3")

set(CMAKE_PREFIX_PATH "~/Qt/5.15.0/gcc_64/lib/cmake")  # Linux
#set(CMAKE_PREFIX_PATH "C:/Qt/5.15.0/mingw73_64/lib/cmake")  # Windows

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)

set(SOURCES main.cpp mainwindow.cpp mainwindow.h boom.cpp boom.h
        bot.cpp bot.h cleverbot.cpp cleverbot.h improvedbot.cpp improvedbot.h
        map.cpp map.h movable.cpp movable.h objectonmap.cpp objectonmap.h
        portal.cpp portal.h rocket.cpp rocket.h tank.cpp tank.h)

qt5_add_big_resources(SOURCES resources.qrc)
add_executable(Tanks ${SOURCES})

target_link_libraries(Tanks Qt5::Core)
target_link_libraries(Tanks Qt5::Widgets)
target_link_libraries(Tanks Qt5::Multimedia)
