TARGET=main
CC=clang

HEADERS=-I./include/

SRC_DIR=source/
BUILD_DIR=build/

SRC = $(wildcard $(SRC_DIR)*.c)
OBJ = $(patsubst $(SRC_DIR)%.c, $(BUILD_DIR)%.o, $(SRC))

debug : CFLAGS=-Wno-conversion -Wno-unused-variable -Wno-pointer-arith -g -DDEBUG # -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wchar-subscripts -Wconditionally-supported -Wctor-dtor-privacy -Wempty-body -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
debug : $(TARGET)

release : CFLAGS=-mavx -march=native -Wno-narrowing -Wno-pointer-arith -O3
release : $(TARGET)

$(TARGET) : $(OBJ)
	@$(CC) $(HEADERS) $(CFLAGS) $^ -o $@

$(BUILD_DIR)%.o : $(SRC_DIR)%.c
	@$(CC) $(HEADERS) $(CFLAGS) -c $^ -o $@

dirs:
	mkdir build

clean :
	rm $(TARGET) $(BUILD_DIR)*.o
