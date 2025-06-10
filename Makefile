
CC = gcc
CFLAGS = -Wall
PFlags = -pthread

part1: slow_functions.o part1.o
	$(CC) $(CFLAGS) $(PFlags) -o $@ $^

part2: part2.o slow_functions.o
	$(CC) $(CFLAGS) $(PFlags) -o $@ $^

