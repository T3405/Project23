cmake_minimum_required(VERSION 3.24)
project(Project C)

set(CMAKE_C_STANDARD 11)

add_executable(Project src/f4server.c headers/commands.h src/f4logic.c headers/errExit.h)
