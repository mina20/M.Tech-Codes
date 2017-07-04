#include<stdio.h>
#include<time.h>
#include<mpi.h>

int main(int argc, char *argv[]) {
        clock_t start=clock();

        int npes, myrank;
        long n=100;
	
	long upper_limit = 1;
	long lower_limit = 0;

	long h = ((upper_limit - lower_limit)/n);
	
        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &npes);
        MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

        long i,a =( (myrank*(n / npes)) ),b=0;
        long start_ = a+1;
        long end = a+(n/npes);

        if (myrank==(npes - 1))
                end = a + (n/npes) + (n%npes);

        for ( i=start_; i<=end ; i++)
                b=b+i;

	MPI_Reduce(&b, &a, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (myrank == 0)
		printf("Answer: %ld\n",a);

        MPI_Finalize();

        if(myrank==0) {
                clock_t end=clock();
		double time_spent = (end - start) / CLOCKS_PER_SEC;
                printf("Times spent: %lf\n",time_spent);
        }

        return 0;
}


