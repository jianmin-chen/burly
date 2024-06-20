CC = gcc
CFLAGS = -Wall -Wextra -O2

SRC = $(wildcard utils/*.c) $(wildcard language/*.c) burly.c
TEST = $(wildcard utils/*.c) test.c

all: burly

burly: $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

test: $(TEST)
	$(CC) $(CFLAGS) -o $@ $^