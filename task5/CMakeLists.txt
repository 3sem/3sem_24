cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(signal_com)

add_executable(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(${PROJECT_NAME} PRIVATE --std=c17)

add_subdirectory(src)