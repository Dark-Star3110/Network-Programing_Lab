CC = gcc

all:	resolver

resolver: 		main.o
						$(CC) main.o -o resolver

main.o: main.c
						$(CC) -c main.c

clean:
					rm -f *.o resolver