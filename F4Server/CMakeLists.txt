cmake_minimum_required(VERSION 3.10)
project(F4Server C)

set(CMAKE_C_STANDARD 11)

include_directories(inc)

add_executable(F4Server src/f4server.c src/f4logic.c inc/errExit.h src/commands.c inc/commands.h inc/f4logic.h src/errExit.c )

add_executable(F4ServerTest test/f4logic_test.c src/f4logic.c)