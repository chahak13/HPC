#include<stdio.h>
#include<math.h>
#include<omp.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>

int p;

//  Using the MONOTONIC clock 
#define CLK CLOCK_MONOTONIC

/* Function to compute the difference between two points in time */
struct timespec diff(struct timespec start, struct timespec end);


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


PPMImage* changeImage(PPMImage* im)
{
	int rows = im->x;
	int cols = im->y;
	int x, y;
	
	PPMImage *im2 = (PPMImage*) malloc(sizeof(PPMImage)); // Output image
	im2->x = rows;	// Set number of rows
	im2->y = cols;	// Set number of columns
	im2->data = (PPMPixel *) malloc(rows * cols * sizeof(PPMPixel));	// Allocate memory for (rows * cols) pixels
	
	/* Generate output image
		1) Find mean
		2) Find standard deviation
		3) In the output image, write each pixel as ABS((original_pixel - mean) / standard_deviation)
	*/
	double red_mean = 0.0, red_std_dev = 0.0;
	double grn_mean = 0.0, grn_std_dev = 0.0;
	double blu_mean = 0.0, blu_std_dev = 0.0;
	for(x = 0; x < rows; x++)
	{
		for(y = 0; y < cols; y++) 
		{
			int idx = (x * cols) + y;
			red_mean += ((double) (im->data + idx)->red);
			grn_mean += ((double) (im->data + idx)->green);
			blu_mean += ((double) (im->data + idx)->blue);
		}
	}
	red_mean /= (rows * cols);
	grn_mean /= (rows * cols);
	blu_mean /= (rows * cols);

	for(x = 0; x < rows; x++)
	{
		for(y = 0; y < cols; y++) 
		{
			int idx = (x * cols) + y;
			red_std_dev += ((((double) (im->data + idx)->red) - red_mean) * (((double) (im->data + idx)->red) - red_mean));
			grn_std_dev += ((((double) (im->data + idx)->green) - grn_mean) * (((double) (im->data + idx)->green) - grn_mean));
			blu_std_dev += ((((double) (im->data + idx)->blue) - blu_mean) * (((double) (im->data + idx)->blue) - blu_mean));
		}
	}

	red_std_dev /= (rows * cols);
	grn_std_dev /= (rows * cols);
	blu_std_dev /= (rows * cols);

	red_std_dev = sqrt(red_std_dev);
	grn_std_dev = sqrt(grn_std_dev);
	blu_std_dev = sqrt(blu_std_dev);
	red_std_dev = 1.0; grn_std_dev = 1.0; blu_std_dev = 1.0;
	// printf("Red:%lf %lf Grn: %lf %lf Blu: %lf %lf\n", red_mean, red_std_dev, grn_mean, grn_std_dev, blu_mean, blu_std_dev);
	for(x = 0; x < rows; x++)
	{
		for(y = 0; y < cols; y++) 
		{
			int idx = (x * cols) + y;
			double red_old = (double) ((im->data + idx)->red);
			double grn_old = (double) ((im->data + idx)->green);
			double blu_old = (double) ((im->data + idx)->blue);
			
			unsigned char red_new = (unsigned char) abs((red_old - red_mean) / red_std_dev);
			unsigned char grn_new = (unsigned char) abs((grn_old - grn_mean) / grn_std_dev);
			unsigned char blu_new = (unsigned char) abs((blu_old - blu_mean) / blu_std_dev);
			(im2->data + idx)->red = red_new;		
			(im2->data + idx)->green = grn_new;
			(im2->data + idx)->blue = blu_new;
			// printf("Old:%d %d %d New:%d %d %d\n", (int) red_old, (int) grn_old, (int)blu_old, red_new, grn_new, blu_new);
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
	p = atoi(argv[2]);	/* number of processors*/
	char *problem_name = "image_warping";
	char *approach_name = "collapsed_directive";
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
		PPMImage *im2 = changeImage(im);
		double end_time = omp_get_wtime();

	/*----------------------Core algorithm finished--------------------------------------------------*/

	clock_gettime(CLK, &end_alg);	/* End the algo timer */
	/* Ensure that only the algorithm is present between these two
	   timers. Further, the whole algorithm should be present. */
	
	char outputImageName[1024];
	outputImageName[0] = '\0';
	strcat(outputImageName, "../../Lenna_normalized_serial_");
	strcat(outputImageName, argv[1]);
	strcat(outputImageName, ".ppm");

        writePPM(outputImageName, im2);
	/* Should end before anything else (printing comes later) */
	clock_gettime(CLK, &end_e2e);
	e2e = diff(start_e2e, end_e2e);
	alg = diff(start_alg, end_alg);

	/*outputFile = fopen(outputFileName,"w");
	fprintf(outputFile,"%.8f\n",pi);*/		

	/* problem_name,approach_name,n,p,e2e_sec,e2e_nsec,alg_sec,alg_nsec
	   Change problem_name to whatever problem you've been assigned
	   Change approach_name to whatever approach has been assigned
	   p should be 0 for serial codes!! 
	 */
	printf("%s,%s,%d,%d,%d,%ld,%d,%ld\n", problem_name, approach_name, n, p, e2e.tv_sec, e2e.tv_nsec, alg.tv_sec, alg.tv_nsec);
	printf("%lf\n", end_time - start_time);
	return 0;
}

