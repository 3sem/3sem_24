cmake_minimum_required(VERSION 3.15)

project(fifo_ipc)

file(GLOB SRCS      ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB HEADERS   ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(fifo_ipc ${SRCS} ${HEADERS})

target_include_directories(
    fifo_ipc
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(fifo_ipc common)