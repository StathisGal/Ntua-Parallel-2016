/*
 *  dmv_gpu.cu -- Template for DMV GPU kernels
 *
 *  Copyright (C) 2010-2013, Computing Systems Laboratory (CSLab)
 *  Copyright (C) 2010-2013, Vasileios Karakasis
 */ 
#include <stdio.h>
#include "dmv.h"


/*
 *  Utility function to get the thread ID within the
 *  global working space.
 */ 
__device__ int get_global_tid()
{
    return (gridDim.x*blockIdx.y + blockIdx.x)*blockDim.x*blockDim.y +
        blockDim.x*threadIdx.y + threadIdx.x;
}

/*
 *  Utility function to get the thread ID within the
 *  local/block working space.
 */ 
__device__ int get_local_tid()
{
    return blockDim.x*threadIdx.y + threadIdx.x;
}

/*
 *  Naive kernel
 */ 
__global__ void dmv_gpu_naive (const value_t *a, const value_t *x, value_t *y,
                              size_t n)
{
    /*
     * FILLME: fill the code for the naive kernel.
     */ 
	int j = threadIdx.x + blockIdx.x * blockDim.x;
	for (int i=0; i<n; i++)
	  {
	    if(j<n)
		y[j] += a[i+j*n]*x[i];
	  }
}

/*
 *  Coalesced memory acceses
 */
__global__ void dmv_gpu_coalesced(const value_t *a, const value_t *x,
                                  value_t *y, size_t n)
{
  /*
   * FILLME: fill the code for the coalesced kernel.
   */
  int j = threadIdx.x + blockIdx.x * blockDim.x;
  float tmp=0;
  for (int i=0; i<n*n; i+=n)
    {
      /* if(j==0) */
      /* printf("i:%d j:%d i/n:%d a:%f x:%f y:%f",i,j,i/n,a[i+j],x[i/n],y[j]); */
      if(j<n) tmp += a[i+j]*x[i/n];
    } 
  y[j] = tmp;
}

/*
 *  Use of shared memory
 */
__global__ void dmv_gpu_shmem(const value_t *a, const value_t *x, value_t *y,
                              size_t n)
{

  int block_width, block_height; 
  if (  n % gridDim.x)
    block_height = (n / gridDim.x) + 1;
    else
      block_height = (n / gridDim.x);
  if ( n % gridDim.y)
    block_width = (n / gridDim.y) + 1;
  else
    block_width = (n / gridDim.y);
  
  extern __shared__ value_t x_sh[];
  
  __shared__ int *blockElt,*block_x_Ind,*block_y_Ind;

  blockElt=(int*)&x_sh[block_width];
  block_x_Ind=(int*)&x_sh[block_width+1];
  block_y_Ind=(int*)&x_sh[block_width+2];
  
  // to y leei tin grammi kai to x tin stili
  if(threadIdx.x == 0)
    {
      /* printf("w:%d h:%d\n",block_width,block_height); */
      if((blockIdx.y+1)* block_width > n)
	{
	  /* printf("mpikeee:%d",n % block_width); */
	  *blockElt = (n % block_width);
	}
      else
	*blockElt =block_width;

      *block_x_Ind = blockIdx.x * block_height;
      *block_y_Ind = blockIdx.y * block_width;
    }
  __syncthreads();
  
  //I think it needs sync

  int thread_x_Ind =blockIdx.x * block_height + threadIdx.x;
    

  /* if (thread_x_Ind < n) */
  if(threadIdx.x <*blockElt)
    x_sh[threadIdx.x] = x[*block_y_Ind + threadIdx.x];
  /* if(threadIdx.x == 0 ) */
  /*   for(int i=0; i<*blockElt; i++) */
  /*     printf("edw exoume %d a:%f x:%f\n",*blockElt,a[(*block_y_Ind + i) * n  + *block_x_Ind + threadIdx.x],x_sh[i]); */
  __syncthreads();

  value_t cSum = 0;

  /* if(threadIdx.x == 1) */
  /*   { */
  /* /\*     printf("blockElt:%d, thread_y_Ind:%d, block_x_Ind:%d, block_y_Ind:%d\n",*blockElt,thread_y_Ind,*block_x_Ind,*block_y_Ind); *\/ */
  /*   } */
  /* printf("thread:%d blockElt:%d blockx:%d, blocky:%d\n",threadIdx.x,*blockElt,blockIdx.x,blockIdx.y); */
  if(thread_x_Ind < n     )
    {
      for(int i=0; i<*blockElt; i++)
  	{
	  /* if(threadIdx.x == 0 && *block_x_Ind == 0 && *block_y_Ind == 0) */
	  /*   printf("a:%f x:%f new:%f cSum:%f\n",a[(*block_y_Ind + i) * n  + *block_x_Ind + threadIdx.x],x_sh[i], x_sh[i]*a[(*block_y_Ind + i) * n  + *block_x_Ind + threadIdx.x],cSum); */
  	  cSum += x_sh[i]*a[(*block_y_Ind + i) * n  + *block_x_Ind + threadIdx.x];
  	}
	  /* if(threadIdx.x == 0 && *block_x_Ind == 0 && *block_y_Ind == 0) */
	    /* printf("sum is: %f\n", cSum);       */
      atomicAdd((float*)&y[thread_x_Ind],(float)cSum);
    }

}
