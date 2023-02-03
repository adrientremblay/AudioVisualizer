cmake_minimum_required(VERSION 3.23)
project(AudioVisualizer)

set(CMAKE_CXX_STANDARD 14)

add_executable(AudioVisualizer
        main.cpp)

find_package(ALSA REQUIRED)
if (ALSA_FOUND)
    include_directories(${ALSA_INCLUDE_DIRS})
    target_link_libraries (AudioVisualizer ${ALSA_LIBRARIES})
endif(ALSA_FOUND)

find_package(PkgConfig REQUIRED)
pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)
include_directories(PkgConfig::FFTW)
target_link_libraries     (AudioVisualizer PkgConfig::FFTW)
