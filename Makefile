CC=gcc

SRC_DIR = src
OBJ_DIR = objects
INC_DIR = include

CFLAGS = -std=c11 -Wall -I$(INC_DIR) -g -D_XOPEN_SOURCE -D_XOPEN_SOURCE_EXTENDED 
LFLAGS = -pthread -lbsd

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(addprefix $(OBJ_DIR)/, $(notdir $(SRCS:.c=.o)))
DEPS = $(OBJS:%.o=%.d)
BIN = webserver

all: $(BIN)

$(BIN): $(OBJS) 
	$(CC) $(CFLAGS)  $^ -o $@ $(LFLAGS)

-include $(DEPS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -f $(BIN) $(OBJS) $(DEPS)
