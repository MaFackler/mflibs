
CC=g++
CFLAGS=-g -Isrc
LIBS=-lX11 -lGL

HEADERS=$(wildcard src/*.h)

all: build/example-platform build/example-renderer build/all-in-c


build/example-platform: examples/example_platform.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< $(LIBS) -o $@

build/example-renderer: examples/example_renderer.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< $(LIBS) -o $@


# NOTE: this is just to check that the header files will also compile with c99
build/all-in-c: examples/all_in_c.c
	gcc $(CFLAGS) -std=c99 $< $(LIBS) -o $@

clean:
	rm build/*

