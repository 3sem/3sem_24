cmake_minimum_required(VERSION 3.20)

project(CommandPipelineProject)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(command_pipeline
    src/main.c
    src/cmd_runner.c
    src/parser.c
    src/input.c
)

target_include_directories(command_pipeline PRIVATE include/)
