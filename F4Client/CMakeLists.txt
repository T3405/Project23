cmake_minimum_required(VERSION 3.10)
project(F4Client C)

set(CMAKE_C_STANDARD 11)

find_package(Curses REQUIRED)

include_directories(../F4Server/inc/ inc/)

add_executable(F4Client src/main.c ../F4Server/src/ioutils.c ../F4Server/src/commands.c inc/ui.h src/ui.c inc/test.h)
target_link_libraries(F4Client)