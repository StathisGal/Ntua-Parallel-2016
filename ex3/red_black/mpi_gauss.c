#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include "mpi.h"
#include "utils.h"

int main(int argc, char ** argv) {
    int rank,size;
    int global[2],local[2]; //global matrix dimensions and local matrix dimensions (2D-domain, 2D-subdomain)
    int global_padded[2];   //padded global matrix dimensions (if padding is not needed, global_padded=global)
    int grid[2];            //processor grid dimensions
    int i,j,t;
    int global_converged=0,converged=0; //flags for convergence, global and per process
    MPI_Datatype dummy;     //dummy datatype used to align user-defined datatypes in memory
    double omega; 			//relaxation factor - useless for Jacobi

    struct timeval tts,ttf,tcs,tcf,teps,tepf,teps_1,teps_2,tepf_1,tepf_2;   //Timers: total-tts,ttf, computation-tcs,tcf
    double ttotal=0,tcomp=0,tepic=0,total_time,comp_time,epik_time;
    
    double ** U, ** u_current, ** u_previous, ** swap; //Global matrix, local current and previous matrices, pointer to swap between current and previous
    

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    //----Read 2D-domain dimensions and process grid dimensions from stdin----//

    if (argc!=5) {
        fprintf(stderr,"Usage: mpirun .... ./exec X Y Px Py");
        exit(-1);
    }
    else {
        global[0]=atoi(argv[1]);
        global[1]=atoi(argv[2]);
        grid[0]=atoi(argv[3]);
        grid[1]=atoi(argv[4]);
    }

    //----Create 2D-cartesian communicator----//
	//----Usage of the cartesian communicator is optional----//

    MPI_Comm CART_COMM;         //CART_COMM: the new 2D-cartesian communicator
    int periods[2]={0,0};       //periods={0,0}: the 2D-grid is non-periodic
    int rank_grid[2];           //rank_grid: the position of each process on the new communicator
		
    MPI_Cart_create(MPI_COMM_WORLD,2,grid,periods,0,&CART_COMM);    //communicator creation
    MPI_Cart_coords(CART_COMM,rank,2,rank_grid);	                //rank mapping on the new communicator

    //----Compute local 2D-subdomain dimensions----//
    //----Test if the 2D-domain can be equally distributed to all processes----//
    //----If not, pad 2D-domain----//
    
    for (i=0;i<2;i++) {
        if (global[i]%grid[i]==0) {
            local[i]=global[i]/grid[i];
            global_padded[i]=global[i];
        }
        else {
            local[i]=(global[i]/grid[i])+1;
            global_padded[i]=local[i]*grid[i];
        }
    }

	//Initialization of omega
    omega=2.0/(1+sin(3.14/global[0]));

    //----Allocate global 2D-domain and initialize boundary values----//
    //----Rank 0 holds the global 2D-domain----//
    if (rank==0) {
        U=allocate2d(global_padded[0],global_padded[1]);   
        init2d(U,global[0],global[1]);
	/* for (int i=0; i<global_padded[0]; i++) */
	/*   { */
	/*     for(int j=0; j<global_padded[1]; j++) */
	/*       printf("%f ",U[i][j]); */
	/*     printf("\n"); */
	/*   } */
    }

    //----Allocate local 2D-subdomains u_current, u_previous----//
    //----Add a row/column on each size for ghost cells----//

    u_previous=allocate2d(local[0]+2,local[1]+2);
    u_current=allocate2d(local[0]+2,local[1]+2);   
       
    //----Distribute global 2D-domain from rank 0 to all processes----//
         
 	//----Appropriate datatypes are defined here----//
	/*****The usage of datatypes is optional*****/
    
    //----Datatype definition for the 2D-subdomain on the global matrix----//

    MPI_Datatype global_block;
    MPI_Type_vector(local[0],local[1],global_padded[1],MPI_DOUBLE,&dummy);
    MPI_Type_create_resized(dummy,0,sizeof(double),&global_block);
    MPI_Type_commit(&global_block);

    //----Datatype definition for the 2D-subdomain on the local matrix----//

    MPI_Datatype local_block;
    MPI_Type_vector(local[0],local[1],local[1]+2,MPI_DOUBLE,&dummy);
    MPI_Type_create_resized(dummy,0,sizeof(double),&local_block);
    MPI_Type_commit(&local_block);

    //----Rank 0 defines positions and counts of local blocks (2D-subdomains) on global matrix----//

    double * buffer = NULL;
    int * scatteroffset, * scattercounts;
    if (rank==0) {
      buffer = &U[0][0];
      scatteroffset=(int*)malloc(size*sizeof(int));
      scattercounts=(int*)malloc(size*sizeof(int));
      for (i=0;i<grid[0];i++)
	for (j=0;j<grid[1];j++) {
	  scattercounts[i*grid[1]+j]=1;
	  scatteroffset[i*grid[1]+j]=(local[0]*local[1]*grid[1]*i+local[1]*j);
	}
    }

    MPI_Scatterv(buffer,scattercounts,scatteroffset,global_block,&(u_current[1][1]),local[0]*local[1],local_block,0,MPI_COMM_WORLD);
    /* for (int p=0; p<size; p++) { */
    /*   if (rank == p) { */
    /* 	printf("Local process on rank %d is:\n", rank); */
    /* 	for (i=0; i<local[0]+2; i++) { */
    /* 	  putchar('|'); */
    /* 	  for (j=0; j<local[1]+2; j++) { */
    /* 	    printf("%f ", u_current[i][j]); */
    /* 	  } */
    /* 	  printf("|\n"); */
    /* 	} */
    /*   } */
    /*   MPI_Barrier(MPI_COMM_WORLD); */
    /* } */

     //************************************//


    if (rank==0)
        free2d(U,global_padded[0],global_padded[1]);

 
     
	//----Define datatypes or allocate buffers for message passing----//

	//*************TODO*******************//


    MPI_Datatype pass_column_block;
    MPI_Type_vector(local[0],1,local[1]+2,MPI_DOUBLE,&dummy);
    MPI_Type_create_resized(dummy,0,sizeof(double),&pass_column_block);
    MPI_Type_commit(&pass_column_block);

    MPI_Datatype pass_row_block;
    MPI_Type_vector(local[1],1,1,MPI_DOUBLE,&dummy);
    MPI_Type_create_resized(dummy,0,sizeof(double),&pass_row_block);
    MPI_Type_commit(&pass_row_block);


	//************************************//


    //----Find the 4 neighbors with which a process exchanges messages----//


    int north=MPI_PROC_NULL, south=MPI_PROC_NULL, east=MPI_PROC_NULL, west=MPI_PROC_NULL;

    MPI_Cart_shift(CART_COMM, 0, 1, &north, &south);
    MPI_Cart_shift(CART_COMM, 1, 1, &west, &east);

    /* printf("rank= %d coords= %d %d  neighbors(u,d,l,r)= %d %d %d %d\n", */
    /* 	   rank,rank_grid[0],rank_grid[1],north,south,west,east); */
	//*************TODO*******************//

    //---Define the iteration ranges per process-----//
	//*************TODO*******************//

    int i_min,i_max,j_min,j_max;

    if(global[0]%grid[0]==0) // fits exactly in the boundary processes
      {
	if(north < 0) i_min = 2;
	else i_min = 1;
	if(south < 0) i_max = local[0];
	else i_max = local[0] +1;
      }
    else
      {
	if(north < 0) i_min = 2;
	else i_min = 1;
	if(south < 0) i_max = global[0]%grid[0]+1;
	else i_max = local[0] +1;

      }
    if(global[1]%grid[1]==0) // fits exactly in the boundary processes
      {
	if(west < 0) j_min = 2;
	else j_min = 1;
	if(east < 0) j_max = local[1];
	else j_max = local[1] + 1;
      }
    else
      {
	if(west < 0) j_min = 2;
	else j_min = 1;
	if(east < 0) j_max = global[1]%grid[1]+1;
	else j_max = local[1] + 1;

      }


	/*Three types of ranges:
		-internal processes
		-boundary processes
		-boundary processes and padded global array
	*/



    for(int i=0; i<local[0]+2; i++)
      for(int j=0; j<local[1]+2; j++)
	u_previous[i][j]=u_current[i][j];

    

    // we want to wait to get from north and west
    // calculate and finally send to south and east
    MPI_Status stat,stat1,stat2,stat3,stat4;
    MPI_Request right_receive,left_receive,north_receive,south_receive;

        /* 	//----Computational core----//    */
    gettimeofday(&tts,NULL);

    #ifdef TEST_CONV
    for (t=0;t<T && !global_converged;t++) {
    #endif
    #ifndef TEST_CONV
    #undef T
    #define T 256
    for (t=0;t<T;t++) {
    #endif


      gettimeofday(&teps,NULL);
/*
    MPI_Sendrecv(&u_current[1][1],1,pass_row_block,north,1,&u_current[local[0]+1][1],1,pass_row_block,south,1, CART_COMM,&stat);
    MPI_Sendrecv(&u_current[1][1],1,pass_column_block,west,1,&u_current[1][local[1]+1],1,pass_column_block,east,1,CART_COMM,&stat);

    MPI_Barrier(MPI_COMM_WORLD);
    gettimeofday(&tepf_1,NULL);     
*/
    /* for (int p=0; p<size; p++) { */
    /*   if (rank == p) { */
    /* 	printf("Local process on rank %d is:\n", rank); */
    /* 	for (i=0; i<local[0]+2; i++) { */
    /* 	  putchar('|'); */
    /* 	  for (j=0; j<local[1]+2; j++) { */
    /* 	    printf("%f ", u_current[i][j]); */
    /* 	  } */
    /* 	  printf("|\n"); */
    /* 	} */
    /*   } */
    /*   MPI_Barrier(MPI_COMM_WORLD); */
    /* } */
    
    swap=u_previous;
    u_previous=u_current;
    u_current=swap; 
  
    //double *tmp_array;
    //tmp_array= (double *) malloc(sizeof(double)*local[0]);
    gettimeofday(&teps_1,NULL);

 //   MPI_Irecv(&u_current[1][0],1,pass_column_block,west,0,CART_COMM,&left_receive);
   // MPI_Irecv(&u_current[0][1],1,pass_row_block,north,0,CART_COMM,&north_receive);

    //MPI_Wait(&left_receive,&stat1);
    //MPI_Wait(&north_receive,&stat2);
    

      MPI_Sendrecv(&u_previous[1][1],1,pass_row_block,north,1,&u_previous[local[0]+1][1],1,pass_row_block,south,1, CART_COMM,&stat);
      MPI_Sendrecv(&u_previous[local[0]][1],1,pass_row_block,south,1,&u_previous[0][1],1,pass_row_block,north,1,CART_COMM,&stat);
      MPI_Sendrecv(&u_previous[1][local[1]],1,pass_column_block,east,1,&u_previous[1][0],1,pass_column_block,west,1,CART_COMM,&stat);
      MPI_Sendrecv(&u_previous[1][1],1,pass_column_block,west,1,&u_previous[1][local[1]+1],1,pass_column_block,east,1,CART_COMM,&stat);

//Red
	for (i=i_min;i<i_max;i++)
                for (j=j_min;j<j_max;j++)
                        if ((i+j)%2==0)
                                u_current[i][j]=u_previous[i][j]+(omega/4.0)*(u_previous[i-1][j]+u_previous[i+1][j]+u_previous[i][j-1]+u_previous[i][j+1]-4*u_previous[i][j]);

      MPI_Barrier(MPI_COMM_WORLD);

      MPI_Sendrecv(&u_current[1][1],1,pass_row_block,north,1,&u_current[local[0]+1][1],1,pass_row_block,south,1, CART_COMM,&stat);
      MPI_Sendrecv(&u_current[local[0]][1],1,pass_row_block,south,1,&u_current[0][1],1,pass_row_block,north,1,CART_COMM,&stat);
      MPI_Sendrecv(&u_current[1][local[1]],1,pass_column_block,east,1,&u_current[1][0],1,pass_column_block,west,1,CART_COMM,&stat);
      MPI_Sendrecv(&u_current[1][1],1,pass_column_block,west,1,&u_current[1][local[1]+1],1,pass_column_block,east,1,CART_COMM,&stat);
//Black

	for (i=i_min;i<i_max;i++)
        	for (j=j_min;j<j_max;j++)
                	if ((i+j)%2==1)
                        	u_current[i][j]=u_previous[i][j]+(omega/4.0)*(u_current[i-1][j]+u_current[i+1][j]+u_current[i][j-1]+u_current[i][j+1]-4*u_previous[i][j]);

      MPI_Barrier(MPI_COMM_WORLD);
	
    gettimeofday(&tepf_2,NULL);
    
    /* /\* 		/\\*Compute and Communicate*\\/ *\/ */

    /* /\* 		/\\*Add appropriate timers for computation*\\/ *\/ */
  /*  
    gettimeofday(&tcs,NULL);    	
    for (i=i_min;i<i_max;i++)
      for (j=j_min;j<j_max;j++)
		u_current[i][j]=u_previous[i][j]+(u_current[i-1][j]+u_previous[i+1][j]+u_current[i][j-1]+u_previous[i][j+1]-4*u_previous[i][j])*omega/4.0;
	

    gettimeofday(&tcf,NULL);
    gettimeofday(&teps_2,NULL);

    MPI_Isend(&u_current[local[0]][1],1,pass_row_block,south,0,CART_COMM,&south_receive);
    MPI_Isend(&u_current[1][local[1]],1,pass_column_block,east,0,CART_COMM,&right_receive);

    MPI_Wait(&south_receive,&stat3);
    MPI_Wait(&right_receive,&stat4);

    MPI_Barrier(MPI_COMM_WORLD);
    gettimeofday(&tepf,NULL);
*/

/* Printing for debuging */
    /* for (int p=0; p<size; p++) { */
    /*   if (rank == p) { */
    /* 	printf("Local process on rank %d is:\n", rank); */
    /* 	for (i=0; i<local[0]+2; i++) { */
    /* 	  putchar('|'); */
    /* 	  for (j=0; j<local[1]+2; j++) { */
    /* 	    printf("%f ", u_current[i][j]); */
    /* 	  } */
    /* 	  printf("|\n"); */
    /* 	} */
    /*   } */
    /*   MPI_Barrier(MPI_COMM_WORLD); */
    /* } */

/* Test Convergence */
#ifdef TEST_CONV
    if (t%C==0) {
      	converged = converge(u_previous,u_current,i_max,j_max);
    }
    MPI_Allreduce( &converged, &global_converged, 1,MPI_INT, MPI_LAND, MPI_COMM_WORLD);
#endif


/* Calculate Time */
     tcomp+= (tcf.tv_sec-tcs.tv_sec)+(tcf.tv_usec-tcs.tv_usec)*0.000001;
    tepic+= (tepf.tv_sec-teps_2.tv_sec)+(tepf.tv_usec-teps_2.tv_usec)*0.000001 +    (tepf_2.tv_sec-teps_1.tv_sec)+(tepf_2.tv_usec-teps_1.tv_usec)*0.000001 +    (tepf_1.tv_sec-teps.tv_sec)+(tepf_1.tv_usec-teps.tv_usec)*0.000001;    

         
        
    }
    gettimeofday(&ttf,NULL);

    ttotal=(ttf.tv_sec-tts.tv_sec)+(ttf.tv_usec-tts.tv_usec)*0.000001;

    MPI_Reduce(&ttotal,&total_time,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
    MPI_Reduce(&tcomp,&comp_time,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
    MPI_Reduce(&tepic,&epik_time,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);


    //----Rank 0 gathers local matrices back to the global matrix----//
   
    if (rank==0) {
            U=allocate2d(global_padded[0],global_padded[1]);
	    buffer=&U[0][0];
    }
    
    MPI_Gatherv(&u_current[1][1],1,local_block,buffer,scattercounts,scatteroffset,global_block,0,MPI_COMM_WORLD);
    

    /* if(rank==0) */
    /*   { */
    /* 	for(int i=0;i<global[0]; i++) */
    /* 	{ */
    /* 	  for(int j=0;j<global[1];j++) */
    /* 	    printf("[%d][%d]:%f ",i,j,U[i][j]); */
    /* 	  printf("\n"); */
    /* 	} */
    /*   } */

	
   

    	//----Printing results----//

    	//**************TODO: Change "Jacobi" to "GaussSeidelSOR" or "RedBlackSOR" for appropriate printing****************//
    if (rank==0) {
      printf("Jacobi X %d Y %d Px %d Py %d Iter %d ComputationTime %lf CommunicationTime %lf TotalTime %lf midpoint %lf\n",global[0],global[1],grid[0],grid[1],t,comp_time,epik_time,total_time,U[global[0]/2][global[1]/2]);
	
        #ifdef PRINT_RESULTS
        char * s=malloc(50*sizeof(char));
        sprintf(s,"resJacobiMPI_%dx%d_%dx%d",global[0],global[1],grid[0],grid[1]);
        fprint2d(s,U,global[0],global[1]);
        free(s);
        #endif

    }
    MPI_Finalize();
    return 0;
}
