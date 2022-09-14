CC=clang
CFLAGS=-std=c99

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
CFILES = $(call rwildcard,src/,*.c) $(call rwildcard,deps/,*.c)
TEST_CFILES = $(call rwildcard,test/,*.c) $(call rwildcard,deps/,*.c)

.PHONY: clean

all: $(CFILES)
	$(CC) $(CFLAGS) $(CFILES) -o program

test: $(TEST_CFILES)
	$(CC) $(CFLAGS) $(TEST_CFILES) -o test_program
	./test_program

clean:
	rm -f program
