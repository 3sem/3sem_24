cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(monte_carlo)

add_executable(${PROJECT_NAME})

# target_compile_options(${PROJECT_NAME} PRIVATE -std=c17 -O3)

add_subdirectory(src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)