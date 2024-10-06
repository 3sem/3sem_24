cmake_minimum_required(VERSION 3.15)

file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB HDRS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_executable(channel ${SRC} ${HDRS})

target_include_directories(
    channel
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)