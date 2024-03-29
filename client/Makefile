OBJ = main.o cmds.o
CFLAGS ?= -O2 -Wall -Wpedantic -march=native
CC ?= cc

build: $(OBJ)

buildexe: build
	$(CC) $(CFLAGS) -o poochat $(OBJ)

main.o:
cmds.o:

clean:
	rm -f $(OBJ) poochat

.PHONY: clean
