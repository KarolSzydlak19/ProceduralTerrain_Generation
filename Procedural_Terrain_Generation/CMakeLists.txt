cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set paths for external libraries
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/External/libraries)
set(GLFW_LIB_DIR ${EXTERNAL_DIR}/lib)
set(INCLUDE_DIR ${EXTERNAL_DIR}/include)

# Include directories
include_directories(${INCLUDE_DIR})
include_directories(${INCLUDE_DIR}/glad)
include_directories(${INCLUDE_DIR}/glfw)
include_directories(${INCLUDE_DIR}/glm)
include_directories(${INCLUDE_DIR}/KHR)
include_directories(${INCLUDE_DIR}/stb)
include_directories(${CMAKE_SOURCE_DIR}/src/headers)

# Add GLAD source file
add_library(glad ${INCLUDE_DIR}/glad/glad.c)

set(SOURCES
    src/Procedural_Terrain_Generation.cpp
    src/Map.cpp
)

# Add the executable
add_executable(MyProject ${SOURCES})
# Add the executable
#add_executable(MyProject src/Procedural_Terrain_Generation.cpp)

# Link libraries
target_link_libraries(MyProject glad ${GLFW_LIB_DIR}/glfw3.lib opengl32.lib)
