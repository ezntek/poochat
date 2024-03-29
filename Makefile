CFLAGS ?= -O2 -Wall -Wpedantic -march=native
CC ?= cc
PAHO_MAKEOPTS ?= -j4
PAHO_CMAKEOPTS = 

all: parts builddeps
	cp ./external/paho-mqtt-c/build/src/libpaho-mqtt3c.a ./
	$(CC) -o poochat-server server/*.o libpaho-mqtt3c.a
	$(CC) -o poochat client/*.o libpaho-mqtt3c.a

parts:
	make -C server build
	make -C client build

clean:
	make -C server clean
	make -C client clean
	rm -f *.a 
	rm -rf ./external/*

obtaindeps:
	if [ ! -d ./external/paho-mqtt-c ]; then \
		git clone https://github.com/eclipse/paho.mqtt.c --depth=1 ./external/paho-mqtt-c; \
	fi

builddeps: obtaindeps
	if [ ! -f ./external/paho-mqtt-c/build/src/libpaho-mqtt3c.a ]; then \
		cd ./external/paho-mqtt-c; \
			mkdir build; \
			cd build; \
			cmake -DPAHO_BUILD_SHARED=FALSE -DPAHO_BUILD_STATIC=TRUE -DPAHO_HIGH_PERFORMANCE=FALSE $(PAHO_CMAKEOPTS) ..; \
		make $(PAHO_MAKEOPTS); \
	fi


.PHONY: clean
