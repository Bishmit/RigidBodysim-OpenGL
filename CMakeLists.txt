cmake_minimum_required(VERSION 3.10)
project(RigidBodySimu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------- OpenGL ----------------
find_package(OpenGL REQUIRED)

# ---------------- GLFW ----------------
find_package(glfw3 CONFIG QUIET)
if(glfw3_FOUND)
    set(GLFW_TARGET glfw)
else()
    find_package(PkgConfig)
    if(PkgConfig_FOUND)
        pkg_check_modules(GLFW REQUIRED glfw3)
        if(GLFW_FOUND)
            add_library(glfw INTERFACE)
            target_include_directories(glfw INTERFACE ${GLFW_INCLUDE_DIRS})
            target_link_libraries(glfw INTERFACE ${GLFW_LIBRARIES})
            set(GLFW_TARGET glfw)
        endif()
    endif()
endif()

if(NOT GLFW_TARGET)
    message(FATAL_ERROR "GLFW not found. Please install GLFW3.")
endif()

# ---------------- GLM (optional, if you're using it) ----------------
find_package(glm CONFIG QUIET)

# ---------------- GLAD ----------------
add_library(glad STATIC glad/glad.c)
target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/glad
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/KHR
)

# ---------------- ImGui ----------------
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

target_link_libraries(imgui PRIVATE
    ${GLFW_TARGET}
    OpenGL::GL
)

# ---------------- Sources ----------------
file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

file(GLOB_RECURSE LIB_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/*.h
)

# ---------------- Executable ----------------
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

# ---------------- Platform-specific linking ----------------
if(APPLE)
    # macOS frameworks
    target_link_libraries(main PRIVATE
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
elseif(UNIX AND NOT APPLE)
    # Linux
    find_package(Threads REQUIRED)
    target_link_libraries(main PRIVATE
        Threads::Threads
        ${CMAKE_DL_LIBS}
    )
    # X11 libraries (required by GLFW on Linux)
    find_package(X11)
    if(X11_FOUND)
        target_link_libraries(main PRIVATE ${X11_LIBRARIES})
    endif()
elseif(WIN32)
    # Windows - GLFW handles most dependencies automatically
    # Just ensure we're linking against the right subsystem
    if(MSVC)
        set_target_properties(main PROPERTIES
            WIN32_EXECUTABLE FALSE  # Set to TRUE if you want a GUI app without console
        )
    endif()
endif()

# ---------------- Compiler warnings ----------------
if(MSVC)
    target_compile_options(main PRIVATE /W4)
else()
    target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic)
endif()