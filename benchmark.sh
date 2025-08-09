#!/bin/bash

Ns=(128 256 512 1024 2048)

make clean

if ! grep -q "VECTORIZE,N,type,time" timings.csv 2>/dev/null; then
    echo "VECTORIZE,N,type,time" > timings.csv
fi

echo "==== Testing different matrix sizes with VEC=disabled ===="
for N in "${Ns[@]}"; do
    echo "-> Building with VEC=disabled"
    make clean
    make VEC=disabled
    echo "-> Running N=$N (VEC=disabled)"
    ./matrixmul $N 5
done

echo ""
echo "==== Testing different matrix sizes with VEC=enabled ===="
for N in "${Ns[@]}"; do
    echo "-> Building with VEC=enabled"
    make clean
    make VEC=enabled
    echo "-> Running N=$N (VEC=enabled)"
    ./matrixmul $N 5
done

echo ""
echo "==== All tests complete. Timings written to timings.csv ===="

