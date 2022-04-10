cmake_minimum_required(VERSION 3.16)
project(solnce)

file(GLOB SOURCES src/*.cpp)

find_package(SDL2 REQUIRED)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")
set(EXT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/external/")

add_executable(${PROJECT_NAME} ${SOURCES} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} "${EXT_SOURCE_DIR}/glad/glad.cpp" ${SDL2_LIBRARIES} ${CMAKE_DL_LIBS})
