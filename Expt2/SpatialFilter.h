#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opencv2\opencv.hpp"

using namespace std;
using namespace cv;

void meanFilter (IplImage *img, IplImage *out, int size); //Mean filter
void medianFilter (IplImage *img, IplImage *out); //Median filter
void gradientFilter (IplImage *img, IplImage *out); //Gradient filter
void laplacianFilter (IplImage *img, IplImage *out); //Laplacian filter
void sobelFilter (IplImage *img, IplImage *out, int size); //Sobel filter
int SpatialFilter (char* input_textfilename, char* output_filename); //Function for processing one image
void Filter_control (int, void *); //Trackbar control function
int MainSpatialFilterControl (char* input_directory, char* output_directory); //Main function

//Global variables for trackbar
int filter_type = 0;
int const max_type = 5;
IplImage *img, *temp, *out; 

int mask_size = 0;
int const max_size = 1;

char *window_name = "Spatial filter experiment";

char *trackbar_type = "Type";
char *trackbar_size = "Mask size";

void meanFilter (IplImage *img, IplImage *out, int size) {
	CvScalar pixel;
	short int sum[3], *kernel;

	//Forming the kernel for size N
	kernel = (short int*)malloc(sizeof(short int)*(size*size));
	if(kernel == NULL) {
		//Memory allocation error
	}

	for (int i=0;i<size;i++) {
		for (int j=0;j<size;j++) {
			kernel[i*size+j] = 1;
		}
	}

	for (int row=(size-1); row<img->height-(size-1); row++) {
		for (int col=(size-1); col<img->width-(size-1); col++) {
			sum[0] = sum[1] = sum[2] = 0;
			for (int k=0; k<(size*size); k++) {
					pixel = cvGet2D (img, row-1+(k/size), col-1+(k%size));
					for (int color=0; color<img->nChannels; color++) {
						sum[color] += (short int)pixel.val[color]*kernel[k]; //Appling the kernel multiplication
					}
			}

			pixel.val[0] = sum[0]/(size*size);
			pixel.val[1] = sum[1]/(size*size);
			pixel.val[2] = sum[2]/(size*size);
			
			cvSet2D (out,row,col,pixel);
		}
	}

	free(kernel);

	cout << "Mean filter was successful\n";
}

void medianFilter (IplImage *img, IplImage *out) {
	CvScalar pixel;
	short int sum[3], medianArr[9][3];
	
	for (int row=1; row<img->height-1; row++) {
		for (int col=1; col<img->width-1; col++) {
			sum[0] = sum[1] = sum[2] = 0;
			for (int k=0; k<9; k++) {
					pixel = cvGet2D (img, row-1+(k/3), col-1+(k%3));
					for (int color=0; color<img->nChannels; color++) {
						medianArr[k][color] = (short int)pixel.val[color];
					}
			}
			
			//Calculating median value
			int i,j,temp;
			for (int color=0; color<img->nChannels; color++) {
				for (j=1;j<9;j++) {
					temp = medianArr[j][color];
					for (i=j-1;(i>=0) && (temp>medianArr[i][color]);i--) {
						medianArr[i+1][color] = medianArr[i][color];
					}
					medianArr[i+1][color] = temp;
				}
			}

			pixel.val[0] = medianArr[4][0];
			pixel.val[1] = medianArr[4][1];
			pixel.val[2] = medianArr[4][2];
			
			cvSet2D (out,row,col,pixel);
		}
	}
	cout << "Median filter was successful\n";
}

void gradientFilter (IplImage *img, IplImage *out) {
	CvScalar pixel;
	short int sum_x[3], sum_y[3], kernel_x[3][3] = {{-1,-1,-1},{0,0,0},{1,1,1}}, kernel_y[3][3] = {{-1,0,1},{-1,0,1},{-1,0,1}};

	for (int row=1; row<img->height-1; row++) {
		for (int col=1; col<img->width-1; col++) {
			sum_x[0] = sum_x[1] = sum_x[2] = 0;
			sum_y[0] = sum_y[1] = sum_y[2] = 0;
			for (int k=0; k<9; k++) {
					pixel = cvGet2D (img, row-1+(k/3), col-1+(k%3));
					for (int color=0; color<img->nChannels; color++) {
						//sum[color] += (short int)pixel.val[color]*(kernel_x[k/3][k%3] +kernel_y[k/3][k%3]); //Adding both x, y kernels
						sum_x[color] += (short int)pixel.val[color]*(kernel_x[k/3][k%3]);
						sum_y[color] += (short int)pixel.val[color]*(kernel_y[k/3][k%3]); 
					}
			}

			pixel.val[0] = (short int) (sqrt((float)(pow((double)sum_x[0],2) + pow((double)sum_y[0],2)))); //sum[0];
			pixel.val[1] = (short int) (sqrt((float)(pow((double)sum_x[1],2) + pow((double)sum_y[1],2)))); //sum[1];
			pixel.val[2] = (short int) (sqrt((float)(pow((double)sum_x[2],2) + pow((double)sum_y[2],2)))); //sum[2];
			
			cvSet2D (out,row,col,pixel);
		}
	}
	cout << "Gradient filter was successful\n";
}

void laplacianFilter (IplImage *img, IplImage *out) {
	CvScalar pixel;
	short int sum[3], kernel[3][3] = {{-1,-1,-1},{-1,8,-1},{-1,-1,-1}};

	for (int row=1; row<img->height-1; row++) {
		for (int col=1; col<img->width-1; col++) {
			sum[0] = sum[1] = sum[2] = 0;
			for (int k=0; k<9; k++) {
					pixel = cvGet2D (img, row-1+(k/3), col-1+(k%3));
					for (int color=0; color<img->nChannels; color++) {
						sum[color] += (short int)pixel.val[color]*kernel[k/3][k%3];
					}
			}

			pixel.val[0] = sum[0];
			pixel.val[1] = sum[1];
			pixel.val[2] = sum[2];
			
			cvSet2D (out,row,col,pixel);
		}
	}
	cout << "Laplcian filter was successful\n";
}

void sobelFilter (IplImage *img, IplImage *out, int size) {
	CvScalar pixel;
	short int sum_x[3], sum_y[3], kernel_x[3][3] = {{-1,-2,-1},{0,0,0},{1,2,1}}, kernel_y[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}}, temp_val;
	float root;
	int temp_sum;

	for (int row=size-2; row<img->height-size+2; row++) {
		for (int col=size-2; col<img->width-size+2; col++) {
			sum_x[0] = sum_x[1] = sum_x[2] = 0;
			sum_y[0] = sum_y[1] = sum_y[2] = 0;
			for (int k=0; k<size*size; k++) {
					pixel = cvGet2D (img, row-1+(k/size), col-1+(k%size));
					for (int color=0; color<img->nChannels; color++) {
						sum_x[color] += (short int)pixel.val[color]*(kernel_x[k/size][k%size]);
						sum_y[color] += (short int)pixel.val[color]*(kernel_y[k/size][k%size]); 
					}
			}

			pixel.val[0] = (short int) (sqrt((float)(pow((double)sum_x[0],2) + pow((double)sum_y[0],2)))); //sum[0];
			pixel.val[1] = (short int) (sqrt((float)(pow((double)sum_x[1],2) + pow((double)sum_y[1],2)))); //sum[1];
			pixel.val[2] = (short int) (sqrt((float)(pow((double)sum_x[2],2) + pow((double)sum_y[2],2)))); //sum[2];
			
			cvSet2D (out,row,col,pixel);
		}
	}
	cout << "Sobel filter was successful\n";
}

int SpatialFilter (char* input_filename, char* output_filename) {
	int N = 10; //Border size
		
	//Loading, creating temp and output image
	img = cvLoadImage (input_filename);
	temp = cvCreateImage (cvSize(img->width+N-1, img->height+N-1), img->depth, img->nChannels);
	out = cvCreateImage (cvGetSize (temp), img->depth, img->nChannels);

	//Adding convolution borders
	CvPoint offset = cvPoint ((N-1)/2, (N-1)/2);
	cvCopyMakeBorder (img, temp, offset, IPL_BORDER_REPLICATE);

	cvNamedWindow (window_name, CV_WINDOW_AUTOSIZE);

	//Adding trackbars
	createTrackbar (trackbar_type, window_name, &filter_type, max_type, Filter_control ); 
	createTrackbar (trackbar_size, window_name, &mask_size, max_size, Filter_control ); 
	Filter_control (5,0); //Default settings

	cvShowImage ("Reference Image", img);
	cvShowImage (window_name, img);

	//To switch between filters
	int key;
	while (1) {
		key = cvWaitKey(0);
		if ((key==27) || (key=='n')) break;

		switch (key) {
			case 'a': meanFilter (temp, out, 3); cvShowImage (window_name, out); break;
			case 'm': medianFilter (temp, out); cvShowImage (window_name, out); break;
			case 'g': gradientFilter (temp, out); cvShowImage (window_name, out); break;
			case 'l': laplacianFilter (temp, out); cvShowImage (window_name, out); break;
			case 's': sobelFilter (temp, out, 3); cvShowImage (window_name, out); break;
			case 'o': cvShowImage (window_name, img); break;
			case 'p': cvSaveImage(output_filename,out); cout << "File has been saved at " << output_filename << endl; break;
			default: cout << "Incorrect operation\n"; 
		}
	}

	//Cleaning up
	cvReleaseImage (&img);
	cvReleaseImage (&temp);
	cvReleaseImage (&out);	

	return 0;
}

void Filter_control (int, void *) {
	cout << "The filter type is " << filter_type << endl;
	int mask = (3+2*mask_size);
	switch (filter_type) {
	case 0: meanFilter (temp, out, mask); cvShowImage (window_name, out); break;
	case 1: medianFilter (temp, out); cvShowImage (window_name, out); break;
	case 2: gradientFilter (temp, out); cvShowImage (window_name, out); break;
	case 3: laplacianFilter (temp, out); cvShowImage (window_name, out); break;
	case 4: sobelFilter (temp, out, 3); cvShowImage (window_name, out); break;
	case 5: cvShowImage (window_name, img); break;
	default: cout << "Incorrect trackbar type"; 
	}
}

int MainSpatialFilterControl (char* input_directory, char* output_directory) {
	//Setting up
	printf ("Checking if processor is available\n");
	if (system(NULL)) puts ("Ok");
	else exit (EXIT_FAILURE);
	printf ("Executing program\n");

	//The command to be executed to generate and save the list of files
	char filePath[] = "cd images & dir/b *.bmp > imageList.txt";
	system (filePath);

	ifstream list("images/imageList.txt");
	char filename[32], input_filename[32], output_filename[64];
	
	while (!list.eof()) {
		strcpy(input_filename ,input_directory);
		strcpy(output_filename, output_directory);
		list.getline(filename,32);
		cout << "Processing " << filename << endl;
		strcat(input_filename, filename);
		strcat(output_filename, filename);
		if (filename[0] == '\0') break; //To end the program when there are no more files to read
		SpatialFilter (input_filename, output_filename);
	}

	cvDestroyAllWindows();

	return 0;
}