project(processmon)

file(GLOB SRCS      ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB HEADERS   ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_executable(processmon ${SRCS} ${HEADERS})

target_include_directories(
    processmon
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)