cmake_minimum_required(VERSION 3.12)
project(wet2)

set(CMAKE_CXX_STANDARD 14)

add_executable(wet2 cacheSim.cpp L1.h L1.cpp CacheLine.h CacheLine.cpp L2.h L2.cpp VictimCache.cpp VictimCache.h)