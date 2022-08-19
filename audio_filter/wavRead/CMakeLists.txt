cmake_minimum_required(VERSION 3.1.0)

set(CMAKE_CXX_STANDARD 14)

project(WAV_Read)

add_library(wavread INTERFACE)

target_include_directories(wavread INTERFACE .)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

add_executable(demo demo.cpp)
