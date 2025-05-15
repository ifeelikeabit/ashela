cmake_minimum_required(VERSION 4.0)
project(ashela)

set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Wextra)

include_directories(lib)
file(GLOB SOURCES "src/*.c")

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
add_executable(ashela ${SOURCES})


