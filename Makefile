CC = gcc
CFLAGS =

ifeq ($(VEC),enabled)
	CFLAGS += -DVECTORIZE=\"enabled\" -O3 -march=native
else
	CFLAGS += -DVECTORIZE=\"disabled\"
endif

CFLAGS += -lm

all:
	$(CC) -o matrixmul matrixmul.c $(CFLAGS)

clean:
	rm -f matrixmul

