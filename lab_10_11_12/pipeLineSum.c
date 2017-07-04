#include <stdio.h>
#include <mpi.h>
#define tag 100  /* tag for sending a number */
#define v 1      /* verbose flag, output if 1, no output if 0 */

void pipeline_sum ( int i, int p );
/* performs a pipeline sum of p*(p+1) numbers */

int main ( int argc, char *argv[] ) {
	int i,p;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&i);
	MPI_Comm_size(MPI_COMM_WORLD,&p);

	pipeline_sum(i,p);
	MPI_Finalize();
	return 0;
}

void pipeline_sum ( int i, int p ) /*performs a pipeline sum of p*(p+1) numbers*/ {
	int n[p][p-i+1];
	int j,k;
	MPI_Status status;
	if(i==0)  /*manager generates numbers*/ {
		for(j=0; j<p; j++)
			for(k=0; k<p+1; k++) 
				n[j][k] = (p+1)*j+k+1;
		if(v>0) {
			printf("The data to sum : ");
			for(j=0; j<p; j++)
				for(k=0; k<p+1; k++) 
					printf(" %d",n[j][k]);
			printf("\n");
		}
	}
	for(j=0; j<p; j++)
		if(i==0)  /*manager starts pipeline of j-th sequence*/ {
			n[j][1] += n[j][0];
			printf("Manager starts pipeline for sequence %d...\n",j);
			MPI_Send(&n[j][1],p,MPI_INT,1,tag,MPI_COMM_WORLD);
			MPI_Recv(&n[j][0],1,MPI_INT,p-1,tag,MPI_COMM_WORLD,&status);
			printf("Manager received sum %d.\n",n[j][0]);
		}      /*worker i receives p-i+1 numbers*/
		else      /*worker i receives p-i+1 numbers*/ {
			MPI_Recv(&n[j][0],p-i+1,MPI_INT,i-1,tag,MPI_COMM_WORLD,&status);
			printf("Processor %d receives sequence %d : ",i,j);
			for(k=0; k<p-i+1; k++) 
				printf(" %d", n[j][k]);
			printf("\n");
			n[j][1] += n[j][0];
			if(i < p-1)
				MPI_Send(&n[j][1],p-i,MPI_INT,i+1,tag,MPI_COMM_WORLD);
			else
				MPI_Send(&n[j][1],1,MPI_INT,0,tag,MPI_COMM_WORLD);
		}
	if(i==0)  /*manager computes the total sum*/ {
	        for(j=1; j<p; j++) 
			n[0][0] += n[j][0];
		printf("The total sum : %d\n",n[0][0]);
	}
}
