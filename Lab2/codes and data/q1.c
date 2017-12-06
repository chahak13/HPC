#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>

//static long int num_steps = 10000000;

double f(double x) {
	double ans = 4.0/(1+x*x);
	return(ans);
}

int main() {
	int j;
	for(j=3;j<=8;j++){
		long int num_steps = pow(10,j);
		double dx = 1.0/(double)num_steps;
		double sum = 0;
		double net_start = omp_get_wtime();
		#pragma omp serial
		{	
			int id = omp_get_thread_num();
			double thread_start=omp_get_wtime();
			int i;
			double partial = 0;
			for(i=0;i<num_steps;i++) {
				double x = (i+0.5)*dx;
				partial+=f(x);
			}
			partial*=dx;
			double thread_elapsed = -1*(thread_start-omp_get_wtime());
			#pragma omp critical
			sum+=partial;
		}
		double net_elapsed = omp_get_wtime() - net_start;
		double pi = sum;
		printf("pi %0.9lf net_elapsed %lf\n",pi,net_elapsed);
	}
}
