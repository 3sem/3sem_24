cmake_minimum_required(VERSION 3.15)

project(IntegralTests)

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

find_library(CRITERION_LIB criterion)
if(CRITERION_LIB)
    set(TEST_LIBRARIES m criterion)
else()
    message(FATAL_ERROR "Не найдена необходимая библиотека Criterion. Завершение сборки.")
endif()


#--------------------------------------------------------------------
#                            MyTests
#--------------------------------------------------------------------
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_executable(MyTests ${TEST_SOURCES} ${PROJECT_SOURCES})

    target_include_directories(MyTests PRIVATE ${PROJECT_INCLUDE_DIRS})

    target_link_libraries(MyTests PUBLIC ${TEST_LIBRARIES})

    set_target_properties(MyTests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
        OUTPUT_NAME test
    )

    add_test(NAME MyTests COMMAND MyTests)
endif()
#--------------------------------------------------------------------


#--------------------------------------------------------------------
#             Указываем настройки компиляции для Debug
#--------------------------------------------------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(${CMAKE_CURRENT_SOURCE_DIR}/../debug_config.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../debug_utils.cmake)

    add_sanitizer_test(MyTests asan ${ASAN_FLAGS})
    add_sanitizer_test(MyTests tsan ${TSAN_FLAGS})
    add_sanitizer_test(MyTests msan ${MSAN_FLAGS})
endif()
#--------------------------------------------------------------------
