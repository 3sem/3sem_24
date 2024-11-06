#---------------------------------------------------------------------
#
# Cmake build system for Monte Carlo program.
#
#---------------------------------------------------------------------

# cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
# VERBOSE=1 cmake --build ./build

cmake_minimum_required(VERSION 3.11)
project("montecarlo")

SET(SOURCES
    source/functions.c
    source/monte-carlo.c
    main.c
)

add_executable(montecarlo ${SOURCES})
target_link_libraries(montecarlo m)
