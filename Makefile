
CC=g++
CFLAGS=-g -Isrc
LIBS=-lX11 -lGL

HEADERS=$(wildcard src/*.h)

all: build/example-platform build/example-renderer


build/example-platform: examples/example_platform.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< $(LIBS) -o $@

build/example-renderer: examples/example_renderer.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< $(LIBS) -o $@


clean:
	rm build/*

