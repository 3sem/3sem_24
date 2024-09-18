#---------------------------------------------------------------------
#
# Cmake build system for uso-terminal
#
#---------------------------------------------------------------------

# cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
# VERBOSE=1 cmake --build ./build

cmake_minimum_required(VERSION 3.11)
project("bigpipe")

SET(SRCS
    bigpipe.c
)

add_library(bpipe STATIC ${SRCS})

add_executable(bigpipe main.c)
target_link_libraries(bigpipe PUBLIC bpipe)

# add_executable(uso-terminal uso-terminal.c)