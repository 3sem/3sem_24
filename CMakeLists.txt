cmake_minimum_required(VERSION 3.15)

project(3_sem)

add_definitions(
-D _DEBUG -ggdb3 -O0 -Wall -Wextra -Waggressive-loop-optimizations
-Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion
-Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op
-Wopenmp-simd -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow
-Wsign-conversion -Wstrict-overflow=2 -Wsuggest-attribute=noreturn
-Wsuggest-final-methods -Wsuggest-final-types -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef
-Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers
-Wno-narrowing -Wno-varargs -Wstack-protector -fcheck-new -fstack-protector
-fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -Werror=vla
)

include_directories(
    GLOBAL
    ${CMAKE_CURRENT_SOURCE_DIR}/logging/src/
)

add_subdirectory(task_1 ${CMAKE_CURRENT_BINARY_DIR}/task_1)
add_subdirectory(task_2 ${CMAKE_CURRENT_BINARY_DIR}/task_2)
add_subdirectory(logging ${CMAKE_CURRENT_BINARY_DIR}/logging)
