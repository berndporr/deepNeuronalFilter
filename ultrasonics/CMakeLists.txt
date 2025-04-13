cmake_minimum_required(VERSION 3.7.0)

project(ultrasounddnf VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()


set(CMAKE_CXX_STANDARD 17)

add_executable(dofilter
  dofilter.cpp
)

target_link_libraries(dofilter matio)
