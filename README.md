# 15618-project

## running sequential C version : simple.c
gcc simple.c -o simple
./simple

## ISPC Implementation

### Diagonal order

- Sequentially process each diagonal (start from top left to bottom right). In each diagonal, use an ISPC kernel (foreach) to compute each table entry in parallel.

- [Diagonal index and length representation] (https://math.stackexchange.com/questions/2206763/length-of-matrix-diagonals): Main diagonal (the one goes from botton left to top right) is indexed 0. All diagonals above it have negative indices (start from the top left one). All diagonals below it have positive indices. Thus, for a m*n matrix, length of diagonal[k] = min(m, n, m+k, n-k).

- Input strings are converted to int arrays, since ISPC does not support char datatype. 

- As required, DP table is uniform across all instances, and it cannot be a varying pointer. Instead, use a [MAXLEN][MAXLEN] 2D int array.

Compile commands: (Generated files are in objs/. Add ispc path in ~/.bashrc.)

ispc -O2 --target=avx LD_ispc.c -o objs/LD_ispc.o -h objs/LD_ispc.h

g++ -m64 main.c -Iobjs/ -O3 -Wall -c -o objs/main.o

g++ -m64 -Iobjs/ -O3 -Wall -o LD objs/main.o objs/LD_ispc.o -lm

Todos:
- Dynamic tasks.
