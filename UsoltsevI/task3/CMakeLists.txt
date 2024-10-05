#---------------------------------------------------------------------
#
# Cmake build system for file sharing speed comparison
#
#---------------------------------------------------------------------

# cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
# VERBOSE=1 cmake --build ./build

cmake_minimum_required(VERSION 3.11)
project("filesharing")

SET(SOURCES
    source/fifo.c
    source/message-queues.c
    source/shared-memory.c
    main.c
)

add_executable(filesharing ${SOURCES})
