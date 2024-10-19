add_subdirectory(src)

target_include_directories(${FIFO_COM} PRIVATE include)
target_include_directories(${MSG_COM} PRIVATE include)
target_include_directories(${SHMEM_COM} PRIVATE include)