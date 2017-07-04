#include<stdio.h>
#include<time.h>
#include<mpi.h>

int main(int argc, char *argv[]) {
	clock_t start=clock();
	
	int npes, myrank;
	long n=500;

        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &npes);
        MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	long i,j,a =( (myrank*(n / npes)) ),b=0,sum=0;
	long start_ = a+1;
	long end = a+(n/npes);

	if (myrank==(npes - 1))
		end = a + (n/npes) + (n%npes);

	for ( i=start_; i<=end ; i++)
                b=b+i;

	if (myrank==0) {
		for (j=(myrank + 1); j < npes ; j++) {
			MPI_Recv(&sum,1,MPI_LONG, j, 2, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			b=b+sum;
		}
		printf("Answer: %ld",b);
	}
	else {
		MPI_Send(&b,1,MPI_LONG,0,2,MPI_COMM_WORLD);
	}
        MPI_Finalize();

	if(myrank==0) {
		clock_t end=clock();
		double time_spent = (end - start) / CLOCKS_PER_SEC;
		printf("\nTimes spent: %lf\n",time_spent);	
	}
	
	return 0;
}
