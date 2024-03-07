# TODO: On windows make aways recompiles?
OUTDIR=build
CC=g++
CFLAGS=-g -Isrc `pkg-config --cflags freetype2`
LIBS=-lX11 -lGL `pkg-config --libs freetype2` 
OUTPUT=-o 
objects=

ifeq ($(OS),Windows_NT)
	CC=cl
	CFLAGS=/Zi /EHsc /Isrc
	LIBS=/link /DEBUG:full kernel32.lib user32.lib winmm.lib opengl32.lib Gdi32.lib
	OUTPUT=/Fe:
	objects=/Fd$(OUTDIR)/ /Fo:$(addsuffix .obj, $(1))
endif

define grep_libs
	grep 'pragma comment(lib' $(1) | sed -r 's/#pragma comment\(lib, "(.*)"\)/ -l\1/' | tr -d '\n'
endef

HEADERS=$(wildcard src/*.h)
TEST_SOURCES=$(wildcard tests/*.c)
TESTS_BIN=$(basename $(TEST_SOURCES))
TESTS_BIN=$(subst tests/,build/,$(basename $(TEST_SOURCES)))

$(info TESTS=$(TESTS_BIN))

top: runtests

all:  build runtests examples

compile_headers: $(HEADERS)
	for header in $(HEADERS); do \
		gcc -c $$header; \
	done



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
	$(CC) $(OUTPUT)$@ $(call objects, $@) $(CFLAGS) $< $(LIBS) `pkg-config --libs freetype2`

$(OUTDIR)/ray: examples/ray.cpp $(HEADERS)
	$(CC) $(OUTPUT)$@ $(call objects, $@) $(CFLAGS) $<


.PHONY: runtests
runtests: $(TESTS_BIN)
	@for test in $(TESTS_BIN); do \
		# echo "RUN TESTS OF " $$test; \
		./$$test; \
	done



$(TESTS_BIN): $(TESTS)

build/%: tests/%.c $(HEADERS)
	@libs=`$(call grep_libs, $<)` \
	&& gcc -ggdb -DMFT_WITH_MAIN -I./src/ $< -o $@ -lc $$libs

.PHONY: test
test: $(TESTS_BIN)
	@for t in $(TESTS_BIN); do ./$$t; done

tdd:
	ag -l | entr -c -s "make test"

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

