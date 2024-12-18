cmake_minimum_required(VERSION 3.15)

project(shared_mem)

file(GLOB SRCS      ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB HEADERS   ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(shared_mem ${SRCS} ${HEADERS})

target_include_directories(
    shared_mem
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(shared_mem common)