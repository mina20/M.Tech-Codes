#include "mpi.h"
#include <stdio.h>
#include<stdlib.h>

int main (int argc, char* argv[]) {
        int npes, myrank;
        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &npes);
        MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	//data input
	int n = 3,i,j,k;
	int *a, *b;
	a = (int*)malloc((n*n)*sizeof(int));
	b = (int*)malloc((n*n)*sizeof(int));
	if (myrank==0) {
		for (i=0; i<(n*n); i++) {
			a[i] = i;
			b[i] = 8-i;
		}
	}

	//data distrubution on processors
	int *procAB;
	procAB = (int*)malloc(2*sizeof(int));
	MPI_Scatter(a, 1, MPI_INT, &procAB[0], 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(b, 1, MPI_INT, &procAB[1], 1, MPI_INT, 0, MPI_COMM_WORLD);

	//creating a virtual cartesian topology
	int dims[2] = {n,n}, periods[2]={1,1}, reorder=0, coords[2];
	MPI_Comm cartcomm;
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &cartcomm);
	MPI_Cart_coords(cartcomm, myrank, 2, coords);

	//matrix multiplaication
	int sendProcA, recvProcA, sendProcB, recvProcB;
	MPI_Request reqsA[2],reqsB[2];
        MPI_Status statsA[2],statsB[2];
	MPI_Request reqs[4];
        MPI_Status stats[4];
	int c = 0;

	for (i=0; i<n; i++) {
		if (i==0) {
			for (j = 0; j < (n-1) ;j++) {
				if (coords[j] != 0 ) {
					k = j+1;
					if (k==2)
						k=0;
					MPI_Cart_shift(cartcomm, k, coords[j], &sendProcA, &recvProcA);
					MPI_Isend(&procAB[j], 1, MPI_INT, sendProcA, i+j, MPI_COMM_WORLD, &reqs[j]);
        			        MPI_Irecv(&procAB[j], 1, MPI_INT, recvProcA, i+j,MPI_COMM_WORLD, &reqs[j+n-1]);
				}
			}
			//MPI_Waitall(4, reqs, stats);
			c = c + procAB[0]*procAB[1];
		}
		else {
			for (j=0;j<n;j++) {
				k = j+1;
				if (k==2)
					k=0;
				MPI_Cart_shift(cartcomm, k, 1, &sendProcA, &recvProcA);
				MPI_Isend(&procAB[j], 1, MPI_INT, sendProcA, i+j, MPI_COMM_WORLD, &reqs[j]);
				MPI_Irecv(&procAB[j], 1, MPI_INT, recvProcA, i+j,MPI_COMM_WORLD, &reqs[j+n]);
				MPI_Waitall(4, reqs, stats);
			}
			c = c + procAB[0]*procAB[1];
		}
	}
	printf("%d => %d\n", c, myrank);

	MPI_Finalize();	
	return 0;
}
