OBJ = main.o
CFLAGS ?= -O2 -Wall -Wpedantic -march=native
CC ?= cc

build: $(OBJ)

buildexe: build
	$(CC) $(CFLAGS) -o poochat-server $(OBJ)

main.o:

clean:
	rm -f poochat-server $(OBJ)

.PHONY: clean
