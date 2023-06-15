cmake_minimum_required(VERSION 3.10)
project(F4Client C)

set(CMAKE_C_STANDARD 11)

find_package(Curses REQUIRED)

include_directories(../F4Server/inc/ inc/ ${CURSES_INCLUDE_DIR})

add_executable(F4Client src/main.c inc/ui.h src/ui.c ../F4Server/src/ioutils.c ../F4Server/src/commands.c)
target_link_libraries(F4Client ${CURSES_LIBRARIES})