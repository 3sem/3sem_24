cmake_minimum_required(VERSION 3.10)

project(MethodsInteractionsProcesses)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
include_directories(${PROJECT_INCLUDE_DIR})

set(SOURCES
    src/main.c
    src/integral.c
)

add_executable(integral ${SOURCES})