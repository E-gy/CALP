CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wno-unused -Werror
LDFLAGS =

LIB = libCALP.a

SRC_DIR ?= src/main
TEST_DIR ?= src/test

CS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(CS:.c=.o)
$(LIB): $(OBJS)
	ar rcs $@ $^

lib: CFLAGS += -O3
lib: clean $(LIB)
lib_d: CFLAGS += -ggdb -O0 -D_DEBUG
lib_d: clean $(LIB)

all: lib

$(TEST_DIR)/%: $(TEST_DIR)/%.c
	$(CC) -ggdb -O0 -D_DEBUG -o $@ $^ -L. -lCALP

check: lib_d src/test/basic src/test/advanced src/test/mathg
	./src/test/basic
	./src/test/advanced
	./src/test/mathg

clean:
	$(RM) $(LIB) $(OBJS)

.PHONY: all lib lib_d check clean
