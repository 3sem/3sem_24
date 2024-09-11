#---------------------------------------------------------------------
#
# Cmake build system for cmd-runner-lib
#
#---------------------------------------------------------------------

SET(SRCS
    source/cmd-runner.c 
    source/cmd-parser.c
)

add_library(cmd-runner STATIC ${SRCS})

target_include_directories(cmd-runner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
