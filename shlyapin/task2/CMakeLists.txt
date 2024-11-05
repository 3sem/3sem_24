# Указываем минимальную версию CMake
cmake_minimum_required(VERSION 3.10)

# Устанавливаем имя проекта
project(FullDuplexPipe VERSION 1.0)

# Включаем экспорт команд компиляции
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Указываем стандарт C++
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# --- Настройки для основной программы ---

# Переменная с каталогом заголовков для основной программы
set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# Добавляем директории заголовочных файлов для основной программы
include_directories(${PROJECT_INCLUDE_DIR})

# Указываем исходные файлы для основной программы вручную
set(SOURCES
    src/main.c
    src/duplex_pipe.c
    src/data_exchange.c
)

# Создаем цель для основной программы
add_executable(dup_pipe ${SOURCES})

# Устанавливаем свойства для основной программы
set_target_properties(dup_pipe PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    OUTPUT_NAME dup_pipe
    DEBUG_POSTFIX _debug
)

# Добавляем опции компиляции только для режима Debug
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(dup_pipe PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Wstrict-aliasing -Wconversion -Wthread-safety -Wdouble-promotion -fsanitize=thread,undefined -Watomic-alignment -Wunreachable-code -Wmissing-noreturn -Wimplicit-fallthrough)
    target_link_options(dup_pipe PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Wstrict-aliasing -Wconversion -Wthread-safety -Wdouble-promotion -fsanitize=thread,undefined -Watomic-alignment -Wunreachable-code -Wmissing-noreturn -Wimplicit-fallthrough)
endif()


# --- Настройки для тестов ---

# Переменная с каталогом заголовков для тестов
# set(TEST_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/test/include)

# Добавляем директории заголовочных файлов для тестов
# include_directories(${TEST_INCLUDE_DIR})

# Указываем исходные файлы для тестов вручную
# set(TEST_SOURCES
#     test/test_main.c
# )

# Создаем отдельную цель для тестов
# add_executable(MyTests ${TEST_SOURCES})

# Устанавливаем свойства для тестов
# set_target_properties(MyTests PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
#     OUTPUT_NAME MyTests
#     DEBUG_POSTFIX _debug
# )

# Добавляем опции компиляции только для режима Debug
# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#     target_compile_options(MyTests PRIVATE -Wall -Wextra -Wpedantic)
# endif()# Включаем экспорт команд компиляции
