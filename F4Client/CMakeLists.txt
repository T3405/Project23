cmake_minimum_required(VERSION 3.24)
project(F4Client C)

set(CMAKE_C_STANDARD 11)

include_directories(../F4Server/inc/)

add_executable(F4Client src/main.c)
