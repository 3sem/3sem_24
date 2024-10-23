set(MY_DEBUG_FLAGS
    -glldb
    -O0
    -Wall
    -Wextra
    -Wpedantic
    -Wcast-align
    -Wchar-subscripts
    -Wconversion
    -Wempty-body
    -Wfloat-equal
    -Wpointer-arith
    -Wshadow
    -Wsign-conversion
    -Wswitch-default
    -Wswitch-enum
    -Wundef
    -Wunreachable-code
    -Wunused
    -Wvariadic-macros
    -Wlarger-than=8192
)

set(ASAN_FLAGS -fsanitize=address)
set(TSAN_FLAGS -fsanitize=thread)
set(MSAN_FLAGS -fsanitize=memory)
