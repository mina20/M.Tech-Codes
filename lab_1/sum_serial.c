#include<stdio.h>
#include<time.h>

int main() {
	
	clock_t start=clock();
	
	long sum=0;
	long n=1000000000;
	for (int i=1; i <= n; i++)
		sum+=i;
	printf("%ld\n",sum);

	clock_t end=clock();

	double time_spent = (end - start) / CLOCKS_PER_SEC;
	printf("Times spent: %lf\n",time_spent);	

	return 0;
}
