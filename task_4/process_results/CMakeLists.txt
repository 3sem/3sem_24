project(res_process)

file(GLOB SRCS      ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB HEADERS   ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_executable(res_process ${SRCS} ${HEADERS})

target_include_directories(
    res_process
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/include
)