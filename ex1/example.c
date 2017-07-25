#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
int main(int argc, char **argv)
{
 	char name[256];
 	int size;
 int myrank;
 int N=1024;
 MPI_Init(&argc,&argv);
 MPI_Comm_size(MPI_COMM_WORLD, &size);
 MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
 gethostname(name, sizeof(name));
 N=atoi(argv[1]);
 fprintf(stdout,"%s: rank=%d, size=%d, N=%d \n",name, myrank,size,N);
 MPI_Finalize();
 return 1;
}
