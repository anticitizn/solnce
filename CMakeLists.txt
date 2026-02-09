cmake_minimum_required(VERSION 3.16)
include(FetchContent)

project(solnce)

file(GLOB_RECURSE SOURCES src/*.cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")
set(EXT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/external/")
add_subdirectory(external/sdl EXCLUDE_FROM_ALL)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_library(imgui
    # Main Imgui files
    external/imgui/imgui.cpp external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp external/imgui/imgui_widgets.cpp
    # SDL2+OpenGL-specific files
    external/imgui/imgui_impl_sdl3.cpp
    external/imgui/imgui_impl_opengl3.cpp)

target_link_libraries(imgui PUBLIC SDL3::SDL3)
target_include_directories(imgui PUBLIC external/imgui/)
# Configure Imgui to use OpengGL through SDL3
target_compile_definitions(imgui PUBLIC GL_GLEXT_PROTOTYPES=1)

add_executable(${PROJECT_NAME} ${SOURCES} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" external)
target_link_libraries(${PROJECT_NAME} "${EXT_SOURCE_DIR}/glad/glad.cpp" "${EXT_SOURCE_DIR}/pugixml/pugixml.cpp" SDL3::SDL3 ${CMAKE_DL_LIBS} imgui)