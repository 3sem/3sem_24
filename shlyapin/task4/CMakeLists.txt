cmake_minimum_required(VERSION 3.15)

project(ComputeIntegral VERSION 1.0.0)

#--------------------------------------------------------------------
#                  Общие настройки проекта
#--------------------------------------------------------------------
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

set(EXECUTABLE_NAME compute_integral)


#--------------------------------------------------------------------
#             Поиск и подключение стандартной библиотеки math
#--------------------------------------------------------------------
find_library(MATH_LIB m) # Либо используем стандартный флаг -lm
if(NOT MATH_LIB)
    message(FATAL_ERROR "Math library not found!")
endif()


#--------------------------------------------------------------------
#                   Определение цели - ${EXECUTABLE_NAME}
#--------------------------------------------------------------------
add_executable(${EXECUTABLE_NAME} ${SOURCES})

target_include_directories(${EXECUTABLE_NAME} PUBLIC ${PROJECT_INCLUDE_DIRS})

target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${MATH_LIB})


#--------------------------------------------------------------------
#         Опция выбора санитайзера (asan, tsan, msan, none)
#--------------------------------------------------------------------
set(SANITIZER_TYPE "none")
# set(SANITIZER_TYPE "asan")
# set(SANITIZER_TYPE "tsan")
# set(SANITIZER_TYPE "msan")


#--------------------------------------------------------------------
#               Настройка Debug сборки
#--------------------------------------------------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(${CMAKE_CURRENT_SOURCE_DIR}/debug_config.cmake)

    target_compile_options(${EXECUTABLE_NAME} PRIVATE ${MY_DEBUG_FLAGS})

    if(SANITIZER_TYPE STREQUAL "asan")
        target_compile_options(${EXECUTABLE_NAME} PRIVATE ${ASAN_FLAGS})
        target_link_options(${EXECUTABLE_NAME} PRIVATE ${ASAN_FLAGS})
    elseif(SANITIZER_TYPE STREQUAL "tsan")
        target_compile_options(${EXECUTABLE_NAME} PRIVATE ${TSAN_FLAGS})
        target_link_options(${EXECUTABLE_NAME} PRIVATE ${TSAN_FLAGS})
    elseif(SANITIZER_TYPE STREQUAL "msan")
        target_compile_options(${EXECUTABLE_NAME} PRIVATE ${MSAN_FLAGS})
        target_link_options(${EXECUTABLE_NAME} PRIVATE ${MSAN_FLAGS})
    endif()
endif()


#--------------------------------------------------------------------
#               Общие настройки для всех режимов сборки
#--------------------------------------------------------------------
set_target_properties(${EXECUTABLE_NAME} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    OUTPUT_NAME ${EXECUTABLE_NAME}
    DEBUG_POSTFIX _debug
)


#--------------------------------------------------------------------
#                           Тестирование
#--------------------------------------------------------------------
find_library(CRITERION_LIB criterion)
if(CRITERION_LIB)
    add_subdirectory(test)
endif()
