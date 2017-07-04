#include<stdio.h>
#include<time.h>
#include<math.h>
#include<mpi.h>

float f(float x);

int main(int argc, char *argv[]) {
        clock_t begin=clock();
	
	int i, n =100;
	float upperLimit=1, lowerLimit=0;
	float sum=0,a,b,intr;
	float h = ( upperLimit - lowerLimit ) / n;

	int npes, myrank;
	MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &npes);
        MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	int start_ = (myrank*(n / npes)) ;
	int end_ = (myrank*(n / npes)) + (n/npes);
	
	 if (myrank==(npes - 1))
                end_ = (myrank*(n / npes)) + (n/npes) + (n%npes);
	
	for (i=start_; i < end_; i++) {
		a = i*h;
		b = a + h;
		intr = (b-a)/3;
		sum = sum + ((3*intr)/8)*(f(a) + 3*f((2*a+b)/3) + 3*f((a+2*b)/3) + f(b));
	}
	
	MPI_Reduce(&sum, &a, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

							
        if(myrank==0) {
		printf("Answer: %f\n",a);
                clock_t end=clock();
		double time_spent = (end - begin) / CLOCKS_PER_SEC;
                printf("Times spent: %lf\n",time_spent);
        }

	MPI_Finalize();

        return 0;
}

float f(float x) {
	return (2 + sin(2*sqrt(x)));
}


