CC=gcc
CFLAGS=-std=c11 -Wall -Iinclude
LFLAGS=-pthread
SRCFOLDER=src
OBJFOLDER=objects
INCLFOLDER=include
C_FILES=$(wildcard $(SRCFOLDER)/*.c)
OBJ_FILES=$(addprefix $(OBJFOLDER)/, $(notdir $(C_FILES:.c=.o)))
HEADER_FILES=$(wildcard $(INCLFOLDER)/*.h)

all: webserver

webserver: $(OBJ_FILES) 
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@

$(OBJFOLDER)/%.o: $(SRCFOLDER)/%.c $(HEADER_FILES)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f webserver $(OBJ_FILES)
