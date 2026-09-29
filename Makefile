.DEFAULT_GOAL := run

DBG = DEBUG

TARGET  = forest
BUILD_DIR = build
SRCS    = src/main.cpp \
          src/execution/executor.cpp \
          src/test/test.cpp

OBJS    = $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

CC = g++

DEBUG_FLAGS = -D_DEBUG \
              -ggdb3 \
              -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr \
              -fno-sanitize-recover=all

CFLAGS = -std=c++17 -Wall \
        -Wextra \
        -Weffc++ \
        -Waggressive-loop-optimizations \
        -Wmissing-declarations \
        -Wcast-align \
        -Wcast-qual \
        -Wchar-subscripts \
        -Wconversion \
        -Wctor-dtor-privacy \
        -Wempty-body \
        -Wfloat-equal \
        -Wformat-nonliteral \
        -Wformat-security \
        -Wformat-signedness \
        -Wformat=2 \
        -Winline \
        -Wlogical-op \
        -Wnon-virtual-dtor \
        -Wopenmp-simd \
        -Woverloaded-virtual \
        -Wpacked \
        -Wpointer-arith \
        -Winit-self \
        -Wredundant-decls \
        -Wshadow \
        -Wsign-conversion \
        -Wsign-promo \
        -Wstrict-overflow=2 \
        -Wsuggest-attribute=noreturn \
        -Wsuggest-final-methods \
        -Wsuggest-final-types \
        -Wsuggest-override \
        -Wswitch-default \
        -Wswitch-enum \
        -Wundef \
        -Wunreachable-code \
        -Wunused \
        -Wuseless-cast \
        -Wvariadic-macros \
        -Wno-literal-suffix \
        -Wno-missing-field-initializers \
        -Wno-narrowing \
        -Wno-old-style-cast \
        -Wno-varargs \
        -Wstack-protector \
        -Wzero-as-null-pointer-constant \
        -Wduplicated-cond \
        -Wduplicated-branches \
        -Wnull-dereference \
        -Wrestrict \
        -fcheck-new -fsized-deallocation \
        -fstack-protector \
        -fstrict-overflow \
        -flto-odr-type-merging \
        -fno-omit-frame-pointer \
        -Wlarger-than=30000 \
        -Wstack-usage=8192 \
        -pie \
        -fPIE \
        -Werror=vla \
        -I./src

RELEASE_FLAGS = -O2 -march=native -g -DNDEBUG -flto -fno-rtti -fno-exceptions

ifeq ($(DBG), DEBUG)
    CFLAGS += $(DEBUG_FLAGS)
else
    CFLAGS += $(RELEASE_FLAGS)
endif

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@$(CC) $(CFLAGS) -o $@ $(OBJS)

$(BUILD_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	@rm -rf $(BUILD_DIR) $(TARGET)