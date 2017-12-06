#include<stdio.h>
#include<math.h>
#include<omp.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>

int nborSize;
int halfwidth = 3;
int p;

//  Using the MONOTONIC clock 
#define CLK CLOCK_MONOTONIC

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

typedef struct {
  unsigned char red,green,blue;
} PPMPixel;

typedef struct {
  int x, y; 
  PPMPixel *data;
} PPMImage;

typedef struct {
  unsigned char gs;
} PPMPixelGS;


typedef struct {
  int x, y;
  PPMPixelGS *data;
} PPMImageGS;



#define RGB_COMPONENT_COLOR 255


int red_neighbours[1024];
int blue_neighbours[1024];
int green_neighbours[1024];

int getNbors(PPMImage * im,int x,int y) {
	int i,j,count=0,m,n;
	int rows = im->x;
	int cols = im->y;
	for(i = x - halfwidth; i <= x + halfwidth; i++){
		m = i;
		if(i < 0)
			m = 0;
		else if(i >= rows)
			m = rows - 1;
		for(j = y - halfwidth; j <= y + halfwidth; j++){
			n = j;
			if(j < 0)
				n = 0;
			else if(j >= cols)
				n = cols - 1;
			int idx1 = (m * cols) + n;
			PPMPixel *temp1 = im->data + idx1;
			red_neighbours[count] = temp1->red;
			green_neighbours[count] =	temp1->green;
			blue_neighbours[count] = temp1->blue;
			count++;
		}
	}
	return 0;
}

int cmpfunc (const void * a, const void * b)
{
	return ( *(int*)a - *(int*)b );
}



PPMImage* changeImage(PPMImage * im)
{
	int rows = im->x;
	int cols = im->y;
	int x, y, halfWidth;
	PPMImage *im2 = (PPMImage*) malloc(sizeof(PPMImage));
	im2->x = rows;
	im2->y = cols;
	im2->data = (PPMPixel *) malloc(rows*cols*sizeof(PPMPixel));
	int qq = 1;
	omp_set_num_threads(p);
	#pragma omp parallel for private(x,y) firstprivate(red_neighbours,green_neighbours,blue_neighbours) shared(im,im2,rows,cols,nborSize,halfwidth) 
	for (x = 0; x < rows; x++) {
		for (y = 0; y < cols; y++) {
			int i, j, m, n;
			int count = 0;
			for(i = x - halfwidth; i <= x + halfwidth; i++){
				m = i;
				if(i < 0)
					m = 0;
				else if(i >= rows)
					m = rows - 1;
				for(j = y - halfwidth; j <= y + halfwidth; j++){
					n = j;
					if(j < 0)
						n = 0;
					else if(j >= cols)
						n = cols - 1;
					int idx1 = (m * cols) + n;
					PPMPixel *temp1 = im->data + idx1;
					red_neighbours[count] = temp1->red;
					green_neighbours[count] =	temp1->green;
					blue_neighbours[count] = temp1->blue;
					count++;
				}
			}
			int ii=0;
			qsort(red_neighbours, nborSize, sizeof(int), cmpfunc);
			qsort(green_neighbours, nborSize, sizeof(int), cmpfunc);
			qsort(blue_neighbours, nborSize, sizeof(int), cmpfunc);
			int index = (x * cols) + y;
			PPMPixel *temp2 = im2->data + index;
			temp2->red = red_neighbours[(nborSize/2)+1];
			temp2->green = green_neighbours[(nborSize/2)+1];
			temp2->blue = blue_neighbours[(nborSize/2)+1];
		}
	}
	return im2;
}


static PPMImage *readPPM(const char *filename)
{
  char buff[16];
  PPMImage *img;
  FILE *fp;
  int c, rgb_comp_color;
  //open PPM file for reading
  fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Unable to open file '%s'\n", filename);
    exit(1);
  }

  //read image format
  if (!fgets(buff, sizeof(buff), fp)) {
    perror(filename);
    exit(1);
  }

  //check the image format
  if (buff[0] != 'P' || buff[1] != '6') {
    fprintf(stderr, "Invalid image format (must be 'P6')\n");
    exit(1);
  }

  //alloc memory form image
  img = (PPMImage *)malloc(sizeof(PPMImage));
  if (!img) {
    fprintf(stderr, "Unable to allocate memory\n");
    exit(1);
  }

  //check for comments
  c = getc(fp);
  while (c == '#') {
    while (getc(fp) != '\n') ;
    c = getc(fp);
  }

  ungetc(c, fp);
  //read image size information
  if (fscanf(fp, "%d %d", &img->x, &img->y) != 2) {
    fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
    exit(1);
  }

  //read rgb component
  if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
    fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
    exit(1);
  }

  //check rgb component depth
  if (rgb_comp_color!= RGB_COMPONENT_COLOR) {
    fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
    exit(1);
  }

  while (fgetc(fp) != '\n') ;
  //memory allocation for pixel data
  img->data = (PPMPixel*)malloc(img->x * img->y * sizeof(PPMPixel));

  if (!img) {
    fprintf(stderr, "Unable to allocate memory\n");
    exit(1);
  }

  //read pixel data from file
  if (fread(img->data, 3 * img->x, img->y, fp) != img->y) {
    fprintf(stderr, "Error loading image '%s'\n", filename);
    exit(1);
  }

  fclose(fp);
  return img;
}

void writePPM(const char *filename, PPMImage *img)
{
  FILE *fp;
  //open file for output
  fp = fopen(filename, "wb");
  if (!fp) {
    fprintf(stderr, "Unable to open file '%s'\n", filename);
    exit(1);
  }

  //write the header file
  //image format
  fprintf(fp, "P6\n");

  //comments


  //image size
  fprintf(fp, "%d %d\n",img->x,img->y);

  // rgb component depth
  fprintf(fp, "%d\n",255);

  // pixel data
  fwrite(img->data, 3 * img->x, img->y, fp);
  fclose(fp);
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
	p=atoi(argv[2]);	/* number of processors*/
	char *problem_name = "median_filtering";
	char *approach_name = "qsort";
	FILE* outputFile;
	char* c=argv[1];
	char* str="../../Lenna";
	char* str2=malloc(15);
	strcpy(str2,str);
	strcat(str2,c);
	char* str3=".ppm";
	strcat(str2,str3);
      	char* filename=str2;
	 PPMImage *im;
 	 im = readPPM(filename);

  	char outputFileName[50];		
	sprintf(outputFileName,"output/%s_%s_%s_%s_output.txt",problem_name,approach_name,argv[1],argv[2]);	

	clock_gettime(CLK, &start_alg);	/* Start the algo timer */

	/*----------------------Core algorithm starts here----------------------------------------------*/
	double start_time = omp_get_wtime();	
	PPMImage* im2 = changeImage(im);
	double end_time = omp_get_wtime();
	/*----------------------Core algorithm finished--------------------------------------------------*/

	clock_gettime(CLK, &end_alg);	/* End the algo timer */
	/* Ensure that only the algorithm is present between these two
	   timers. Further, the whole algorithm should be present. */

	char outputImageName[1024];
	outputImageName[0] = '\0';
	strcat(outputImageName, "../../Lenna_");
	strcat(outputImageName, argv[1]);
	strcat(outputImageName,"_filtering_parallel.ppm");
    	writePPM(outputImageName,im2);

	/* Should end before anything else (printing comes later) */
	clock_gettime(CLK, &end_e2e);
	e2e = diff(start_e2e, end_e2e);
	alg = diff(start_alg, end_alg);

	printf("%s,%s,%d,%d,%d,%ld,%d,%ld\n", problem_name, approach_name, n, p, e2e.tv_sec, e2e.tv_nsec, alg.tv_sec, alg.tv_nsec);

	return 0;
}

