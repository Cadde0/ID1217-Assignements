CC = gcc
CFLAGS = -Wall -pthread

all: main

main: main.o palindrome.o
    $(CC) $(CFLAGS) -o main main.o palindrome.o

main.o: main.c palindrome.h
    $(CC) $(CFLAGS) -c main.c

palindrome.o: palindrome.c palindrome.h
    $(CC) $(CFLAGS) -c palindrome.c

clean:
    rm -f *.o main