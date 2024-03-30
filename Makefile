CFLAGS ?= -O2 -Wall -Wpedantic -march=native
CC ?= cc
EXTERNAL_DEPS = libpaho-mqtt3c.a libjson-c.a
DEP_MAKEOPTS ?= -j4

all: parts copydeps
	$(CC) -o poochat-server server/*.o $(EXTERNAL_DEPS)
	$(CC) -o poochat client/*.o $(EXTERNAL_DEPS)

parts:
	make -C server build
	make -C client build

cleanall: clean	
	rm -f *.a 
	rm -rf ./external/*

clean:
	make -C server clean
	make -C client clean

obtaindeps: obtainpaho obtainjsonc

obtainpaho:
	if [ ! -d ./external/paho-mqtt-c ]; then \
		git clone https://github.com/eclipse/paho.mqtt.c --depth=1 ./external/paho-mqtt-c; \
	fi

obtainjsonc:
	if [ ! -d ./external/json-c ]; then \
		git clone https://github.com/json-c/json-c --depth=1 ./external/json-c; \
	fi

builddeps: obtaindeps buildpaho buildjsonc	

buildpaho:
	if [ ! -f ./external/paho-mqtt-c/build/src/libpaho-mqtt3c.a ]; then \
		cd ./external/paho-mqtt-c; \
			mkdir build; \
			cd build; \
			cmake -DPAHO_BUILD_SHARED=FALSE -DPAHO_BUILD_STATIC=TRUE -DPAHO_HIGH_PERFORMANCE=FALSE ..; \
		make $(DEP_MAKEOPTS); \
	fi

buildjsonc:
	if [ ! -f ./external/json-c/build/libjson-c.a ]; then \
		cd ./external/json-c; \
		mkdir build; \
		cd build; \
		cmake -DBUILD_SHARED_LIBS=OFF ..; \
		make $(DEP_MAKEOPTS); \
	fi

copydeps: builddeps copyjsonc copypaho

copypaho:
	cp ./external/paho-mqtt-c/build/src/libpaho-mqtt3c.a ./

copyjsonc:
	cp ./external/json-c/build/libjson-c.a ./

.PHONY: clean
