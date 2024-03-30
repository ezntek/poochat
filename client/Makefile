OBJ = main.o cmds.o common.o packet.o
CFLAGS ?= -O2 -Wall -Wpedantic -march=native
CC ?= cc

build: $(OBJ)

buildexe: build
	$(CC) $(CFLAGS) -o poochat $(OBJ)

main.o:
cmds.o:
common.o:
packet.o:

clean:
	rm -f $(OBJ) poochat

.PHONY: clean
