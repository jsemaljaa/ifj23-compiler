.PHONY : all clean

ZIPNAME = xvinog00
TARGET = compiler
CC = gcc
CFLAGS = -std=c17 -Werror -Wall -Wextra

# directories
SRCDIR = src

all: $(TARGET)

$(TARGET): $(SRCDIR)/*.c
	gcc $(CFLAGS) $^  -o $@

clean:
	rm -f *.o $(TARGET)