#ifndef VECTYPE
#define VECTYPE "none"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define EPSILON 1e-6


double timeDiff(struct timespec *start, struct timespec *end) {
	return (end->tv_sec - start->tv_sec) + (end->tv_nsec - start->tv_nsec) / 1e9;
}

int determineOptimalBlockSize(int N) {
	long l1d = sysconf(_SC_LEVEL1_DCACHE_SIZE); // _SC_LEVEL1_DCACHE_SIZE is a GNU extension
	printf("L1 data cache size: %ld bytes (%.2f KiB)\n", l1d, l1d / 1024.0);
	
	int maxBlockSize = sqrt(l1d / (3.0 * sizeof(double)));
	int limit = N < maxBlockSize ? N : maxBlockSize;
	
	int blockSize = 1;
	while(blockSize * 2 <= limit) blockSize *= 2;
	
	return blockSize;
}

double **alloc2DMatrix(int N) {
	double **matrix = (double **)malloc(N * sizeof(double *));
	
	for(int i = 0; i < N; i++) {
		matrix[i] = (double *)malloc(N * sizeof(double));
	}

	return matrix;
}

void free2DMatrix(double **matrix, int N) {
	for(int i = 0; i < N; i++)
		free(matrix[i]);
	free(matrix);
}

void fillMatrixZeros(double **matrix, int N) {
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++) 
			matrix[i][j] = 0.0;
}

void fillMatrixRandoms(double **matrix, int N) {
	for(int i = 0; i < N; i++) 
		for(int j = 0; j < N; j++)
			matrix[i][j] = (double)rand() / RAND_MAX;
}

void naiveMatMul(double **A, double **B, double **C, int N) {
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
			for(int k = 0; k < N; k++)
				C[i][j] += A[i][k] * B[k][j];
}

void cachedMatMul(double **A, double **B, double **C, int N, int blockSize) {
	for(int i0 = 0; i0 < N; i0 += blockSize)
		for(int j0 = 0; j0 < N; j0 += blockSize)
			for(int k0 = 0; k0 < N; k0 += blockSize)
				for(int i = i0; i < MIN(i0 + blockSize, N); i++)
					for(int j = j0; j < MIN(j0 + blockSize, N); j++)
						for(int k = k0; k < MIN(k0 + blockSize, N); k++)
						       C[i][j] += A[i][k] * B[k][j];	
}

int checkMatrixEquality(double **A, double **B, int N) {
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
			if (fabs(A[i][j] - B[i][j]) < EPSILON) return 0;
	return 1;
}

int main(int argc, char **argv) {
	int N = 2048;
	int repetition = 1;
	if(argc >= 2) {
		N = atoi(argv[1]);
		if(N <= 0) {
			fprintf(stderr, "Invalid matrix size!\n");
			return 1;
		}
		if(argc == 3) {
			repetition = atoi(argv[2]);
			if(N <= 0) {
				fprintf(stderr, "Invalid repetition number!\n");
				return 1;
			}
		}
	}
	
	printf("Vectorization: %s\n", VECTYPE);	

	int blockSize = determineOptimalBlockSize(N);
	printf("Optimal block size: %d\n", blockSize);


	double **A = alloc2DMatrix(N);
	double **B = alloc2DMatrix(N);
	double **naiveResult = alloc2DMatrix(N);
	double **cachedResult = alloc2DMatrix(N);

	struct timespec start;
	struct timespec end;

        fillMatrixRandoms(A, N);
        fillMatrixRandoms(B, N);

	FILE *out = fopen("timings.csv", "a");
        if(!out) {
                fprintf(stderr, "Couldn't open file!\n");
                return 1;
        }

	int i;
	for(i = 0; i < repetition; i++) {
		fillMatrixZeros(naiveResult, N);
		fillMatrixZeros(cachedResult, N);

	        clock_gettime(CLOCK_MONOTONIC, &start);
		naiveMatMul(A, B, naiveResult, N);
		clock_gettime(CLOCK_MONOTONIC, &end);
		double naiveTime = timeDiff(&start, &end);
		printf("Time passed to complete naive matrix multiplication: %.6f seconds\n", naiveTime);
		fprintf(out, "%s,%d,%s,%.6f\n", VECTYPE, N, "naive", naiveTime);

		clock_gettime(CLOCK_MONOTONIC, &start);
	        cachedMatMul(A, B, cachedResult, N, blockSize);
		clock_gettime(CLOCK_MONOTONIC, &end);
		double cachedTime = timeDiff(&start, &end);
		printf("Time passed to complete cache-aware (tiling) matrix multiplication: %.6f seconds\n", cachedTime);
		fprintf(out, "%s,%d,%s,%.6f\n", VECTYPE, N, "cached", cachedTime);
	}

	fclose(out);

	free2DMatrix(A, N);
	free2DMatrix(B, N);
	free2DMatrix(naiveResult, N);
	free2DMatrix(cachedResult, N);

	return 0;
}
