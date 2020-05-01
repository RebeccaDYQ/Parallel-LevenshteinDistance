CC=gcc

MPI = -DMPI
MPICC = mpicc


DEBUG=0
INSTRUMENT=1
CFLAGS=-g -O3 -Wall -DDEBUG=$(DEBUG) -DTRACK=$(INSTRUMENT) -std=gnu99
LDFLAGS= -lm

CFILES = distance-mpi.c
all: distance-mpi

distance-mpi: $(CFILES)
	$(MPICC) $(CFLAGS) $(MPI) -o distance-mpi $(CFILES) $(LDFLAGS)