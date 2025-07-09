cmake_minimum_required(VERSION 3.10)

project(OpenGLPhysicsEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenGL, GLFW, and GLM
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Add GLAD
add_library(glad STATIC glad/glad.c)
target_include_directories(glad PUBLIC glad)

# Add ImGui (GLFW + OpenGL backend)
add_library(imgui STATIC
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC imgui imgui/backends)
target_link_libraries(imgui PRIVATE glfw OpenGL::GL)

# Collect all source and header files
file(GLOB_RECURSE SRC_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)
file(GLOB_RECURSE LIB_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.h
)

# Add the main executable
add_executable(main ${SRC_FILES} ${LIB_FILES})

# Include dirs: GLAD, GLM, and src (for headers like Application/renderer.h)
target_include_directories(main 
    PRIVATE 
        glad 
        ${GLM_INCLUDE_DIRS} 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(main PRIVATE glad imgui glfw OpenGL::GL)
