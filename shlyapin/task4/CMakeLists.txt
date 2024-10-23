CMAKE_MINIMUM_REQUIRED(VERSION 3.15)

project(ComputeIntegral VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/integral.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
)

set(PROJECT_INCLUDE_DIRS 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(PROJECT_LIBRARIES m)

#--------------------------------------------------------------------
#                   Определение Release цели - compute_integral 
#--------------------------------------------------------------------
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_executable(compute_integral ${SOURCES})

    target_include_directories(compute_integral PUBLIC ${PROJECT_INCLUDE_DIRS})

    target_link_libraries(compute_integral PUBLIC ${PROJECT_LIBRARIES})
    
    set_target_properties(compute_integral PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    )
endif()
#--------------------------------------------------------------------


#--------------------------------------------------------------------
#                   Настройка Debug режима сборки
#--------------------------------------------------------------------
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(${CMAKE_CURRENT_SOURCE_DIR}/debug_utils.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/debug_config.cmake)

    add_debug_executable_with_sanitizers(compute_integral asan ${ASAN_FLAGS})
    add_debug_executable_with_sanitizers(compute_integral tsan ${TSAN_FLAGS})
    add_debug_executable_with_sanitizers(compute_integral msan ${MSAN_FLAGS})
endif()
#--------------------------------------------------------------------


#--------------------------------------------------------------------
#                           Тестирование
#--------------------------------------------------------------------
find_library(CRITERION_LIB criterion)
if(CRITERION_LIB)
     add_subdirectory(test)
endif()
#--------------------------------------------------------------------