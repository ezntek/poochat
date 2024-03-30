CFLAGS ?= -O2 -Wall -Wpedantic -march=native
CC ?= cc
EXTERNAL_DEPS = libpaho-mqtt3c.a libcjson.a
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

obtaindeps: obtainpaho obtaincjson

obtainpaho:
	if [ ! -d ./external/paho-mqtt-c ]; then \
		git clone https://github.com/eclipse/paho.mqtt.c --depth=1 ./external/paho-mqtt-c; \
	fi

obtaincjson:
	if [ ! -d ./external/cJSON ]; then \
		git clone https://github.com/DaveGamble/cJSON --depth=1 ./external/cJSON; \
	fi

builddeps: obtaindeps buildpaho buildcjson	

buildpaho:
	if [ ! -f ./external/paho-mqtt-c/build/src/libpaho-mqtt3c.a ]; then \
		cd ./external/paho-mqtt-c; \
			mkdir build; \
			cd build; \
			cmake -DPAHO_BUILD_SHARED=FALSE -DPAHO_BUILD_STATIC=TRUE -DPAHO_HIGH_PERFORMANCE=FALSE ..; \
		make $(DEP_MAKEOPTS); \
	fi

buildcjson:
	if [ ! -f ./external/cJSON/build/libcjson.a ]; then \
		cd ./external/cJSON; \
		mkdir build; \
		cd build; \
		cmake -DENABLE_CJSON_TEST=Off -DBUILD_SHARED_AND_STATIC_LIBS=On ..; \
		make $(DEP_MAKEOPTS); \
	fi

copydeps: builddeps copycjson copypaho

copypaho:
	cp ./external/paho-mqtt-c/build/src/libpaho-mqtt3c.a ./

copycjson:
	cp ./external/cJSON/build/libcjson.a ./

.PHONY: clean
