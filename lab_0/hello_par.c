#include<stdio.h>
#include<mpi.h>

main(int argc, char *argv[]) {
	int npes, myrank;
	//printf("From processor %d out of %d, Hello World!\n",myrank,npes);
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &npes);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	printf("From processor %d out of %d, Hello World!\n",myrank,npes);
	MPI_Finalize();
	return 0;
}
