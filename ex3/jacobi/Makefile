

all: utils.c utils.h mpi_jacobi_final.c
	mpicc utils.h utils.c  mpi_jacobi_final.c  -std=c99 -lm -O3 -o mpi_Jacobi
	mpicc utils.h utils.c  mpi_jacobi_final.c  -std=c99 -lm -O3 -DTEST_CONV -o mpi_Jacobi_conv



