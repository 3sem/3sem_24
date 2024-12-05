CXX = gcc
CXXFLAGS =  -D _DEBUG -ggdb3 -std=c17 -O0 -Wall -Wextra -Waggressive-loop-optimizations \
 		    -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts \
  			-Wconversion -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security \
   			-Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wopenmp-simd \
    		-Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion \
	 		-Wstrict-overflow=2 -Wsuggest-final-methods \
	  		-Wsuggest-final-types -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef \
	   		-Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros \
	    	-Wno-missing-field-initializers -Wno-narrowing -Wno-varargs -Wstack-protector \
		 	-fcheck-new -fstack-protector -fstrict-overflow -flto-odr-type-merging \
		  	-fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -Werror=vla \
			-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
#-O3 -march=core-avx2

TARGET = console
SourcePrefix = src/
BuildPrefix = build/
BuildFolder = build
Include = -Iinclude

Lib = -lreadline

Sources = console.c parser.c common.c exec.c
Main = main.c

Source = $(addprefix $(SourcePrefix), $(Sources))
MainObject = $(patsubst %.c, $(BuildPrefix)%.o, $(Main))

objects = $(patsubst $(SourcePrefix)%.c, $(BuildPrefix)%.o, $(Source))

.PHONY : all clean folder

all : release

release : CXXFLAGS = -O2 -std=c17
release : folder $(TARGET)
	@echo "[Release mode]"

debug : folder $(TARGET)
	@echo "[Debug mode]"

$(BuildPrefix)%.o : $(SourcePrefix)%.c
	@echo [CXX] -c $< -o $@
	@$(CXX) $(CXXFLAGS) $(Include) -c $< -o $@

$(TARGET) : $(objects) $(MainObject)
	@echo [CC] $^ -o $@
	@$(CXX) $(CXXFLAGS) $(Include) $(Lib) $^ -o $@

clean :
	rm $(BuildFolder)/*.o
	rm $(TARGET)

folder :
	mkdir -p $(BuildFolder)