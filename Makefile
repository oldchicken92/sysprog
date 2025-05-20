all: smsh2

CC = gcc
CFLAGS = -Wall

part1: smsh2.o splitline.o execute.o
	$(CC) $(CFLAGS) -o smsh2 $^

part2: smsh3.o splitline.o execute.o
	$(CC) $(CFLAGS) -o smsh3 $^

part3: smsh4.o splitline.o execute.o
	$(CC) $(CFLAGS) -o smsh4 $^