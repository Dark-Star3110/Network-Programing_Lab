CC = gcc

all:	build

build: 		main.o error.o
						$(CC) main.o  error.o -o resolver

main.o: main.c
						$(CC) -c main.c

error.o: error.c
						$(CC) -c error.c

clean:
					rm -f *.o resolver