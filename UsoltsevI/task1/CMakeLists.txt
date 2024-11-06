#---------------------------------------------------------------------
#
# Cmake build system for uso-terminal
#
#---------------------------------------------------------------------

# cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
# VERBOSE=1 cmake --build ./build

cmake_minimum_required(VERSION 3.11)
project("uso-terminal")

add_subdirectory(cmd-runner-lib)

add_executable(uso-terminal uso-terminal.c)
target_link_libraries(uso-terminal PUBLIC cmd-runner)
