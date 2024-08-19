cmake_minimum_required(VERSION 3.16)
include(FetchContent)

project(solnce)

file(GLOB_RECURSE SOURCES src/*.cpp)

find_package(SDL2 REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")
set(EXT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/external/")

set(BOX2D_PATH "external/box2d/")
add_subdirectory(${BOX2D_PATH} box2d)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_library(imgui
    # Main Imgui files
    external/imgui/imgui.cpp external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp external/imgui/imgui_widgets.cpp
    # SDL2+OpenGL-specific files
    external/imgui/imgui_impl_sdl.cpp
    external/imgui/imgui_impl_opengl3.cpp)

# Make SDL2 available to Imgui
target_include_directories(imgui PUBLIC ${SDL2_INCLUDE_DIRS})
# imgui/backends/ contains the SDL implementation
target_include_directories(imgui PUBLIC external/imgui/)
# Configure Imgui to use OpengGL through SDL2
target_compile_definitions(imgui PUBLIC GL_GLEXT_PROTOTYPES=1)

add_executable(${PROJECT_NAME} ${SOURCES} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" ${SDL2_INCLUDE_DIRS} external)
target_link_libraries(${PROJECT_NAME} "${EXT_SOURCE_DIR}/glad/glad.cpp" "${EXT_SOURCE_DIR}/pugixml/pugixml.cpp" ${SDL2_LIBRARIES} ${CMAKE_DL_LIBS} imgui box2d)
