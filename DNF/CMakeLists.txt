cmake_minimum_required(VERSION 3.1.0)

set(CMAKE_CXX_STANDARD 11)

project (CLDL)

add_library(CLDL
        lib/Layer.cpp
        lib/Net.cpp
        lib/Neuron.cpp)

target_include_directories(CLDL PUBLIC include)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()
