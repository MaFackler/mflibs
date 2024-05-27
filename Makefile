OUTDIR=build
CFLAGS=-g -Wall -Wno-missing-braces -Wno-parentheses -Werror -Wno-unused-value
OUTPUT=-o 
objects=
LIB_MACRO=MF_LIB_LINUX
POSTFIX=
ifeq ($(PLATFORM),WINDOWS)
LIB_MACRO=MF_LIB_WINDOWS
CC=x86_64-w64-mingw32-gcc
POSTFIX=.exe
endif

HEADERS=$(wildcard src/*.h)
IGNORE_SINGLE_COMPILE=src/mf_platform_opengl.h

TOOLS=$(wildcard tools/*.c)
TESTS_BIN=$(patsubst tools/%.c,build/%$(POSTFIX),$(TOOLS))

EXAMPLES=$(wildcard examples/*.c)
EXAMPLES_BIN=$(patsubst examples/%.c,build/%$(POSTFIX),$(EXAMPLES))

TEST_SOURCES=$(wildcard tests/*.c)
# TESTS_BIN=$(subst tests/,build/,$(basename $(TEST_SOURCES)))
TESTS_BIN=$(patsubst tests/%.c,build/%$(POSTFIX),$(TEST_SOURCES))

$(info TESTS=$(TESTS_BIN))
$(info EXAMPLES=$(EXAMPLES_BIN))



# ifeq ($(OS),Windows_NT)
# 	CFLAGS=/Zi /EHsc /Isrc
# 	LIBS=/link /DEBUG:full kernel32.lib user32.lib winmm.lib opengl32.lib Gdi32.lib
# 	OUTPUT=/Fe:
# 	objects=/Fd$(OUTDIR)/ /Fo:$(addsuffix .obj, $(1))
# endif

define grep_libs
	grep '$(LIB_MACRO)' $(1) | sed -r 's/$(LIB_MACRO)\("(.*)"\)/ -l\1/' | tr -d '\n'
endef

define compile
	@echo "Compile $(1) -> $(2)"
	@libs=`$(call grep_libs, $(1))` \
	&& $(CC) $(CFLAGS) -DMFT_WITH_MAIN -I./src/ $(1) -o $(2) $$libs
endef




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
		$(CC) -c $$header -o build/single.gch; \
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

build/%$(POSTFIX): tests/%.c $(HEADERS)
	@libs=`$(call grep_libs, $<)` \
	&& $(CC) -ggdb -DMFT_WITH_MAIN -I./src/ $< -o $@ -lc -lm $$libs

build/%$(POSTFIX): ./**/%.c $(HEADERS)
	$(call compile,$<,$@) 

clean:
ifeq ($(OS),Windows_NT)
	del /F /Q $(OUTDIR)\*
else
	rm $(OUTDIR)/*
endif

# NOTE: this is just for my VIM setup <F5> will execute :make run
run:
	./build/GL10_materials

