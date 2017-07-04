#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>

int main(int argc, char *argv[]) {
	int npes, myrank;
        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &npes);
        MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	
	int i,j, d=4;
        int *A, *x;
	x = (int*)malloc(d*sizeof(int));
	if(myrank==0) {
		A = (int*)malloc((d*d)*sizeof(int));
		//matrix input
		for (i=0;i<(d*d);i++) 
			A[i] = i; 
		//vector input
		for (i=0;i<(d);i++) 
			x[i] = i; 
	}
	if (npes > d) { //error printing
		if(myrank==0)
			printf("Error: Number of processors are greater than matrix dimension\n");
		MPI_Finalize();
		return 1;
	}
	//data distribution in processors
	int *procA;
	int scatter_countA = ((d/npes)*d);
	if (myrank==(npes-1)) 
		scatter_countA = ((d/npes)*d) + ((d%npes)*d);
	procA = (int*)malloc( scatter_countA * sizeof(int) );
	int *scA, *displA ;
	if (myrank==0) {
		scA = (int*)malloc( npes*sizeof(int) );
		displA = (int*)malloc( npes*sizeof(int) );
	}
	MPI_Gather(&scatter_countA,1,MPI_INT,scA,1,MPI_INT,0,MPI_COMM_WORLD);
	 if (myrank==0) {
                displA[0]=0;
                for (i=1; i<npes; i++)
                	displA[i]=displA[i-1]+scA[i-1];
        }
	MPI_Scatterv(A, scA, displA, MPI_INT, procA, scatter_countA, MPI_INT ,0, MPI_COMM_WORLD);
	MPI_Bcast(x, d , MPI_INT , 0, MPI_COMM_WORLD );
	
	//computations for matrix-vector multiplication
	int scatter_countY = (d/npes);
	if (myrank==(npes-1)) 
		scatter_countY = ((d/npes)) + ((d%npes));
	int *procY;
        procY = (int*)malloc( scatter_countY * sizeof(int) );
	for (i=0;i<scatter_countY;i++) 
		procY[i] = 0; 

	for (i=0; i < scatter_countY; i++)
		for (j=0; j < d; j++ )
			procY[i] = procY[i] + procA[i*d+j]*x[j];
	
	//Gathering answer
	int *y;
	y = (int*)malloc( d * sizeof(int) );
	int *scY, *displY ;
	scY = (int*)malloc( npes*sizeof(int) );
	displY = (int*)malloc( npes*sizeof(int) );
	MPI_Allgather(&scatter_countY,1,MPI_INT,scY,1,MPI_INT,MPI_COMM_WORLD);
        displY[0]=0;
        for (i=1; i<npes; i++)
               	displY[i]=displY[i-1]+scY[i-1];
	
	MPI_Allgatherv(procY,scatter_countY, MPI_INT, y, scY, displY , MPI_INT, MPI_COMM_WORLD);

	for (i=0;i<d;i++) 
		printf("%d ",y[i]);
	printf("=> rank: %d\n",myrank);

	
	MPI_Finalize();
	return 0;
}
