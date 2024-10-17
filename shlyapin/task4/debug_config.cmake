set(MY_DEBUG_FLAGS
    -g
    -O0
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wconversion
    -fsanitize=thread,undefined
    # -fsanitize=address
)