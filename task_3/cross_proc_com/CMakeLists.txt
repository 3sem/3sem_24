cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(IPC)

set(FIFO_COM FIFO_com)
set(MSG_COM msg_com)
set(SHMEM_COM shmem_com)

add_executable(${FIFO_COM})
add_executable(${MSG_COM})
add_executable(${SHMEM_COM})

add_subdirectory(common)
add_subdirectory(FIFO_com)
add_subdirectory(msg_com)
add_subdirectory(shmem_com)

set_target_properties(${FIFO_COM} ${MSG_COM} ${SHMEM_COM} 
                        PROPERTIES 
                        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)