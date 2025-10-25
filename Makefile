# Makefile for gate_ctrl application
# CC: Compiler for C programs, gcc for here
# CFLAGS: Compiler flags
# SRC: Source files: list the source files to compile
# OUT: Output executable file name

CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -pthread -Iinclude
LDFLAGS = -pthread

SRC = src/main.c src/threads.c src/queue.c src/locks.c src/metrics.c

OUT = gate_ctrl

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(LDFLAGS)

clean:
	rm -f $(OUT)
