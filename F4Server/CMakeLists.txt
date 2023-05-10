cmake_minimum_required(VERSION 3.24)
project(F4Server C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

add_executable(F4Server src/f4server.c src/f4logic.c include/errExit.h src/commands.c include/commands.h include/f4logic.h src/errExit.c)
