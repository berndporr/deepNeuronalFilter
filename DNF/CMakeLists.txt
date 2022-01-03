cmake_minimum_required(VERSION 3.1.0)

set(CMAKE_CXX_STANDARD 11)

project (DNF)

add_library(DNF	
        dnf/Layer.cpp
        dnf/Net.cpp
        dnf/Neuron.cpp)

target_include_directories(DNF PUBLIC . dnf)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()
