CC=clang
CFLAGS=-std=c99

PROG_NAME=freight
ifeq ($(OS),Windows_NT)
	PROG_NAME=freight.exe
	CC=gcc
endif

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
CFILES = $(call rwildcard,src/,*.c) $(call rwildcard,deps/,*.c)

.PHONY: clean

all: $(CFILES)
	$(CC) $(CFLAGS) $(CFILES) -o $(PROG_NAME)

clean:
	rm -f $(PROG_NAME)
