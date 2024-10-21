include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)

add_definitions(-DTIMER)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/common             ${CMAKE_CURRENT_BINARY_DIR}/common)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/shared_memory_src  ${CMAKE_CURRENT_BINARY_DIR}/shared_mem)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/msg_queues_src     ${CMAKE_CURRENT_BINARY_DIR}/message_queues)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/named_pipe_src     ${CMAKE_CURRENT_BINARY_DIR}/fifo_ipc)
