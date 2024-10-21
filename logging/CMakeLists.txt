cmake_minimum_required(VERSION 3.5)

file(GLOB SRCS src/*)

add_library(logging ${SRCS})

include_directories(
    logging
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

