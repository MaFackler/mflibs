OUTDIR=build
CC=g++
CFLAGS=-g -Isrc
LIBS=-lX11 -lGL
OUTPUT=-o 
objects=
RM=rm

ifeq ($(OS),Windows_NT)
	CC=cl
	CFLAGS=/Zi /EHsc /Isrc
	LIBS=/link kernel32.lib user32.lib winmm.lib opengl32.lib Gdi32.lib
	OUTPUT=/Fe:
	RM=del
	objects=/Fd$(OUTDIR)/ /Fo:$(addsuffix .obj, $(1))
endif

HEADERS=$(wildcard src/*.h)
TESTS=$(wildcard tests/*.cpp)

all:  $(OUTDIR)/all-in-c $(OUTDIR)/runtests examples

examples: $(OUTDIR)/example-platform $(OUTDIR)/example-renderer $(OUTDIR)/tetris


$(OUTDIR)/example-platform: examples/example_platform.cpp $(HEADERS)
	$(CC) $(OUTPUT)$@ $(call objects, $@) $(CFLAGS) $< $(LIBS)

$(OUTDIR)/example-renderer: examples/example_renderer.cpp $(HEADERS)
	$(CC) $(OUTPUT)$@ $(call objects, $@) $(CFLAGS) $< $(LIBS)

$(OUTDIR)/tetris: examples/tetris.cpp $(HEADERS)
	$(CC) $(OUTPUT)$@ $(call objects, $@) $(CFLAGS) $< $(LIBS)

$(OUTDIR)/runtests: $(HEADERS) $(TESTS)
	$(CC) $(OUTPUT)$@ $(CFLAGS) tests/tests.cpp



test: $(OUTDIR)/runtests
	./$<



# NOTE: this is just to check that the header files will also compile with c99
$(OUTDIR)/all-in-c: examples/all_in_c.c
ifneq ($(OS),Windows_NT)
	gcc $(CFLAGS) -std=c99 $< $(LIBS) -o $@
endif

clean:
	# TODO: exclude .gitkeep
	$(RM) $(OUTDIR)/*

