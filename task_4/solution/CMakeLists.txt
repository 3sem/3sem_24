cmake_minimum_required(VERSION 3.16)

project(Integrator)

add_executable(integrator.x ./main.c
	src/integrator.c)

target_include_directories(integrator.x PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(integrator.x PRIVATE
	-D_DEBUG
	-ggdb3
	-O0
	-Wall
	-Wextra
	-Wmissing-declarations
	-Wcast-align
	-Wcast-qual
	-Wchar-subscripts
	-Wconversion
	-Wempty-body
	-Wfloat-equal
	-Wformat-nonliteral
	-Wformat-security
	-Wformat=2
	-Winline
	-Wpacked
	-Wpointer-arith
	-Wredundant-decls
	-Wshadow
	-Wsign-conversion
	-Wstrict-overflow=2
	-Wswitch-default
	-Wswitch-enum
	-Wundef
	-Wunreachable-code
	-Wunused
	-Wvariadic-macros
	-Wno-missing-field-initializers
	-Wno-narrowing
	-Wstack-protector
	-fstack-protector
	-fstrict-overflow
	-fno-omit-frame-pointer
	-Wlarger-than=8192
	-fPIE
	-Werror=vla
	-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
)

target_link_options(integrator.x PRIVATE
	-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
)
