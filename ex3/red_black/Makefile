

all: utils.c utils.h mpi_red_black.c
	mpicc utils.h utils.c  mpi_red_black.c  -std=c99 -lm -O2 -o mpi_Red_Black
	mpicc utils.h utils.c  mpi_red_black.c  -std=c99 -lm -O2 -DTEST_CONV -o mpi_Red_Black_conv



