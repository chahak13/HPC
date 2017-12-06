/*
   Calculating the value of pi using reduction : Parallel Implementation
Author : Omkar Damle.
Date : August 2016.
 */

#include<stdio.h>
#include<math.h>
#include<omp.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>


//  Using the MONOTONIC clock
#define CLK CLOCK_MONOTONIC

/* Function to compute the difference between two points in time */
struct timespec diff(struct timespec start, struct timespec end);

/*
   Function to computes the difference between two time instances
   Taken from - http://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
   Further reading:
http://stackoverflow.com/questions/6749621/how-to-create-a-high-resolution-timer-in-linux-to-measure-program-performance
http://stackoverflow.com/questions/3523442/difference-between-clock-realtime-and-clock-monotonic
 */
struct timespec diff(struct timespec start, struct timespec end){
	struct timespec temp;
	if((end.tv_nsec-start.tv_nsec)<0){
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	}
	else{
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}


int main(int argc, char* argv[])
{
	struct timespec start_e2e, end_e2e, start_alg, end_alg, e2e, alg;
	/* Should start before anything else */
	clock_gettime(CLK, &start_e2e);

	/* Check if enough command-line arguments are taken in. */
	if(argc < 3){
		printf( "Usage: %s n p \n", argv[0] );
		return -1;
	}

	int n=atoi(argv[1]);	/* size of input array */
	int p=atoi(argv[2]);	/* number of processors*/
	char *problem_name = "matrix_multiplication";
	char *approach_name = "omp_parallel";
//	char buffer[10];
//	FILE* inputFile;
	FILE* outputFile;
	//	inputFile = fopen(argv[3],"r");

	char outputFileName[50];
	sprintf(outputFileName,"output/%s_%s_%s_%s_output.txt",problem_name,approach_name,argv[1],argv[2]);

	 int *a[n],*b[n],*c[n];
	 //counters for loops
	 int i,j,k,l,m=0;
	 //putting values in the matrices;
	
	 for(i = 0;i < n;i++){
	 	a[i] = (int *) malloc(n * sizeof(int));
	 	b[i] = (int *) malloc(n * sizeof(int));
	 	c[i] = (int *) malloc(n * sizeof(int));
	 	for(j = 0; j < n; j++){
	 		a[i][j] = 1;
	 		b[i][j] = 1;
	 		c[i][j] = 0;
	 	}
	 }


	//int a[n][n],b[n][n],c[n][n];
	//int i,j,k;

	//for(i=0;i<n;i++){                           //Initialise all three arrays
	//	for(j=0;j<n;j++){
	//		a[i][j] = 1;
	//		b[i][j] = 1;
	//		c[i][j] = 0;
	//	}
	//}

	//Setting parameters for parallelizing the code

	clock_gettime(CLK, &start_alg);	/* Start the algo timer */

	/*----------------------Core algorithm starts here----------------------------------------------*/


	int block_size = n/2;
	omp_set_num_threads(p);

	//Matrix multiplication
	for (i = 0; i < n; i += block_size)
			{
				for (j = 0; j < n; j += block_size)
				{
					#pragma omp parallel for private(k,l,m) collapse(2)
					for (k = 0; k < block_size; k++)
					{
						// printf("%d\n", m);
						for (l = 0; l < block_size; l++)
						{
							for (m = 0; m < n; m++)
							{
								#pragma omp critical
								// printf("(%d,%d,%d,%d,%d)\n",i,j,k,l,m );
								c[i + k][j + l] += a[i + k][m] * b[m][j + l];
							}
						}
					}
				}
			}

/*
double str = sqrt(p);                   // find square root of the number of the cores so that we can approximately partition the block
	int in = str;
	if(in==0)
		in++;
	if(in+0.5<=str)
		in++;
	str = n/(double)in;
	int len = str;
	if(len+0.5<=str)
		len++;
	if(len==0)
		len++;                              // Here len variable is the block size
	//printf("%d %d\n",in,len);

	clock_gettime(CLK, &start_alg);

#pragma omp parallel private(i,j,k)						//Block algorithm parallelization
	{
		int nm = omp_get_thread_num();                  // get index of the thread so that we can distinguish each thread and give them paritcular are of work
		//printf("%d",nm);
		int st = (nm/in)*len;
		int vvv = 0,yu,endd,ppp;
		if(n-st<=len)
			vvv = 1;
		if(vvv==0){
			for(i=st;i<st+len;i++){                         // starting point and ending point of the block (column number)
				yu = (nm%in)*len;
				endd = yu+len;
				if(n-endd<len)
					endd = n;
				for(j=yu;j<endd;j++){                // starting point and ending point of the block (row number)
					for(k=0;k<n;k++)
						c[i][j]+=a[i][k]*b[k][j];
				}
			}
		}
		else{
			for(ppp = st+nm%in;ppp<n;ppp++){            //  This is for the case where distribution is not linear so remaining all threads work on these loops
				for(j=0;j<n;j++){
					c[ppp][j] = 0;
					for(k=0;k<n;k++)
						c[ppp][j]+=a[ppp][k]*b[k][ppp];
				}
			}
		}
	}

*/
	/*----------------------Core algorithm finished--------------------------------------------------*/

	clock_gettime(CLK, &end_alg);	/* End the algo timer */
	/* Ensure that only the algorithm is present between these two
	   timers. Further, the whole algorithm should be present. */


	/* Should end before anything else (printing comes later) */
	clock_gettime(CLK, &end_e2e);
	e2e = diff(start_e2e, end_e2e);
	alg = diff(start_alg, end_alg);

	// /*-----------REMOVE THIS SEGMENT. ONLY FOR DEBUGGING----------------*/
	// for(i=0;i<n;i++){
	// 	for(j=0;j<n;j++)
	// 		printf("%d ", c[i][j]);
	// 	printf("\n");
	// }
	// /*-------------------------------------------------------------------*/


	outputFile = fopen(outputFileName,"w");
//	fprintf(outputFile,"%.8f\n",pi);

	/* problem_name,approach_name,n,p,e2e_sec,e2e_nsec,alg_sec,alg_nsec
	   Change problem_name to whatever problem you've been assigned
	   Change approach_name to whatever approach has been assigned
	   p should be 0 for serial codes!!
	 */
	printf("%s,%s,%d,%d,%ld,%ld,%ld,%ld\n", problem_name, approach_name, n, p, e2e.tv_sec, e2e.tv_nsec, alg.tv_sec, alg.tv_nsec);

	return 0;

}
