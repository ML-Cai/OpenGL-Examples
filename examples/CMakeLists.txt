cmake_minimum_required(VERSION 3.10)


# Find all .cpp files in the example/ directory
file(GLOB EXAMPLE_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

# Loop through each .cpp file and create an executable
foreach(SOURCE_FILE ${EXAMPLE_SOURCES})
    # Extract the file name without the directory and extension
    get_filename_component(EXE_NAME ${SOURCE_FILE} NAME_WE)

    # Create an executable for each .cpp file
    add_executable(${EXE_NAME} ${SOURCE_FILE})

    # Link against the GLFW library
    target_link_libraries(${EXE_NAME} PRIVATE glfw ${GLFW_LIBRARIES})

    # Link against the appropriate OpenGL library for your platform
    if (WIN32)
        target_link_libraries(${EXE_NAME} PRIVATE opengl32)
    elseif(UNIX)
        find_package(OpenGL REQUIRED)
        target_link_libraries(${EXE_NAME} PRIVATE OpenGL::GL)
    endif()

    # Include the GLFW headers
    target_include_directories(${EXE_NAME} PRIVATE external/glfw/include)
endforeach()
