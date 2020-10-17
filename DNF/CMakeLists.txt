cmake_minimum_required(VERSION 3.17.2)

set(CMAKE_CXX_STANDARD 11)

project (CLDL)

add_library(CLDL
        lib/Layer.cpp
        lib/Net.cpp
        lib/Neuron.cpp)

target_include_directories(CLDL PUBLIC include)
