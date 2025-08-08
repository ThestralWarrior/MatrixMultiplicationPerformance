CC = gcc
CFLAGS = -lm

VECFLAGS = 

ifeq ($(VEC),sse)
	VECFLAGS = -msse4.2 -DVECTYPE=\"sse\"
else ifeq ($(VEC),avx)
	VECFLAGS = -mavx -DVECTYPE=\"avx\"
else ifeq ($(VEC),avx2)
	VECFLAGS = -mavx2 -mfma -DVECTYPE=\"avx2\"
else
	VECFLAGS = -DVECTYPE=\"none\"
endif	

all:
	$(CC) -o matrixmul matrixmul.c $(VECFLAGS) $(CFLAGS)

clean:
	rm -f matrixmul

