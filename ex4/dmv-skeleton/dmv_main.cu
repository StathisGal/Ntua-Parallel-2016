/*
 *  dmv_main.cu -- DMV front-end program.
 *
 *  Copyright (C) 2010-2012, Computing Systems Laboratory (CSLab)
 *  Copyright (C) 2010-2012, Vasileios Karakasis
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <cuda.h>
#include "alloc.h"
#include "dmv.h"
#include "error.h"
#include "gpu_util.h"
#include "timer.h"
#include "cublas_v2.h"

#ifndef VALUES_MAX
#   define VALUES_MAX MAKE_VALUE_CONSTANT(1.)
#endif

#ifndef EPS
#   define EPS MAKE_VALUE_CONSTANT(1.e-6)
#endif

#ifndef NR_ITER
#   define NR_ITER 1
/* #   define NR_ITER 100 */
#endif

static void check_result(const value_t *test, const value_t *orig, size_t n)
{
    printf("Checking ... ");
    size_t  i_fail = vec_equals(test, orig, n, EPS);
    if (!i_fail) {
        printf("PASSED\n");
    } else {
        printf("FAILED (index: %ld)\n", i_fail - 1);
        printf("%" VALUE_FORMAT " != " "%" VALUE_FORMAT "\n",
               test[i_fail-1], orig[i_fail-1]);
    }
}

static void report_results(xtimer_t *timer, size_t n)
{
    double  elapsed_time = timer_elapsed_time(timer);
    size_t  flops        = 2*n*n*NR_ITER;

    printf("Elapsed time: %lf s\n", elapsed_time);
    printf("Performance:  %lf Gflop/s\n", flops*1.e-9 / elapsed_time);
}

static void print_usage()
{
    printf("Usage: [GPU_KERNEL=<kernel_no>] [GPU_BLOCK_SIZE=<size>] "
           "%s <matrix size>\n", program_name);
    printf("GPU_KERNEL defaults to 0\n");
    printf("GPU_BLOCK_SIZE defaults to 256\n");
    printf("Available kernels [id:name]:\n");
    size_t i;
    for (i = 0; i < GPU_KERNEL_END; ++i) {
        printf("\t%zd:%s\n", i, gpu_kernels[i].name);
    }
}

int main(int argc, char **argv)
{
    set_program_name(argv[0]);
    if (argc < 2) {
        warning(0, "too few arguments");
        print_usage();
        exit(EXIT_FAILURE);
    }

    size_t n = atoi(argv[1]);
    if (!n)
        error(0, "invalid argument: %s", argv[1]);

    /* Read block size and kernel to launch from the environment */
    const char *env_gpu_kernel = getenv("GPU_KERNEL");
    const char *env_gpu_block_size = getenv("GPU_BLOCK_SIZE");
    const char *env_gpu_block_width = getenv("GPU_BLOCK_WIDTH");
    int kernel = (env_gpu_kernel) ? atoi(env_gpu_kernel) : GPU_NAIVE;
    int block_size = (env_gpu_block_size) ? atoi(env_gpu_block_size) : 256;
    int block_wid = (env_gpu_block_width) ? atoi(env_gpu_block_width) : 32;
    size_t orig_n = n;  // original matrix size
    int grid_size = 1;  // FILLME: compute the grid size
	float alpha, beta;
	cublasHandle_t handle;
    printf("kernel:%d", kernel);
    /*
     *  FILLME: you can optionally adjust appropriately (increase
     *          only) the matrix size here if that helps you with your
     *          kernel code, e.g., to avoid divergent warps.
     */ 

    printf("Matrix size: %zd\n", orig_n);
    printf("Adjusted matrix size: %zd\n", n);

    /*
     * Allocate the structures.
     * 
     * Initialization to zero is crucial if you adjusted the matrix
     * size.
     */
    value_t **A = (value_t **) calloc_2d(n, n, sizeof(**A));
    if (!A)
        error(1, "alloc_2d failed");
    value_t **A_t;
    if (kernel != 0)
      {
	A_t=  (value_t **) calloc_2d(n, n, sizeof(**A_t));
      }

    value_t *x = (value_t *) calloc(n, sizeof(*x));
    if (!x)
        error(1, "malloc failed");

    value_t *y_serial = (value_t *) calloc(n, sizeof(*y_serial));
    if (!y_serial)
        error(1, "malloc failed");
    
    value_t *y = (value_t *) calloc(n, sizeof(*y));
    if (!y)
        error(1, "malloc failed");

    /* Initialize */
    srand48(0);
    mat_init_rand(A, orig_n, VALUES_MAX);
    vec_init_rand(x, orig_n, VALUES_MAX);
    vec_init(y_serial, orig_n, MAKE_VALUE_CONSTANT(0.0));
    vec_init(y, orig_n, MAKE_VALUE_CONSTANT(0.0));

    if(kernel != 0)
      {
	for(int i=0; i<n; i++)
	  for(int j=0; j<n; j++)
	    A_t[i][j]=A[j][i];
      }	

    /* Setup timers */
    xtimer_t timer;

    /* Compute serial */
#ifdef SERIAL_KERNEL
    printf(">>>> Begin of record <<<<\n");
    printf("Serial version:\n");
    timer_clear(&timer);
    timer_start(&timer);
    for (size_t i = 0; i < NR_ITER; ++i)
        dmv_serial(A, x, y_serial, orig_n);
    timer_stop(&timer);
    report_results(&timer, orig_n);
    printf(">>>> End of record <<<<\n");
#endif  // SERIAL_KERNEL

#ifdef OPENMP_KERNEL
    /* Compute OpenMP */
    printf(">>>> Begin of record <<<<\n");
    printf("OpenMP version:\n");
    timer_clear(&timer);
    timer_start(&timer);
    for (size_t i = 0; i < NR_ITER; ++i)
        dmv_omp(A, x, y, orig_n);
    timer_stop(&timer);
#ifndef _NOCHECK_
    check_result(y, y_serial, orig_n);
#endif
    report_results(&timer, orig_n);
    printf(">>>> End of record <<<<\n");
#endif  // OPENMP_KERNEL

#ifdef GPU_KERNEL
    /*
     *  FILLME: Set up the blocks, grid and shared memory depending on
     *          the kernel. Make any transformations to the input
     *          matrix here.
     */


    int second_arg;

    if(kernel==2)
      {
	second_arg=(((n-1)/block_wid)+1);
      }
	else if(kernel==3) {
		alpha = 1;
		beta = 0;
		cublasCreate(&handle);
	}
    else
      second_arg=1;

    dim3 gpu_block(block_size, 1);   // FILLME: set up the block dimensions    
    dim3 gpu_grid((((n-1)/block_size)+1), second_arg);    // FILLME: set up the grid dimensions
    size_t shmem_size = (block_wid+3)*sizeof(value_t);  // FILLME: set up the shared memory size

    printf(">>>> Begin of record <<<<\n");
    printf("Block size: %dx%d\n", gpu_block.x, gpu_block.y);
    printf("Grid size : %dx%d\n", gpu_grid.x, gpu_grid.y);
    printf("Shared memory size: %ld bytes\n", shmem_size);

    /* GPU allocations */
    value_t *gpu_A = (value_t *) gpu_alloc(n*n*sizeof(*gpu_A));

    if (!gpu_A)
        error(0, "gpu_alloc failed: %s", gpu_get_last_errmsg());
    
    value_t *gpu_x = (value_t *) gpu_alloc(n*sizeof(*gpu_x));
    if (!gpu_x)
        error(0, "gpu_alloc failed: %s", gpu_get_last_errmsg());

    value_t *gpu_y = (value_t *) gpu_alloc(n*sizeof(*gpu_y));
    if (!gpu_y)
        error(0, "gpu_alloc failed: %s", gpu_get_last_errmsg());

    /* for(int i=0; i<n; i++) */
    /*   { */
    /* 	for(int j=0; j<n; j++) */
    /* 	  printf("%f ",A[i][j]); */
    /* 	printf("\n"); */
    /*   } */
    /* printf("x:"); */
    /* for(int i=0; i<n; i++) */
    /*   printf("%f ",x[i]); */
    /* printf("\n"); */
    /* /\* Copy data to GPU *\/ */
    if(kernel != 0 && kernel != 3)
      {
    	if (copy_to_gpu(A_t[0], gpu_A, n*n*sizeof(*gpu_A)) < 0)
    	  error(0, "copy_to_gpu failed: %s", gpu_get_last_errmsg());
	printf("transposed");
      }
    else
      {
	if (copy_to_gpu(A[0], gpu_A, n*n*sizeof(*gpu_A)) < 0)
	  error(0, "copy_to_gpu failed: %s", gpu_get_last_errmsg());
      }

    if (copy_to_gpu(x, gpu_x, n*sizeof(*gpu_x)) < 0)
        error(0, "copy_to_gpu failed: %s", gpu_get_last_errmsg());

    /* Reset y and copy it to GPU */
    vec_init(y, n, MAKE_VALUE_CONSTANT(0.0));
    if (copy_to_gpu(y, gpu_y, n*sizeof(*gpu_y)) < 0)
        error(0, "copy_to_gpu failed: %s", gpu_get_last_errmsg());

    if (kernel >= GPU_KERNEL_END)
        error(0, "the requested kernel does not exist");

    printf("GPU kernel version: %s\n", gpu_kernels[kernel].name);

    /* Execute and time the kernel */
    timer_clear(&timer);
    timer_start(&timer);
    for (size_t i = 0; i < NR_ITER; ++i) {
      if (copy_to_gpu(y, gpu_y, n*sizeof(*gpu_y)) < 0)
        error(0, "copy_to_gpu failed: %s", gpu_get_last_errmsg());

	if(kernel==3) cublasSgemv(handle, CUBLAS_OP_T, n, n, &alpha, gpu_A, n, gpu_x, 1, &beta, gpu_y, 1);
	else gpu_kernels[kernel].fn<<<gpu_grid,gpu_block,shmem_size>>>(gpu_A, gpu_x, gpu_y, n);

#ifdef _DEBUG_
        cudaError_t err;
        if ( (err = cudaGetLastError()) != cudaSuccess)
            error(0, "gpu kernel failed to launch: %s", gpu_get_errmsg(err));
#endif
        cudaThreadSynchronize();
    }
    timer_stop(&timer);

    /* Copy result back to host and check */
    if (copy_from_gpu(y, gpu_y, n*sizeof(*y)) < 0)
        error(0, "copy_from_gpu failed: %s", gpu_get_last_errmsg());

#ifndef _NOCHECK_

    /* for(int i=0; i<orig_n; i++) */
    /*   printf("[%d] y:%f,y_serial:%f \n",i,y[i],y_serial[i]); */
    /* printf("y:\n"); */
    /* for(int i=0; i<n; i++) */
    /*   { */
    /* 	  printf("%f ",y[i]); */
    /* 	printf("\n"); */
    /*   } */
    /* printf("y_serial:\n"); */
    /* for(int i=0; i<n; i++) */
    /*   { */
    /* 	  printf("%f ",y_serial[i]); */
    /* 	printf("\n"); */
    /*   } */
    check_result(y, y_serial, orig_n);						
#endif
    report_results(&timer, orig_n);
    printf(">>>> End of record <<<<\n");
#endif  // GPU_KERNEL

    /* Free resources on host */
    free_2d((void **) A);
    free(x);
    free(y);
    free(y_serial);

#ifdef GPU_KERNEL
    /* Free resources on GPU */
    gpu_free(gpu_A);
    gpu_free(gpu_x);
    gpu_free(gpu_y);
#endif  // GPU_KERNEL

    return EXIT_SUCCESS;
}
