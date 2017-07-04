#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<math.h>

int main(int argc, char *argv[]) {
        int npes, myrank;
        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &npes);
        MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

        int i,j, d=4;
        int *A; 
	float *x;
        x = (float*)malloc(d*sizeof(float));
        if(myrank==0) {
                A = (int*)malloc((d*d + d)*sizeof(int));
                //matrix input
		A[0]=1; A[1]=2; A[2]=-3; A[3]=4; A[4]=12; 
		A[5]=2; A[6]=2; A[7]=-2; A[8]=3; A[9]=10; 
		A[10]=0; A[11]=1; A[12]=1; A[13]=0; A[14]=-1; 
		A[15]=1; A[16]=-1; A[17]=1; A[18]=-2; A[19]=-4;
                //vector input
		x[0]=1; x[1]=1; x[2]=1; x[3]=1;
        }
        if (npes > d) { //error printing
                if(myrank==0)
                        printf("Error: Number of processors are greater than matrix dimension\n");
                MPI_Finalize();
                return 1;
        }
        //data distribution in processors
        int *procA;
        int scatter_countA = ((d/npes)*(d+1));
        if (myrank==(npes-1))
                scatter_countA = ((d/npes)*(d+1)) + ((d%npes)*(d+1));
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

        float *procX;
	int *scX, *displX ;
        int scatter_countY = (d/npes);
        if (myrank==(npes-1))
                scatter_countY = ((d/npes)) + ((d%npes));
        procX = (float*)malloc( scatter_countY * sizeof(float) );
        if (myrank==0) {
                scX = (int*)malloc( npes*sizeof(int) );
                displX = (int*)malloc( npes*sizeof(int) );
        }
        MPI_Gather(&scatter_countY,1,MPI_INT,scX,1,MPI_INT,0,MPI_COMM_WORLD);
        if (myrank==0) {
                displX[0]=0;
                for (i=1; i<npes; i++)
                        displX[i]=displX[i-1]+scX[i-1];
        }
        MPI_Scatterv(x, scX, displX, MPI_FLOAT, procX, scatter_countY, MPI_FLOAT ,0, MPI_COMM_WORLD);

	MPI_Bcast(x,d,MPI_FLOAT,0,MPI_COMM_WORLD);

        //computations for matrix-vector multiplication
        float *procY;
	float norm,normSum;
        procY = (float*)malloc( scatter_countY * sizeof(float) );
	int itr,k;
	for (itr = 0; itr < 1; itr++) {
		norm = 0;
        	for (i=0;i<scatter_countY;i++)
                	procY[i] = 0;
		for (i=0; i < scatter_countY; i++) {
        	        for (j=0; j < d; j++ ) {
				if (j != (myrank+i))
                        		procY[i] = procY[i] + procA[i*d+i+j]*x[j];
			}
			procY[i] = ( (procA[i*d+i+d] - procY[i]) / procA[i*d+i+myrank+i] );
			printf("%f => %d\n",procY[i],myrank);
		}
		for (i=0;i<scatter_countY;i++)
			norm = norm + pow((procY[i] - procX[i]),2);
		MPI_Reduce(&norm,&normSum,1,MPI_FLOAT,MPI_SUM, 0, MPI_COMM_WORLD);
		//if (myrank==0)
		//	printf("%f\n",sqrt(normSum));
		if (myrank==0 && sqrt(normSum) < 0.01)
			break;
		for (i=0; i < scatter_countY; i++)
			procX[i] = procY[i];
        	MPI_Gatherv(procX, scatter_countY, MPI_FLOAT, x, scX, displX, MPI_FLOAT ,0, MPI_COMM_WORLD);
		MPI_Bcast(x,d,MPI_FLOAT,0,MPI_COMM_WORLD);
		//for (i=0;i<d;i++)
                //	printf("%f=>%d\n ",x[i],myrank);
	}
	/*for (i=0;i<d;i++)
               printf("%f ",x[i]);
        printf("=> rank: %d\n",myrank);*/
   	//Gathering answer
        /*int *y;
        y = (int*)malloc( d * sizeof(int) );
        int *scY, *displY ;
        scY = (int*)malloc( npes*sizeof(int) );
        displY = (int*)malloc( npes*sizeof(int) );
        MPI_Allgather(&scatter_countY,1,MPI_INT,scY,1,MPI_INT,MPI_COMM_WORLD);
        displY[0]=0;
        for (i=1; i<npes; i++)
                displY[i]=displY[i-1]+scY[i-1];

        MPI_Allgatherv(procY,scatter_countY, MPI_INT, y, scY, displY , MPI_INT, MPI_COMM_WORLD);*/



        MPI_Finalize();
        return 0;
}
