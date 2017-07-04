#include <stdio.h>
#include<stdlib.h>
#include<mpi.h>

int partition(int array[], int start, int end, int pivot);
void quickSort(int array[], int start, int end);
void swap(int *a, int *b);

int main(int argc, char *argv[])
{
	int npes, myrank;
	MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &npes);
        MPI_Comm_rank(MPI_COMM_WORLD, &myrank);	

        int n=16,i;
	int input[] = {8,5,7,4,6,3,9,1,2,54,6,9,7,5,3,4};

	int start_ = myrank*(n/npes);
	int end_ = (myrank*(n / npes)) + (n/npes);
	int size_ = (n/npes);
         if (myrank==(npes - 1)) {
                end_ = (myrank*(n / npes)) + (n/npes) + (n%npes);
		size_ = (n/npes) + (n%npes);
	}

	int array[n];
	for (i=0;i<size_;i++)
		array[i] = input[start_+i];

	int pivot=7;
	int part = partition(array,0,(size_-1),pivot);
	/*for (i=0;i<size_;i++)
		printf("%d ",array[i]);
	printf("=>part: %d, rank: %d \n",part,myrank);*/
		
	int recv_rank = myrank + npes/2;
	int send_rank = myrank - npes/2;
	int sr_count=0;
	
	if (myrank >= (npes/2)) {
		//send to 0 and 1
		MPI_Send(&part,1,MPI_INT,send_rank,2,MPI_COMM_WORLD);
		MPI_Send(&array,(part+1),MPI_INT,send_rank,1,MPI_COMM_WORLD);

		//recv from 0 and 1
		MPI_Recv(&sr_count,1,MPI_INT,send_rank,3,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		MPI_Recv(&array[size_],sr_count,MPI_INT,send_rank,4,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		
		size_ = size_ + sr_count;
		for (i=0;i<(size_- (part+1));i++)
			array[i] = array[i+part+1];
		size_ = size_ - part - 1;
	}
	else {	
		//send to 2 and 3
		int temp1=size_-part-1;
		MPI_Send(&temp1,1,MPI_INT,recv_rank,3,MPI_COMM_WORLD);
		MPI_Send(&array[part+1],temp1,MPI_INT,recv_rank,4,MPI_COMM_WORLD);

		//recv from 2 and 3
		MPI_Recv(&sr_count,1,MPI_INT,recv_rank,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		sr_count++;
                MPI_Recv(&array[size_],sr_count,MPI_INT,recv_rank,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		
		size_ = size_ + sr_count;
		for (i=part+1;i<(part+1+sr_count);i++)
			array[i] = array[i + (size_-sr_count-part-1)];
		size_ = size_- (size_ - sr_count- part -1);
	}
	
	int n_npes = (npes/2);
	if (myrank < (npes/2)) {
	pivot=3;
	part = partition(array,0,(size_-1),pivot);
	/*for (i=0;i<size_;i++)
		printf("%d ",array[i]);
	printf("=>part: %d, rank: %d \n",part,myrank);*/
		
	recv_rank = myrank + n_npes/2;
	send_rank = myrank - n_npes/2;
	sr_count=0;
	
	if (myrank >= (n_npes/2)) {
		//send to 0 and 1
		MPI_Send(&part,1,MPI_INT,send_rank,2,MPI_COMM_WORLD);
		MPI_Send(&array,(part+1),MPI_INT,send_rank,1,MPI_COMM_WORLD);

		//recv from 0 and 1
		MPI_Recv(&sr_count,1,MPI_INT,send_rank,3,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		MPI_Recv(&array[size_],sr_count,MPI_INT,send_rank,4,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		
		size_ = size_ + sr_count;
		for (i=0;i<(size_- (part+1));i++)
			array[i] = array[i+part+1];
		size_ = size_ - part - 1;
	}
	else {	
		//send to 2 and 3
		int temp1=size_-part-1;
		MPI_Send(&temp1,1,MPI_INT,recv_rank,3,MPI_COMM_WORLD);
		MPI_Send(&array[part+1],temp1,MPI_INT,recv_rank,4,MPI_COMM_WORLD);

		//recv from 2 and 3
		MPI_Recv(&sr_count,1,MPI_INT,recv_rank,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		sr_count++;
                MPI_Recv(&array[size_],sr_count,MPI_INT,recv_rank,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		
		size_ = size_ + sr_count;
		for (i=part+1;i<(part+1+sr_count);i++)
			array[i] = array[i + (size_-sr_count-part-1)];
		size_ = size_- (size_ - sr_count- part -1);
	}
	}
	else {
	pivot=9;
	part = partition(array,0,(size_-1),pivot);
	/*for (i=0;i<size_;i++)
		printf("%d ",array[i]);
	printf("=>part: %d, rank: %d \n",part,myrank);*/
		
	recv_rank = myrank + n_npes/2;
	send_rank = myrank - n_npes/2;
	sr_count=0;
	
	if (myrank >= ((n_npes/2)+n_npes) ) {
		//send to 0 and 1
		MPI_Send(&part,1,MPI_INT,send_rank,2,MPI_COMM_WORLD);
		MPI_Send(&array,(part+1),MPI_INT,send_rank,1,MPI_COMM_WORLD);

		//recv from 0 and 1
		MPI_Recv(&sr_count,1,MPI_INT,send_rank,3,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		MPI_Recv(&array[size_],sr_count,MPI_INT,send_rank,4,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		
		size_ = size_ + sr_count;
		for (i=0;i<(size_- (part+1));i++)
			array[i] = array[i+part+1];
		size_ = size_ - part - 1;
	}
	else {	
		//send to 2 and 3
		int temp1=size_-part-1;
		MPI_Send(&temp1,1,MPI_INT,recv_rank,3,MPI_COMM_WORLD);
		MPI_Send(&array[part+1],temp1,MPI_INT,recv_rank,4,MPI_COMM_WORLD);

		//recv from 2 and 3
		MPI_Recv(&sr_count,1,MPI_INT,recv_rank,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		sr_count++;
                MPI_Recv(&array[size_],sr_count,MPI_INT,recv_rank,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		
		size_ = size_ + sr_count;
		for (i=part+1;i<(part+1+sr_count);i++)
			array[i] = array[i + (size_-sr_count-part-1)];
		size_ = size_- (size_ - sr_count- part -1);
	}
	}

	for (i=0;i<size_;i++)
		printf("%d ",array[i]);
	printf("=>part: %d, rank: %d \n",part,myrank);
	
        quickSort(array,0,size_-1);

	for (i=0;i<size_;i++)
		printf("%d ",array[i]);
	printf("=> (sorted)  part: %d, rank: %d \n",part,myrank);
	

	int *size_array, *displ, *output;
	if(myrank==0) {
		size_array = (int*)malloc(npes*sizeof(int));
		output = (int*)malloc(n*sizeof(int));
		displ = (int*)malloc(npes*sizeof(int));
	}
	MPI_Gather(&size_, 1, MPI_INT, size_array, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if (myrank==0) {
		displ[0]=0;
		for (i=1; i<npes; i++)
		displ[i]=displ[i-1]+size_array[i-1];
	}
	MPI_Gatherv(&array,size_, MPI_INT, output, size_array, displ, MPI_INT, 0, MPI_COMM_WORLD); 
	
	if (myrank==0) {
        	printf("The sorted array: ");
        	for (i = 0; i<n; i++)
                	printf("%d ",output[i]);
        	printf("\n");
	}

	MPI_Finalize();

        return 0;
}


int partition(int array[], int start, int end, int pivot)
{
        int i = start;
        int j = end;
        while(i<j) {
                while(array[i]<=pivot && i<end)
                        i++;
                while(array[j]>pivot && j>start)
                        j--;
                if(i<j)
                        swap(&array[i],&array[j]);
        }
        if (j != start)
                swap(&array[start],&array[j]);
        return j;
}

void quickSort(int array[], int start, int end)
{
        if (start < end)
        {
                int part = partition(array, start, end,array[start]);
                quickSort(array,start, part-1);
                quickSort(array,part+1,end);
        }
}

void swap(int *a, int *b) {
        *a+=*b;
        *b=*a-*b;
        *a=*a-*b;
}


