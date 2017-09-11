LFLAGS=-pthread
CC=gcc
CFLAGS=-std=c11
All:
	$(CC) $(CFLAGS) -o webserver src/webserver.c $(LFLAGS)
