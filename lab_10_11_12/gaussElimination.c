#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<math.h>

int main(int argc, char *argv[]) {
        int npes, rank;
        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &npes);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        int i,j,k,d=3, tag = 1;
	// data input
        float *A;
        float *b;
        if(rank==0) { 
                A = (float*)malloc((d*d)*sizeof(float));
        	b = (float*)malloc(d*sizeof(float));	
                //matrix A input
                A[0]=5; A[1]=6; A[2]=9; 
		A[3]=8; A[4]=7; A[5]=4; 
		A[6]=3; A[7]=2;  A[8]=10;
                //vector b input
		b[0]=2; b[1]=3; b[2]=4;
        }
	
	// data distribution in processors
	float *procA, *tempA, procB, tempB;
        procA = (float*)malloc( (d) * sizeof(float) );
        tempA = (float*)malloc( (d) * sizeof(float) );
        MPI_Scatter(A, d, MPI_FLOAT,  procA, d, MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Scatter(b, npes/d, MPI_FLOAT,  &procB, npes/d, MPI_FLOAT, 0, MPI_COMM_WORLD);

	// error prompt
        if (npes != d) {
                if(rank==0)
                        printf("Error: Number of processors and matrix dimension should match\n");
                MPI_Finalize();
                return 1;
        }

	// gaussian elimination
	for (k=0;k<d;k++) {
		if (rank==k)/*division*/ {
			for (i=k+1; i<d; i++) 
				procA[i] = procA[i] / procA[k];
			procB = procB / procA[k];
			procA[k] = 1;
			for (i=k+1; i<d; i++) {
				MPI_Send(&procA[k+1],d-k-1,MPI_FLOAT, i, tag, MPI_COMM_WORLD);
				MPI_Send(&procB,1,MPI_FLOAT, i, tag, MPI_COMM_WORLD);
			}
		}
		if (rank > k)/*elimination*/{
			MPI_Recv(tempA, d-k-1, MPI_FLOAT, k, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
			MPI_Recv(&tempB, 1, MPI_FLOAT, k, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
			for (i=k+1; i<d; i++)
				procA[i] = procA[i] - procA[k]*tempA[i-k-1];
			procB = procB - procA[k]*tempB;	
			procA[k] = 0;
		}
	}
	
	//back substituttion
	float x,*allX;
        allX = (float*)malloc( (d) * sizeof(float) );
	for (i=0;i<d;i++)
		allX[i] = 0;
	for (k=(d-1);k>=0;k--) {
		if (rank==k) {	
			for (i=k+1; i<d;i++)
				MPI_Recv(&allX[i], 1, MPI_FLOAT, i, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			x = procB;
			for ( i=k+1; i<d;i++) 
				x = x - procA[i]*allX[i];
			for (i=k-1;i>=0;i--)
				MPI_Send(&x,1,MPI_FLOAT,i, tag, MPI_COMM_WORLD);
		}
	}

	for (i=0; i<d ; i++)
		printf("%f ", procA[i]);
	printf("%f => %d\n", procB,rank);

	MPI_Gather(&x, 1, MPI_FLOAT,allX, 1, MPI_FLOAT,0, MPI_COMM_WORLD);
	/*if (rank==0) {
		printf("Answer: ");
		for (i=0; i<d ; i++)
			printf("%f ", allX[i]);
		printf("\n");
	}*/
        MPI_Finalize();
        return 0;
}

