CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(ComputeIntegral VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(PROJECT_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include)

set(SOURCES
    src/main.c
    src/integral.c
)
#--------------------------------------------------------------------
#                   Определение цели definite_integral
#--------------------------------------------------------------------
add_executable(definite_integral ${SOURCES})

target_include_directories(definite_integral PUBLIC ${PROJECT_INCLUDE_DIRS})

target_link_libraries(definite_integral PUBLIC m)

set_target_properties(definite_integral PROPERTIES
RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
OUTPUT_NAME integral
DEBUG_POSTFIX _debug
)
#--------------------------------------------------------------------

#--------------------------------------------------------------------
#                   Настройка Debug режима сборки
#--------------------------------------------------------------------
include(debug_config.cmake)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(definite_integral PRIVATE ${MY_DEBUG_FLAGS})
    target_link_options   (definite_integral PRIVATE ${MY_DEBUG_FLAGS})
endif()
#--------------------------------------------------------------------
