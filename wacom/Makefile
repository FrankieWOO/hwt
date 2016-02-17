default: capture_pointer

capture_pointer: capture_pointer.c
	gcc $< $(shell pkg-config --cflags --libs x11) -o $@

clean:
	rm -f capture_pointer
