cmake_minimum_required(VERSION 3.15)

project(IntegralTests)


#--------------------------------------------------------------------
#                  Общие настройки проекта
#--------------------------------------------------------------------
enable_testing()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

set(TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_integral.c
)

set(PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/integral.c
)

set(PROJECT_INCLUDE_DIRS 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

set(EXECUTABLE_NAME test_compute)


#--------------------------------------------------------------------
#             Поиск библиотеки Criterion
#--------------------------------------------------------------------
find_library(CRITERION_LIB criterion)
if(NOT CRITERION_LIB)
    message(FATAL_ERROR "Не найдена необходимая библиотека Criterion. Завершение сборки.")
endif()


#--------------------------------------------------------------------
#             Поиск математической библиотеки
#--------------------------------------------------------------------
find_library(MATH_LIB m)
if(NOT MATH_LIB)
    message(FATAL_ERROR "Не найдена математическая библиотека. Завершение сборки.")
endif()


#--------------------------------------------------------------------
#                     Определение цели - MyTests
#--------------------------------------------------------------------
add_executable(${EXECUTABLE_NAME} ${TEST_SOURCES} ${PROJECT_SOURCES})

target_include_directories(${EXECUTABLE_NAME} PRIVATE ${PROJECT_INCLUDE_DIRS})

target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${CRITERION_LIB} ${MATH_LIB})

add_test(NAME ${EXECUTABLE_NAME} COMMAND ${EXECUTABLE_NAME})


#--------------------------------------------------------------------
#               Настройки для Debug режима
#--------------------------------------------------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(${CMAKE_CURRENT_SOURCE_DIR}/../debug_config.cmake)

    target_compile_options(${EXECUTABLE_NAME} PRIVATE ${MY_DEBUG_FLAGS})
endif()


#--------------------------------------------------------------------
#               Общие настройки для всех режимов сборки
#--------------------------------------------------------------------
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    OUTPUT_NAME ${EXECUTABLE_NAME}
    DEBUG_POSTFIX _debug
)