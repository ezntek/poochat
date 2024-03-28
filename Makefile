CFLAGS ?= -O2 -Wall -Wpedantic -march=native
CC ?= cc

all:
	make -C server build
	make -C client build

clean:
	make -C server clean
	make -C client clean

.PHONY: clean
