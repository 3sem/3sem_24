cmake_minimum_required(VERSION 3.15)

project(3_sem)

include_directories(
    GLOBAL
    ${CMAKE_CURRENT_SOURCE_DIR}/logging/src/
)

add_subdirectory(task_1 ${CMAKE_CURRENT_BINARY_DIR}/task_1)
add_subdirectory(task_2 ${CMAKE_CURRENT_BINARY_DIR}/task_2)
add_subdirectory(logging ${CMAKE_CURRENT_BINARY_DIR}/logging)
