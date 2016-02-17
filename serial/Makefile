SHELL=/bin/bash

default: build/serial.o

build/serial.o: src/serial.c include/serial.h
	gcc -o build/serial.o -c src/serial.c  -Iinclude -fPIC

clean:
	rm -f build/serial.o
