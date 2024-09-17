cmake_minimum_required(VERSION 3.10)

project(CommandPipelineProject)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(command_pipeline
    src/main.c
    src/separator.c
    src/processor.c
)

target_include_directories(command_pipeline PRIVATE include/)
