cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(full_duplex_pipe)

add_executable(${PROJECT_NAME})

target_compile_options(${PROJECT_NAME} PRIVATE -std=c17 -O3)

# target_link_options(${PROJECT_NAME} PRIVATE -lasan -lubsan)

add_subdirectory(src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)