# 15618-project

## sequential C version : simple.c
gcc simple.c -o simple
./simple

## ISPC Implementation

### Diagonal order

- Sequentially process each diagonal (start from top left to bottom right). In each diagonal, use an ISPC kernel (foreach) to compute each table entry in parallel.

- [Diagonal index and length representation](https://math.stackexchange.com/questions/2206763/length-of-matrix-diagonals): Main diagonal (the one goes from botton left to top right) is indexed 0. All diagonals above it have negative indices (start from the top left one). All diagonals below it have positive indices. Thus, for a m*n matrix, length of diagonal[k] = min(m, n, m+k, n-k).

- Input strings are converted to int arrays, since ISPC does not support char datatype. 

- As required, DP table is uniform across all instances, and it cannot be a varying pointer. Instead, use a [MAXLEN][MAXLEN] 2D int array.

Update: 
- Store DP table in diagonal order, and unrolled to 1-D array. This change requires complicated index calculation and precomputation of diagonal lengths and start points.

- First initialize the table in parallel (does not make much difference). Then precompute comparison results and fill in DP table in parallel (makes a big difference!). 

#### Compile commands: (Generated files are in objs/. Add ispc path in ~/.bashrc. Also support tasking model.)
Update: Add a compiler option to disable gather coalescing (which will cause segmentation fault).

ispc -O2 --target=avx LD_ispc.c -o objs/LD_ispc.o -h objs/LD_ispc.h --opt=disable-coalescing

g++ -m64 main.c -Iobjs/ -O3 -Wall -c -o objs/main.o

g++ -m64 -Iobjs/ -O3 -Wall -o LD objs/main.o objs/LD_ispc.o ob  objs/tasksys.o ../sequential/LD_seq.o -lm -lpthread

### Row Order
Process 8 rows (or fewer at the end) at a time. Start with a diagonal line of length 8 or shorter, and move it horizontally to the end. Compute the entries on the diagonal using an ISPC kernel. 

Problems: Non-sequential memory accesses; not fully using 8 ISPC units at the start and end positions.

### ISPC task
Arbitrarily set a `BLOCK_LEN`. Each task is responsible for computing `BLOCK_LEN` entries.

## OpenMP Implementation
### Row Order
compile commands:  
make  
./distance-omp  

Current performance: Achieve some speedup if the table has large #col and small #row.  

## MPI Implementation
### Row Order  
compile commands:
make  
mpirun -np 8 distance-mpi  

