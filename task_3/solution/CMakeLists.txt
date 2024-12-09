cmake_minimum_required(VERSION 3.16)

project(FIFO_main.x)

add_subdirectory(./utils ${CMAKE_BINARY_DIR}/utils)

add_executable(FIFO_main.x ./FIFO_main.c)
add_executable(shmem_main.x ./shmem_main.c)
add_executable(msg_main.x ./msg_main.c)

target_link_libraries(FIFO_main.x PRIVATE utils)
target_link_libraries(shmem_main.x PRIVATE utils)
target_link_libraries(msg_main.x PRIVATE utils)

target_include_directories(FIFO_main.x PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/utils/include
)
target_include_directories(shmem_main.x PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/utils/include
)
target_include_directories(msg_main.x PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/utils/include
)

set(COMPILE_OPTIONS
	-D_DEBUG
	-ggdb3
	-O0
	-Wall
	-Wextra
	-Weffc++
	-Wc++14-compat
	-Wmissing-declarations
	-Wcast-align
	-Wcast-qual
	-Wchar-subscripts
	-Wconversion
	-Wctor-dtor-privacy
	-Wempty-body
	-Wfloat-equal
	-Wformat-nonliteral
	-Wformat-security
	-Wformat=2
	-Winline
	-Wnon-virtual-dtor
	-Woverloaded-virtual
	-Wpacked
	-Wpointer-arith
	-Winit-self
	-Wredundant-decls
	-Wshadow
	-Wsign-conversion
	-Wsign-promo
	-Wstrict-overflow=2
	-Wsuggest-override
	-Wswitch-default
	-Wswitch-enum
	-Wundef
	-Wunreachable-code
	-Wunused
	-Wvariadic-macros
	-Wno-missing-field-initializers
	-Wno-narrowing
	-Wno-old-style-cast
	-Wno-varargs
	-Wstack-protector
	-fcheck-new
	-fsized-deallocation
	-fstack-protector
	-fstrict-overflow
	-fno-omit-frame-pointer
	-Wlarger-than=8192
	-fPIE
	-Werror=vla
	-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
)

target_link_options(FIFO_main.x PRIVATE
	-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
)

target_compile_options(FIFO_main.x PRIVATE ${COMPILE_OPTIONS})
target_link_options(FIFO_main.x PRIVATE
	-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
)

target_compile_options(shmem_main.x PRIVATE ${COMPILE_OPTIONS})
target_link_options(shmem_main.x PRIVATE
	-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
)

target_compile_options(msg_main.x PRIVATE ${COMPILE_OPTIONS})
target_link_options(msg_main.x PRIVATE
	-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
)
