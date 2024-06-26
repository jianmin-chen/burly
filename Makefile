CC = gcc
CFLAGS = -Wno-error

SRC = $(wildcard utils/*.c) $(wildcard language/*.c) burly.c
TEST = $(wildcard utils/*.c) test.c

all: burly

burly: $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

test: $(TEST)
	$(CC) $(CFLAGS) -o $@ $^