cmake_minimum_required(VERSION 3.15)

add_definitions(-DDEBUG)

project(cmd_emulator)

add_subdirectory(src ${CMAKE_CURRENT_BINARY_DIR}/cmd_emulator)