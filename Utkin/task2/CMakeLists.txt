cmake_minimum_required(VERSION 3.10)

project(DupPipe)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

set(SOURCES
    src/main.c
    src/pipe.c
)

add_executable(dup_pipe ${SOURCES})

target_include_directories(dup_pipe PRIVATE ${PROJECT_INCLUDE_DIR})

