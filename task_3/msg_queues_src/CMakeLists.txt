project(message_queues)

file(GLOB SRCS      ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB HEADERS   ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_executable(message_queues ${SRCS} ${HEADERS})

target_include_directories(
    message_queues
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(message_queues common)