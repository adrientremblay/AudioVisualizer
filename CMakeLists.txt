cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# Set project name and version
project(AudioVisualizer VERSION 0.1)

# Set build type
set(CMAKE_BUILD_TYPE Release)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Enable debug symbols by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug
            CACHE STRING
            "Choose the type of build (Debug or Release)"
            FORCE
            )
endif()

# Find SFML shared libraries
find_package(SFML 2.5
        COMPONENTS
        system window graphics network audio REQUIRED
        )

find_package(PkgConfig REQUIRED)
pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)
include_directories(PkgConfig::FFTW)
link_libraries     (PkgConfig::FFTW)

find_package(OpenGL REQUIRED)

# Compile executable
add_executable(${PROJECT_NAME} lib/glad/glad.c main.cpp src/FFTStream.cpp src/Shader.cpp lib/stb_image/stb_image.cpp)

# Set include directory search paths
target_include_directories(${PROJECT_NAME}
        PRIVATE
        "${PROJECT_BINARY_DIR}"
        )

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES})

find_package(assimp REQUIRED)
include_directories(${ASSIMP_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} assimp)

# Link executable to required SFML libraries
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio GL dl)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_TTF_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_TTF_LIBRARIES})

find_package(TGUI REQUIRED)
target_link_libraries(${PROJECT_NAME} tgui)

# Install target
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

