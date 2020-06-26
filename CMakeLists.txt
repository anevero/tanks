cmake_minimum_required(VERSION 3.10)
project(Tanks)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON) # Enable Qt MOC compiler for managing signals / slots
set(CMAKE_AUTORCC ON) # Enable Qt resources compiler
set(CMAKE_AUTOUIC ON) # Enable Qt UI compiler
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -Wextra -O3")

set(CMAKE_PREFIX_PATH "~/Qt/5.15.0/gcc_64/lib/cmake")  # Linux
#set(CMAKE_PREFIX_PATH "C:/Qt/5.15.0/mingw81_64/lib/cmake")  # Windows

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)

set(SOURCES main.cpp
        game_core/mainwindow.cpp game_core/about_dialog.cpp
        game_core/new_game_dialog.cpp game_core/map.cpp
        movable_objects/boom.cpp movable_objects/bot.cpp
        movable_objects/cleverbot.cpp movable_objects/improvedbot.cpp
        movable_objects/movable.cpp movable_objects/rocket.cpp
        movable_objects/tank.cpp
        static_objects/objectonmap.cpp static_objects/portal.cpp)

qt5_add_big_resources(RESOURCES resources/resources.qrc)
add_executable(Tanks ${SOURCES} ${RESOURCES})

target_link_libraries(Tanks Qt5::Core)
target_link_libraries(Tanks Qt5::Widgets)
target_link_libraries(Tanks Qt5::Multimedia)
