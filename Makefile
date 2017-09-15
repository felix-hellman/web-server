CC=gcc

SRC_DIR = src
OBJ_DIR = objects
INC_DIR = include

CFLAGS = -std=c11 -Wall -I$(INC_DIR) -g -D_XOPEN_SOURCE
LFLAGS = -pthread

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(addprefix $(OBJ_DIR)/, $(notdir $(SRCS:.c=.o)))
DEPS = $(OBJS:%.o=%.d)
BIN = webserver

all: $(BIN)

$(BIN): $(OBJS) 
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@

-include $(DEPS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -f $(BIN) $(OBJS) $(DEPS)
