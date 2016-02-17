SHELL  =/bin/bash
CC     =g++
CFLAGS =-Iinclude
CFLAGS+=-I/usr/include
LDFLAGS=-lportaudio

default: python/pyrex_audio.so

build/%.o: src/%.cpp
	$(CC) -o $@ -c $< $(CFLAGS) -fPIC
build/%.cpp: python/%.pyx
	cython --cplus -o $@ $< 
build/pyrex_%.o: build/pyrex_%.cpp
	$(CC) -o $@ -c $< $(CFLAGS) $(shell python-config --cflags) -fPIC 
python/pyrex_%.so: build/pyrex_%.o
	$(CC) -shared -o $@ $^ $(LDFLAGS)

python/pyrex_audio.so: build/pyrex_audio.o build/audio_interface.o
	$(CC) -shared -o $@ $^ $(LDFLAGS) $(shell python-config --ldflags)

clean:
	rm -rf build/*.o build/*.c build/*.cpp python/*.so m-files/*.$(shell mexext) *.pyc

