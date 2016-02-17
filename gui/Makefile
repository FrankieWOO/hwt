SHELL  =/bin/bash
CC     =g++
CFLAGS =-Iinclude
CFLAGS+=$(shell fltk-config --cxxflags --use-gl)

default: python/pyrex_gui.so

build/%.cpp: python/%.pyx
	cython --cplus -o $@ $< 
build/pyrex_%.o: build/pyrex_%.cpp
	$(CC) -o $@ -c $< $(CFLAGS) $(shell python-config --cflags) -fPIC 
build/pyrex_gui.cpp: python/pyrex_gui.pyx python/sliderbox.pyx python/scope.pyx
	cython --cplus -o $@ python/pyrex_gui.pyx
build/scope.o: src/scope.cpp include/scope.h
	$(CC) -o $@ -c $< $(CFLAGS) -fPIC
build/gui.o: src/gui.cpp include/gui.h include/sliderbox.h include/scope.h
	$(CC) -o $@ -c $< $(CFLAGS) -fPIC $(shell python-config --cflags)
python/pyrex_gui.so: build/pyrex_gui.o build/gui.o build/scope.o
	$(CC) -shared -o $@ $^ $(shell fltk-config --ldflags --use-gl) $(shell python-config --ldflags)

clean:
	rm -rf build/*.o build/*.c build/*.cpp python/*.so m-files/*.$(shell mexext) *.pyc

