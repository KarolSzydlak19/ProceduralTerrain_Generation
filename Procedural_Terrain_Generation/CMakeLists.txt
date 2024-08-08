cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set paths for external libraries
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/External/libraries)
set(GLFW_LIB_DIR ${EXTERNAL_DIR}/lib)
set(INCLUDE_DIR ${EXTERNAL_DIR}/include)

# Debug configuration
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -Wall -g")
set(CMAKE_BUILD_TYPE Debug)

# Include directories
include_directories(${INCLUDE_DIR})
include_directories(${INCLUDE_DIR}/glad)
include_directories(${INCLUDE_DIR}/glfw)
include_directories(${INCLUDE_DIR}/glm)
include_directories(${INCLUDE_DIR}/KHR)
include_directories(${INCLUDE_DIR}/stb)
include_directories(${CMAKE_SOURCE_DIR}/src/headers)

# Add GLAD source file
add_library(glad ${INCLUDE_DIR}/glad/glad.c "src/stb.cpp" "src/headers/Texture.h" "src/Texture.cpp")

# Source files
set(SOURCES
    src/EBO.cpp
    src/VAO.cpp
    src/VBO.cpp
    src/Shader.cpp
    src/Camera.cpp
    src/Map.cpp
    src/Procedural_Terrain_Generation.cpp
    src/Window.cpp
)

# Add the executable
add_executable(MyProject ${SOURCES} "src/stb.cpp" "src/headers/Texture.h" "src/Texture.cpp")

# Link libraries
target_link_libraries(MyProject glad ${GLFW_LIB_DIR}/glfw3.lib opengl32)

# Set the working directory for the executable
set_target_properties(MyProject PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src"
)
