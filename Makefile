# TODO: On windows make aways recompiles?
OUTDIR=build
CC=g++
CFLAGS=-g -Isrc `pkg-config --cflags freetype2`
LIBS=-lX11 -lGL `pkg-config --libs freetype2` -lm
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

define compile
	@echo "Compile $(1) -> $(2)"
	@libs=`$(call grep_libs, $(1))` \
	&& gcc -ggdb -DMFT_WITH_MAIN -I./src/ $(1) -o $(2) -lc -lm $$libs
endef


HEADERS=$(wildcard src/*.h)
IGNORE_SINGLE_COMPILE=src/mf_platform_opengl.h

TOOLS=$(wildcard tools/*.c)
TOOLS_BIN=$(subst tools/,build/,$(basename $(TOOLS)))

EXAMPLES=$(wildcard examples/*.c)
EXAMPLES_BIN=$(subst examples/,build/,$(basename $(EXAMPLES)))

TEST_SOURCES=$(wildcard tests/*.c)
TESTS_BIN=$(subst tests/,build/,$(basename $(TEST_SOURCES)))

$(info TESTS=$(TESTS_BIN))
$(info EXAMPLES=$(EXAMPLES_BIN))


all: build tools examples
examples: $(EXAMPLES_BIN)
tools: $(TOOLS_BIN)

.PHONY: test
test: $(TESTS_BIN)
	@for t in $(TESTS_BIN); do ./$$t; done
tdd:
	ag -l | entr -c -s "make test"


# Compiles all headers to check if they compile individually
compile_headers: $(HEADERS)
	@for header in $(filter-out $(IGNORE_SINGLE_COMPILE),$(HEADERS)); do \
		gcc -c $$header -o build/single.gch; \
	done



.PHONY: runtests
runtests: $(TESTS_BIN)
	@for test in $(TESTS_BIN); do \
		# echo "RUN TESTS OF " $$test; \
		./$$test; \
	done

build:
	@mkdir -p build


$(TESTS_BIN): $(TESTS)

build/%: tests/%.c $(HEADERS)
	@libs=`$(call grep_libs, $<)` \
	&& gcc -ggdb -DMFT_WITH_MAIN -I./src/ $< -o $@ -lc -lm $$libs

build/%: ./**/%.c $(HEADERS)
	$(call compile,$<,$@) 

clean:
ifeq ($(OS),Windows_NT)
	del /F /Q $(OUTDIR)\*
else
	rm $(OUTDIR)/*
endif

# NOTE: this is just for my VIM setup <F5> will execute :make run
run:
	./build/GL06_CoordinateSystems

