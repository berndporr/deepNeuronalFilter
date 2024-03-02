cmake_minimum_required(VERSION 3.8.0)

project(dnf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

include(GNUInstallDirs)

add_subdirectory(eeg_filter)
add_subdirectory(eeg_sim)
add_subdirectory(ecg_filter)
add_subdirectory(audio_filter)
add_subdirectory(cvui)

enable_testing ()

if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wconversion -Wextra -pedantic)
endif()

set(LIBSRC
  dnf/Layer.cpp
  dnf/Net.cpp
  dnf/Neuron.cpp
  )

set(LIBINCLUDE
  dnf/Layer.h
  dnf/Net.h
  dnf/Neuron.h
  )

add_library(dnf STATIC ${LIBSRC})

set_target_properties(dnf PROPERTIES
  POSITION_INDEPENDENT_CODE TRUE
  PUBLIC_HEADER dnf.h
  PRIVATE_HEADER "${LIBINCLUDE}")

install(TARGETS dnf EXPORT dnf-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  PRIVATE_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/dnf)

install(EXPORT dnf-targets
  DESTINATION lib/cmake/dnf
  FILE dnf-config.cmake
)
