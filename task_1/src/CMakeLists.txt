cmake_minimum_required(VERSION 3.15)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/headers/*.h)

add_executable(cmd_emulator ${SOURCES} ${HEADERS})

target_include_directories(
    cmd_emulator
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(cmd_emulator logging)