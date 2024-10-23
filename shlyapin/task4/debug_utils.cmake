#--------------------------------------------------------------------
#       Функция для добавления санитайзеров в цель сборки
#--------------------------------------------------------------------
function(add_debug_executable_with_sanitizers target sanitizer_name san_flags)
    add_executable(${target}_${sanitizer_name} ${SOURCES})

    target_compile_options(${target}_${sanitizer_name} PRIVATE ${san_flags} ${MY_DEBUG_FLAGS})
    target_link_options(${target}_${sanitizer_name} PRIVATE ${san_flags} ${MY_DEBUG_FLAGS})

    target_link_libraries(${target}_${sanitizer_name} ${PROJECT_LIBRARIES})

    target_include_directories(${target}_${sanitizer_name} PRIVATE ${PROJECT_INCLUDE_DIRS})

    set_target_properties(${target}_${sanitizer_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
        OUTPUT_NAME ${target}_${sanitizer_name}_debug
    )
endfunction()
#--------------------------------------------------------------------


#--------------------------------------------------------------------
#           Функция для создания тестов с санитайзерами
#--------------------------------------------------------------------
function(add_sanitizer_test target sanitizer_name san_flags)
    add_executable(${target}_${sanitizer_name} ${TEST_SOURCES} ${PROJECT_SOURCES})

    target_compile_options(${target}_${sanitizer_name} PRIVATE ${MY_DEBUG_FLAGS} ${san_flags})
    target_link_options(${target}_${sanitizer_name} PRIVATE ${MY_DEBUG_FLAGS} ${san_flags})

    target_link_libraries(${target}_${sanitizer_name} ${PROJECT_LIBRARIES} ${TEST_LIBRARIES})

    target_include_directories(${target}_${sanitizer_name} PRIVATE ${PROJECT_INCLUDE_DIRS} ${TEST_INCLUDE_DIRS})

    set_target_properties(${target}_${sanitizer_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
        OUTPUT_NAME ${target}_${sanitizer_name}
        DEBUG_POSTFIX _debug
    )

    add_test(NAME ${target}_${sanitizer_name} COMMAND ${target}_${sanitizer_name})
endfunction()
#--------------------------------------------------------------------
