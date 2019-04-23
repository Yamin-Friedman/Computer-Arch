cmake_minimum_required(VERSION 3.9)
project(CompArch)

SET(CMAKE_C_STANDARD 99)


add_executable(wet2 ./hw2/cacheSim.cpp hw2/Cache.h hw2/Cache.cpp hw2/CacheLine.cpp hw2/CacheLine.h)