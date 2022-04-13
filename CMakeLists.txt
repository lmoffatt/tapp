cmake_minimum_required(VERSION 3.5)

project(tapp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(tapp main.cpp tapp_basics.h tapp_io.h tapp_run.h
)
