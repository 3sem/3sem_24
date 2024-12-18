project(integral)

file(GLOB SRCS      ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB HEADERS   ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_executable(integral ${SRCS} ${HEADERS})

target_include_directories(
    integral
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(integral m)