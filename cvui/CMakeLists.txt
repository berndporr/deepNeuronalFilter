cmake_minimum_required(VERSION 3.9.0)

set(CMAKE_CXX_STANDARD 17)

project(cvui)

add_library(cvui INTERFACE)

target_include_directories(cvui INTERFACE .)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()
