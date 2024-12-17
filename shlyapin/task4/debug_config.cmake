set(MY_DEBUG_FLAGS
    -glldb
    -O0
    -pedantic-errors

    -Wall
    -Wextra
    -Wpedantic
    
    -Wcast-align
    -Wchar-subscripts
    -Wconversion
    -Wempty-body
    -Wfloat-equal
    -Wshadow
    -Wsign-conversion
    -Wswitch-default
    -Wswitch-enum
    -Wundef
    -Wunreachable-code
    -Wunused
    -Wlarger-than=8192
)

set(ASAN_FLAGS -fsanitize=address,leak,undefined)
set(TSAN_FLAGS -fsanitize=thread,undefined)
set(MSAN_FLAGS -fsanitize=memory,undefined)
