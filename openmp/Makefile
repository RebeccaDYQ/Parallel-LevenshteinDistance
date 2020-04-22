CC=gcc
OMP=-fopenmp -DOMP
CFLAGS=-g -O3 -Wall
LDFLAGS= -lm

CFILES = distance_omp.c

all: distance-omp

distance-omp: $(CFILES) 
	$(CC) $(CFLAGS) $(OMP) -o distance-omp $(CFILES) $(LDFLAGS)