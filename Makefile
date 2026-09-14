# Build and test the toolkit on any machine with a C compiler.
#
#   make test     compile and run every test suite
#   make lib      build the static library only
#   make clean    remove the build directory
#
# The warning flags are deliberately strict. -Werror turns every warning into
# an error, which is the setting most embedded teams use: a warning that is
# allowed to scroll past is a warning nobody reads.

CC      ?= gcc
CFLAGS  = -std=c99 -Wall -Wextra -Wpedantic -Werror -Isrc -Itests -O2
LDLIBS  = -lm

SRC     = $(wildcard src/*.c)
OBJ     = $(patsubst src/%.c,build/%.o,$(SRC))
TESTSRC = $(wildcard tests/test_*.c)
TESTBIN = $(patsubst tests/%.c,build/%,$(TESTSRC))

all: test

build:
	mkdir -p build

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

lib: build/libembedded.a

build/libembedded.a: $(OBJ)
	ar rcs $@ $(OBJ)

build/test_%: tests/test_%.c $(OBJ) | build
	$(CC) $(CFLAGS) $< $(OBJ) -o $@ $(LDLIBS)

test: $(TESTBIN)
	@echo "running $(words $(TESTBIN)) test suites"
	@echo "-------------------------------------------"
	@fail=0; for t in $(TESTBIN); do ./$$t || fail=1; done; \
	 echo "-------------------------------------------"; \
	 if [ $$fail -eq 0 ]; then echo "all suites passed"; else echo "FAILURES"; fi; \
	 exit $$fail

clean:
	rm -rf build

.PHONY: all lib test clean
