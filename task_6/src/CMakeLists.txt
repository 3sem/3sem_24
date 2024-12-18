project(processmon)

add_compile_definitions(-DBIN_PATH=\"${CMAKE_CURRENT_BINARY_DIR}\")

file(GLOB SRCS      ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB HEADERS   ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_executable(processmon ${SRCS} ${HEADERS})

target_include_directories(
    processmon
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)