cmake_minimum_required(VERSION 3.10)
project(OpenGLPhysicsEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

find_package(glfw CONFIG QUIET)
if(glfw_FOUND)
    set(GLFW_TARGET glfw)
else()
    find_package(glfw3 CONFIG REQUIRED)
    set(GLFW_TARGET glfw3)
endif()

find_package(glm CONFIG QUIET)
if(NOT glm_FOUND)
    find_package(glm REQUIRED)
endif()

add_library(glad STATIC glad/glad.c)
target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/glad
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/KHR
)

add_library(imgui STATIC
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
)
target_link_libraries(imgui PRIVATE ${GLFW_TARGET} OpenGL::GL)

file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)
file(GLOB_RECURSE LIB_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.h
)

add_executable(main ${SRC_FILES} ${LIB_FILES})

target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/glad
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/KHR
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(main PRIVATE
    glad
    imgui
    ${GLFW_TARGET}
    OpenGL::GL
)
