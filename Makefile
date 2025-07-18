CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
LDFLAGS = -lncurses

SRC = src/main.c src/ui.c src/notes.c src/poller.c
OBJ = $(SRC:.c=.o)
BIN = termnote

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(BIN) $(OBJ)

.PHONY: all clean

