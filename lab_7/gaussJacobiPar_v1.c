#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<math.h>

int main(int argc, char *argv[]) {
        int npes, myrank;
        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &npes);
        MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

        int i,j, d=3;
        int *A; 
	float *x;
        x = (float*)malloc(d*sizeof(float));
        if(myrank==0) {
                A = (int*)malloc((d*d + d)*sizeof(int));
                //matrix input
		A[0]=5; A[1]=-2; A[2]=3; A[3]=-1;
		A[4]=-3; A[5]=9; A[6]=1; A[7]=2;
		A[8]=2; A[9]=-1; A[10]=-7; A[11]=3;
                //vector input
		x[0]=0; x[1]=0; x[2]=0;
        }
        if (npes != d) { //error printing
                if(myrank==0)
                        printf("Error: Number of processors and matrix dimension should match\n");
                MPI_Finalize();
                return 1;
        }
	int *procA;
	procA = (int*)malloc( (d+1) * sizeof(int) );
	MPI_Scatter(A, d+1, MPI_INT,  procA, d+1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(x, d, MPI_FLOAT, 0, MPI_COMM_WORLD );

	int f = 0;
	
	float x_new, norm; 
	float *x_newArray;
	x_newArray = (float*)malloc(d*sizeof(float));
	for (j=0; j<d; j++)
		x_newArray[j] = x[j];
	int max_itr = 0;
	while (f==0) {
		i = myrank;
		x_new = procA[d];
		for (j=0; j<d; j++) 
			if (j != i) 
				x_new = x_new - procA[j]*x[j];
		x_new = x_new / procA[i];
		MPI_Allgather(&x_new, 1, MPI_FLOAT, x_newArray, 1, MPI_FLOAT, MPI_COMM_WORLD);
		norm = 0;
		for (j=0; j<d; j++)
			norm = norm + pow((x[j] -  x_newArray[j]) , 2 );
		norm = sqrt(norm);
		if (norm < 0.001)
			f = 1;
		for (j=0; j<d; j++)
			x[j] = x_newArray[j];
		max_itr++;
		if (max_itr==50)
			break;
	}
	if (myrank==0) {
		printf("Answer at iteration no. %d:\n",max_itr);
		for (j=0;j<d;j++)
        		printf("%f\n",x_newArray[j]);
	}

        MPI_Finalize();
        return 0;
}
