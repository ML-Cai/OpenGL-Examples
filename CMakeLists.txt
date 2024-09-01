cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(OpenGL-Examples)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ------------------------------------------------------------------------------
# Add glfw as a subdirectory
add_subdirectory(3rdparty/glfw)

# Add glad
set(GLAD_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/3rdparty/glad/include)
include_directories(${GLAD_INCLUDE_DIR})

# ------------------------------------------------------------------------------
# Add examples
add_subdirectory(examples)
