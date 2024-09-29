cmake_minimum_required(VERSION 3.20)
project(MyProject)

if(NOT MSVC)
    add_compile_options(-g)
endif()

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set paths for external libraries
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/External/libraries)
set(GLFW_LIB_DIR ${EXTERNAL_DIR}/lib)
set(INCLUDE_DIR ${EXTERNAL_DIR}/include)
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/External/imgui)

# Include directories
include_directories(${INCLUDE_DIR})
include_directories(${INCLUDE_DIR}/glad)
include_directories(${INCLUDE_DIR}/glfw)
include_directories(${INCLUDE_DIR}/glm)
include_directories(${INCLUDE_DIR}/KHR)
include_directories(${INCLUDE_DIR}/stb)
include_directories(${CMAKE_SOURCE_DIR}/src/headers)
include_directories(${IMGUI_DIR})

# Add GLAD source file
add_library(glad ${INCLUDE_DIR}/glad/glad.c "src/headers/PerlinNoiseGenerator.h" "src/PerlinNoiseGenerator.cpp" "src/headers/SimplexNoiseGenerator.h" "src/SimplexNoiseGenerator.cpp")

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
    src/MapBuilder.cpp
    src/ShadowMap.cpp   # Add this
    src/Skybox.cpp      # Add this
    src/Texture.cpp     # Add this
    src/Sphere.cpp      # Add this
    # ImGui source files
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/imgui_impl_opengl3.cpp
)

# Add the executable
add_executable(MyProject ${SOURCES} "src/stb.cpp" "src/headers/Texture.h" "src/Texture.cpp" "src/headers/Window.h" "src/headers/Sphere.h" "src/Sphere.cpp" "src/headers/ShadowMap.h" "src/ShadowMap.cpp" "src/Skybox.cpp" "src/headers/PerlinNoiseGenerator.h" "src/PerlinNoiseGenerator.cpp" "src/headers/SimplexNoiseGenerator.h" "src/SimplexNoiseGenerator.cpp")


# Link libraries
target_link_libraries(MyProject glad ${GLFW_LIB_DIR}/glfw3.lib opengl32)

# Define GLAD as the OpenGL loader for ImGui
target_compile_definitions(MyProject PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

# Set the working directory for the executable
set_target_properties(MyProject PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/src"
)

if(NOT MSVC)
    add_compile_options(-g)
endif()
