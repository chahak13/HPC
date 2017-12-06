#include <stdio.h>
#include <math.h>
#include <omp.h>

int main(){
	int i;
	for(i=4;i<=9;i++){
	int k;
	double pi=0;
	long int num_steps = pow(10,i);
	double start_time = omp_get_wtime();
	for(k=1;k<num_steps;k++){
		if(k%2 != 0){
			pi += 1.0/(2*k-1);
		}
		else{
			pi += -1.0/(2*k-1);
		}
	}
	pi *= 4;
	double end_time = omp_get_wtime();
	double elapsed = end_time - start_time;
	printf("pi %0.9lf elapsed %lf\n",pi,elapsed);
	}
}
