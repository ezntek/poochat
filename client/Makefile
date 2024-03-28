OBJ = main.o
CFLAGS ?= -O2 -Wall -Wpedantic -march=native
CC ?= cc
TARGET ?= ../

build: $(OBJ)
	$(CC) $(CFLAGS) -o $(OUTDIR)poochat $(OBJ)

main.o:

clean:
	rm -f $(OUTDIR)poochat $(OBJ)

.PHONY: clean
