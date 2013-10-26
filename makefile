ifeq ($(OS), Windows_NT)
	RM=del
	LDFLAGS += -lMingw32
else
ifeq ($(shell uname --kernel-name), Linux)
	RM=rm
endif
endif





CC = cc
CFLAGS += -std=c99 -pedantic-errors -g -Wall -Wextra -O2
LDFLAGS += -lm -lSDLmain -lSDL -lSDL_image
BIN=a.out
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
DEP=$(SRC:.c=.d)
$(BIN): $(OBJ)
	$(CC) $^ $(LDFLAGS) -o $(BIN)
-include $(DEP)
%.o: %.c
	$(CC) $< $(CFLAGS) -c -MMD -MP
run: $(BIN)
	$(BIN) spectrum2.jpg
clean:
	$(RM) $(OBJ) $(DEP) $(BIN)
	
.PHONY: run clean