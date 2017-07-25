/*
 Standard implementation of the Floyd-Warshall Algorithm
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "util.h"
#include <tbb/task_scheduler_init.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>

inline int min(int a, int b);



int main(int argc, char **argv)
{

     int **A;
     int i,j,k;
     struct timeval t1, t2;
     double time;
     int N=1024;
     int t=1;
     
     
     if (argc != 3) {
        fprintf(stdout,"Usage: %s N\n", argv[0]);
        exit(0);
     }

     N=atoi(argv[1]);
     t=atoi(argv[2]);
     tbb::task_scheduler_init init(t);     
     A = (int **) malloc(N*sizeof(int *));
     for(i=0; i<N; i++) A[i] = (int *) malloc(N*sizeof(int));

     graph_init_random(A,-1,N,128*N);

     tbb::simple_partitioner ap;
     gettimeofday(&t1,0);
     for(k=0;k<N;k++)
       tbb::parallel_for(
			   tbb::blocked_range<int>(0,N), [=](const tbb::blocked_range<int>& r)
			   {
			     for(int i=r.begin(); i<r.end(); i++)
			       for(int j=0; j<N; j++)
				 A[i][j]=min(A[i][j], A[i][k] + A[k][j]);
			   }, ap
			 );

     gettimeofday(&t2,0);

     time=(double)((t2.tv_sec-t1.tv_sec)*1000000+t2.tv_usec-t1.tv_usec)/1000000;
     printf("FW,%d,%.4f\n", N, time);

     /*
     for(i=0; i<N; i++)
        for(j=0; j<N; j++) fprintf(stdout,"%d\n", A[i][j]);
     */

     return 0;     
}

inline int min(int a, int b)
{
     if(a<=b)return a;
     else return b;
}

