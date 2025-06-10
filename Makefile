CC = gcc
CFLAGS = -Wall -pthread

part1: slow_functions.o part1.o
	$(CC) $(CFLAGS) -o PART1 $^

part2: part2.o slow_functions.o
	$(CC) $(CFLAGS) -o PART2 $^

clean: 
	rm -f *.o PART1 PART2