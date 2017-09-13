CC=gcc

SRC_DIR = src
OBJ_DIR = objects
INC_DIR = include

CFLAGS = -std=c11 -Wall -I$(INC_DIR) -g
LFLAGS = -pthread

SRCS=$(wildcard $(SRC_DIR)/*.c)
OBJS=$(addprefix $(OBJ_DIR)/, $(notdir $(SRCS:.c=.o)))
BIN = webserver

all: $(BIN)

$(BIN): $(OBJS) 
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BIN) $(OBJS)
