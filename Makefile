.PHONY : all clean

ZIPNAME = xvinog00
TARGET = compiler
CC = gcc
CFLAGS = -std=c17 -Werror -Wall -Wextra

# directories
SRCDIR = src

# files
SRCFILES = $(wildcard $(SRCDIR)/*.c)
TESTING = $(filter-out $(SRCDIR)/compiler.c, $(SRCFILES))

all: $(TARGET)

$(TARGET): $(SRCFILES)
	gcc $(CFLAGS) $^  -o $@

test: test.c $(TESTING)
	gcc $(CFLAGS) $^ -g -o $@

clean:
	rm -f *.o $(TARGET)