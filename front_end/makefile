TARGET = lan
CC = g++
CFLAGS = -D _DEBUG -ggdb3 -O3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations \
-Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral \
-Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith \
-Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn \
-Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code -Wunused \
-Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector \
-fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wstack-usage=8192 -pie -fPIE -Werror=vla \
-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

IFLAGS = -I./include/ -I./../common/include/

SRC_FOLDER = ./src/
OBJ_FOLDER = ./obj/

SRC_COMMON = ./../common/src/
OBJ_COMMON = ./../common/obj/

$(OBJ_COMMON)%.o : $(SRC_COMMON)%.cpp
	@mkdir -p $(@D)
	@$(CC) $(IFLAGS) $(CFLAGS) -c $< -o $@

SRC    = $(wildcard $(SRC_FOLDER)*.cpp)
OBJ    = $(patsubst $(SRC_FOLDER)%.cpp, $(OBJ_FOLDER)%.o, $(SRC))
SRC_C  = $(wildcard $(SRC_COMMON)*.cpp)
OBJ   += $(patsubst $(SRC_COMMON)%.cpp, $(OBJ_COMMON)%.o, $(SRC_C))


$(TARGET) : $(OBJ) 
	@$(CC) $(OBJ) $(CFLAGS) -o $(TARGET)

$(PREF_OBJ)%.o : $(PREF_SRC)%.cpp
	@$(CC) $(DEFINE) $(CFLAGS) -c $< -o $@

$(TARGET) : $(OBJ)
	@$(CC) $(IFLAGS) $(CFLAGS) $(OBJ) -o $(TARGET) -lm

$(OBJ_FOLDER)%.o : $(SRC_FOLDER)%.cpp
	@mkdir -p $(@D)
	@$(CC) $(IFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm $(TARGET) $(OBJ)

