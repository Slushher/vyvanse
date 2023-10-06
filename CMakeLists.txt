cmake_minimum_required(VERSION 3.23.2)

project(VYVANSE VERSION 1.0.0)

add_compile_options(-g)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin/")
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp" "lib/glad/glad.*")
add_executable(vyvanse ${SOURCES})

target_include_directories(vyvanse PUBLIC headers)
include_directories("lib/glad/include")
target_link_libraries(vyvanse -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl)