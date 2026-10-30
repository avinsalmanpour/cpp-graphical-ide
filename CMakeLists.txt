cmake_minimum_required(VERSION 3.29)

project(cpp-graphical-ide)

set(CMAKE_CXX_STANDARD 20)

add_executable(cpp-graphical-ide
        cpp-graphical-ide.cpp)

target_link_libraries(cpp-graphical-ide
        -lconio)

target_link_libraries(cpp-graphical-ide
        -lmingw32
        -lSDL2main
        -lSDL2
        -lSDL2_image
        -lSDL2_ttf
        -lSDL2_mixer)
