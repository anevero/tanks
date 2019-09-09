cmake_minimum_required(VERSION 3.12)
project(Tanks)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Specify location according to Qt installation path.
set(CMAKE_PREFIX_PATH "~/Qt/5.13.0/gcc_64/lib/cmake")
# set(CMAKE_PREFIX_PATH "C:/Qt/5.12.3/mingw73_64/lib/cmake")

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)

add_executable(Tanks main.cpp mainwindow.cpp mainwindow.h boom.cpp boom.h
        bot.cpp bot.h cleverbot.cpp cleverbot.h improvedbot.cpp improvedbot.h
        map.cpp map.h movable.cpp movable.h objectonmap.cpp objectonmap.h
        portal.cpp portal.h rocket.cpp rocket.h tank.cpp tank.h resources.qrc)

target_link_libraries(Tanks Qt5::Core)
target_link_libraries(Tanks Qt5::Widgets)
target_link_libraries(Tanks Qt5::Multimedia)
