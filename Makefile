# Try to find clang first, then fallback to gcc
CC := $(shell command -v clang 2>/dev/null || command -v gcc 2>/dev/null)

# If no compiler found, give an error
ifeq ($(CC),)
$(error No suitable C compiler found (clang or gcc))
endif

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

