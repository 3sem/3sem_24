cmake_minimum_required(VERSION 3.10)

# Set the project name
project(DaemonVasya)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Specify the C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add the executable
add_executable(daemon 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daemon.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/monitoring.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.c
)

# Include directories
target_include_directories(daemon PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
