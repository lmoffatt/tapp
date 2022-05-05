cmake_minimum_required(VERSION 3.5)

project(tapp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(tapp main.cpp tapp_base.h tapp_basics.h tapp_io.h tapp_run.h tapp_utility.h tapp_type_reduction.h tapp_lisp.h tapp_lisp_base.h tapp_lisp_type_reduction.h tapp_lisp_global_env.h tapp_lisp_base_serial.h tapp_lisp_run.h tapp_list_map.h
)
