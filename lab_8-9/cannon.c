#include "mpi.h"
#include <stdio.h>
#include<stdlib.h>

int main (int argc, char* argv[]) {
        int npes, myrank;
        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &npes);
        MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	//data input
	int n = 3,i,j;
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
	int procA, procB;
	MPI_Scatter(a, 1, MPI_INT, &procA, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(b, 1, MPI_INT, &procB, 1, MPI_INT, 0, MPI_COMM_WORLD);

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
			if (coords[0] != 0 ) { 
				MPI_Cart_shift(cartcomm, 1, coords[0], &sendProcA, &recvProcA);
				MPI_Isend(&procA, 1, MPI_INT, sendProcA, i, MPI_COMM_WORLD, &reqsA[0]);
        		        MPI_Irecv(&procA, 1, MPI_INT, recvProcA, i,MPI_COMM_WORLD, &reqsA[1]);
				MPI_Waitall(2, reqsA, statsA);
			}
			if ( coords[1] != 0) {
				MPI_Cart_shift(cartcomm, 0, coords[1], &sendProcB, &recvProcB);
				MPI_Isend(&procB, 1, MPI_INT, sendProcB, i+n, MPI_COMM_WORLD, &reqsB[0]);
		                MPI_Irecv(&procB, 1, MPI_INT, recvProcB, i+n,MPI_COMM_WORLD, &reqsB[1]);
				MPI_Waitall(2, reqsB, statsB);
			}
			c = c + procA*procB;
		}
		else {
			MPI_Cart_shift(cartcomm, 1, 1, &sendProcA, &recvProcA);
			MPI_Isend(&procA, 1, MPI_INT, sendProcA, i, MPI_COMM_WORLD, &reqs[0]);
			MPI_Irecv(&procA, 1, MPI_INT, recvProcA, i,MPI_COMM_WORLD, &reqs[1]);
			MPI_Cart_shift(cartcomm, 0, 1, &sendProcB, &recvProcB);
			MPI_Isend(&procB, 1, MPI_INT, sendProcB, i+n, MPI_COMM_WORLD, &reqs[2]);
			MPI_Irecv(&procB, 1, MPI_INT, recvProcB, i+n,MPI_COMM_WORLD, &reqs[3]);
			MPI_Waitall(4, reqs, stats);
			c = c + procA*procB;
		}
	}
	printf("%d => %d\n", c, myrank);

	MPI_Finalize();	
	return 0;
}
