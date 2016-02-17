SHELL  =/bin/bash
CC     =gcc
CFLAGS =-Iinclude
CFLAGS+=-I../serial/include
MEXOUT = -o

# check windows arch, change mex -o switch to -output
ifeq ($(shell mexext), mexw64)
    MEXOUT = -output
else ifeq ($(shell mexext), mexw32)
    MEXOUT = -output
endif

# always make
default: python/pyrex_maccepa.so python/pyrex_edinburghvsa.so 

# make if we have matlab installed
mex: m-files/maccepa.$(shell mexext)      m-files/model_maccepa.$(shell mexext) \
     m-files/edinburghvsa.$(shell mexext) m-files/model_edinburghvsa.$(shell mexext) 

build/%.c  : python/%.pyx
	cython         -o $@ $< 
build/pyrex_%.o: build/pyrex_%.c
	$(CC) -o $@ -c $< $(CFLAGS) $(shell python-config --cflags) -fPIC 
build/lib%.o: src/lib%.c include/lib%.h sketchbook/%/defines.h
	$(CC) -o $@ -c $< $(CFLAGS) -Isketchbook/$(subst .c,,$(subst src/lib,,$<)) -fPIC
python/pyrex_%.so     : build/pyrex_%.o      build/%.o      build/lib%.o      ../serial/build/serial.o
	$(CC) -shared -o $@ $^ $(shell python-config --ldflags) -lrt
m-files/model_%.$(shell mexext): build/lib%.o src/mex_lib%.c 	
	mex $(MEXOUT) $@ $^ -DMEX_INTERFACE $(CFLAGS) -Isketchbook/$(subst .o,,$(subst build/lib,,$<))

../serial/build/serial.o:
	$(MAKE) -C ../serial
build/maccepa.o     : src/vsa_arduino_interface.c sketchbook/maccepa/defines.h
	$(CC) -o $@ -c $< $(CFLAGS) -DMACCEPA_INTERFACE      -Isketchbook/maccepa      -fPIC $(shell python-config --cflags)
build/edinburghvsa.o: src/vsa_arduino_interface.c sketchbook/edinburghvsa/defines.h
	$(CC) -o $@ -c $< $(CFLAGS) -DEDINBURGHVSA_INTERFACE -Isketchbook/edinburghvsa -fPIC $(shell python-config --cflags)

m-files/edinburghvsa.$(shell mexext): src/vsa_mex_interface.c src/vsa_arduino_interface.c include/vsa_arduino_interface.h sketchbook/edinburghvsa/defines.h ../serial/build/serial.o
	mex $(MEXOUT) $@ src/vsa_mex_interface.c src/vsa_arduino_interface.c ../serial/build/serial.o -DEDINBURGHVSA_INTERFACE -DMEX_INTERFACE -lrt $(CFLAGS) -Isketchbook/edinburghvsa
m-files/maccepa.$(shell mexext)     : src/vsa_mex_interface.c src/vsa_arduino_interface.c include/vsa_arduino_interface.h sketchbook/maccepa/defines.h ../serial/build/serial.o
	mex $(MEXOUT) $@ src/vsa_mex_interface.c src/vsa_arduino_interface.c ../serial/build/serial.o -DMACCEPA_INTERFACE      -DMEX_INTERFACE -lrt $(CFLAGS) -Isketchbook/maccepa

clean:
	rm -rf build/*.o build/*.c build/*.cpp python/*.so m-files/*.$(shell mexext) *.pyc

