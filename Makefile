CC = gcc
CFLAGS = -Wall -Wextra -O2

SRC = $(wildcard utils/*.c) $(wildcard language/*.c) burly.c

all: burly

burly: $(SRC)
	$(CC) $(CFLAGS) -o $@ $^