
CC = gcc
CFLAGS = -Wall
PFlags = -pthread

part1: slow_functions.o part1.o
	$(CC) $(CFLAGS) $(PFlags) -o $@ $^

