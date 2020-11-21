cmake_minimum_required(VERSION 3.10)
project(Tanks)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_AUTOMOC ON) # Enable Qt MOC compiler
set(CMAKE_AUTORCC ON) # Enable Qt resources compiler
set(CMAKE_AUTOUIC ON) # Enable Qt UI compiler

# You can set CMake prefix path here or pass it to CMake executable.
#set(CMAKE_PREFIX_PATH "~/Qt/5.15.2/gcc_64/lib/cmake")
#set(CMAKE_PREFIX_PATH "C:/Qt/5.15.2/mingw81_64/lib/cmake")

if (NOT CMAKE_PREFIX_PATH)
    message(WARNING "CMAKE_PREFIX_PATH is not defined, you may need to set it "
            "(-DCMAKE_PREFIX_PATH=\"path/to/Qt/lib/cmake\")")
endif ()

set(QT_VERSION 5)
set(REQUIRED_LIBS Core Widgets Multimedia)
set(REQUIRED_LIBS_QUALIFIED Qt5::Core Qt5::Widgets Qt5::Multimedia)
find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -O3")

set(SOURCES main.cpp
        game_core/mainwindow.cpp game_core/about_dialog.cpp
        game_core/new_game_dialog.cpp game_core/settings_dialog.cpp
        game_core/map.cpp game_core/game_object.cpp
        movable_objects/boom.cpp movable_objects/bot.cpp
        movable_objects/clever_bot.cpp movable_objects/improved_bot.cpp
        movable_objects/movable.cpp movable_objects/rocket.cpp
        movable_objects/tank.cpp
        static_objects/static_object.cpp static_objects/portal.cpp)

qt5_add_big_resources(RESOURCES resources/resources.qrc)
add_executable(${PROJECT_NAME} ${SOURCES} ${RESOURCES})
target_link_libraries(${PROJECT_NAME} ${REQUIRED_LIBS_QUALIFIED})
