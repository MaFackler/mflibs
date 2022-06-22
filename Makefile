# TODO: On windows make aways recompiles?
OUTDIR=build
CC=g++
CFLAGS=-g -Isrc
LIBS=-lX11 -lGL
OUTPUT=-o 
objects=

ifeq ($(OS),Windows_NT)
	CC=cl
	CFLAGS=/Zi /EHsc /Isrc
	LIBS=/link kernel32.lib user32.lib winmm.lib opengl32.lib Gdi32.lib
	OUTPUT=/Fe:
	objects=/Fd$(OUTDIR)/ /Fo:$(addsuffix .obj, $(1))
endif

HEADERS=$(wildcard src/*.h)
TESTS=$(wildcard tests/*.cpp)

all:  build $(OUTDIR)/runtests examples

build:
	mkdir build

examples: $(OUTDIR)/example-platform $(OUTDIR)/example-renderer $(OUTDIR)/tetris $(OUTDIR)/debug $(OUTDIR)/craft $(OUTDIR)/gui $(OUTDIR)/ray

$(OUTDIR)/craft: examples/craft.cpp $(HEADERS)
	$(CC) $(OUTPUT)$@ $(call objects, $@) $(CFLAGS) $< $(LIBS)

$(OUTDIR)/debug: examples/debug.cpp $(HEADERS)
	$(CC) $(OUTPUT)$@ $(call objects, $@) $(CFLAGS) $< $(LIBS)

$(OUTDIR)/example-platform: examples/example_platform.cpp $(HEADERS)
	$(CC) $(OUTPUT)$@ $(call objects, $@) $(CFLAGS) $< $(LIBS)

$(OUTDIR)/example-renderer: examples/example_renderer.cpp $(HEADERS)
	$(CC) $(OUTPUT)$@ $(call objects, $@) $(CFLAGS) $< $(LIBS)

$(OUTDIR)/tetris: examples/tetris.cpp $(HEADERS)
	$(CC) $(OUTPUT)$@ $(call objects, $@) $(CFLAGS) $< $(LIBS)

$(OUTDIR)/gui: examples/gui.cpp $(HEADERS)
	$(CC) $(OUTPUT)$@ $(call objects, $@) $(CFLAGS) `pkg-config --cflags freetype2` $< $(LIBS) `pkg-config --libs freetype2`

$(OUTDIR)/ray: examples/ray.cpp $(HEADERS)
	$(CC) $(OUTPUT)$@ $(call objects, $@) $(CFLAGS) $<


$(OUTDIR)/runtests: $(HEADERS) $(TESTS)
	$(CC) $(OUTPUT)$@ $(call objects, $@) $(CFLAGS) tests/tests.cpp



test: $(OUTDIR)/runtests
	./$<


.PHONY: run
run:
	$(OUTDIR)/craft


# NOTE: this is just to check that the header files will also compile with c99
# TODO: c99 support?
$(OUTDIR)/all-in-c: examples/all_in_c.c
ifneq ($(OS),Windows_NT)
	gcc $(CFLAGS) -std=c99 $< $(LIBS) -o $@
endif

clean:
ifeq ($(OS),Windows_NT)
	del /F /Q $(OUTDIR)\*
else
	rm $(OUTDIR)/*
endif

