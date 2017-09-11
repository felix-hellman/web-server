LFLAGS=-pthread
CC=gcc
CFLAGS=-std=c11
All: HTTP.o
	$(CC) $(CFLAGS) -o webserver src/webserver.c $(LFLAGS) HTTP.o
	rm HTTP.o

HTTP.o:
	$(CC) $(CFLAGS) -c src/HTTP.c
