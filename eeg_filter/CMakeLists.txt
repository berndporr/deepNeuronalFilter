cmake_minimum_required(VERSION 3.1.0)
project(eeg_filter)
set(CMAKE_CXX_STANDARD 14)
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED)
include_directories(eeg_filter PRIVATE .. ../cvui/)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

add_executable(eeg_filter eeg_filter.cpp dynamicPlots.cpp dynamicPlots.h parameters.h ../cvui/cvui.h)
target_link_libraries(eeg_filter dnf iir fir ${OpenCV_LIBS})

