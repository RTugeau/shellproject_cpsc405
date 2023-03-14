CC = gcc 
CFLAGS = -g -Wall

default: shell

shell: main.o
	$(CC) $(CFLAGS) -o shell main.o

main.o: main.c main.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f shell *.o
