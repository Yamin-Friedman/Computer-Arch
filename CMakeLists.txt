cmake_minimum_required(VERSION 3.9)
project(CompArch)

SET(CMAKE_C_STANDARD 99)


add_executable(wet4 hw4/core_api.c hw4/core_api.h hw4/sim_api.h hw4/sim_mem.c hw4/main.c)