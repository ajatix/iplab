#include "GeometricTransform.h"

using namespace cv;
using namespace std;

int main(int argc, char *argv[]) {

	printf("PERFORMING OPERATION...");

	//Getting the parameters
	double scale = atof(argv[2]);
	int option_s = atoi(argv[3]);
	double angle = atof(argv[4]);
	int option_a = atoi(argv[5]);

	//Setting up the images
	IplImage *src = cvLoadImage(argv[1]);
	IplImage *dst_scale, *dst;

	//Deciphering the input commands to perform operations
	//Scaling first
	switch (option_s) {
	case 0: dst_scale = Scale_NN (src, scale); break;
	case 1: dst_scale = Scale_Bilinear (src, scale); break;
	case 2: dst_scale = Scale_Bicubic (src, scale); break;
	default: cout << "Invalid option for scaling\n"; exit(1);
	}

	switch (option_a) {
	case 0: dst = Rot_NN (dst_scale, angle); break;
	case 1: dst = Rot_Bilinear (dst_scale, angle); break;
	default: cout << "Invalid option for rotation\n"; exit(1);
	}

	//Setting up display
	cvNamedWindow ("Input");
	cvNamedWindow ("Output");

	printf("SUCCESS!!!\n\n");

	//Displaying pics
	cvShowImage ("Input", src);
	cvShowImage ("Output", dst);

	//Displaying output on the command window
	printf("The image has been scaled %3.2f times and rotated by %3.2f degrees\n", scale, angle);

	cvWaitKey (0);

	//Saving the images
	char *output_filename = new char[64];
	strcpy(output_filename, argv[1]);
	strcat(output_filename, "[scaled_");
	strcat(output_filename,argv[2]);
	strcat(output_filename, "_type_");
	strcat(output_filename,argv[3]);
	strcat(output_filename, "_rotation_");
	strcat(output_filename,argv[4]);
	strcat(output_filename, "].jpg");

	cvSaveImage(output_filename, dst);
	printf("\nImage saved at %s\n", output_filename);

	//Cleaning up
	cvReleaseImage (&src);
	cvReleaseImage (&dst_scale);
	cvReleaseImage (&dst);
	cvDestroyAllWindows ();

	return 0;
}

IplImage* Scale_NN(IplImage *src, double Scale_Factor) {
	IplImage *dst;
	int adjX,adjY,ceilX,ceilY,floorX,floorY;
	double x,y;

	CvScalar pixel;

	//Creating an empty resized image
	dst = cvCreateImage(cvSize((int)((double)src->width*Scale_Factor),(int)((double)src->height*Scale_Factor)),src->depth,src->nChannels);

	//Scanning entire image pixels
	for(int i = 0;i<dst->height;i++) {
		for(int j = 0;j<dst->width;j++) {

			//Getting original unscaled pixel coordinates
			x = (double)j/Scale_Factor;
			y = (double)i/Scale_Factor;

			//Skipping interpolation for known original pixels
			if(x == floor(x) && y == floor(y)) {
				pixel = cvGet2D(src,(int)y,(int)x);
				cvSet2D(dst,i,j,pixel);
			}
			//Interpolation
			else {
				//Calculating upper and lower bounds of the pixel value for nearest neighbour
				floorX = (int)x;
				floorY = (int)y;
				ceilX = floorX+1;
				ceilY = floorY+1;

				adjX = floorX;
				adjY = floorY;

				//Deciding which point to select
				if ((x-floorX)>0.5) {
					adjX = ceilX;
				}
				if ((y-floorY)>0.5) {
					adjY = ceilY;
				}

				//Checking if selected point exists
				if ((src->width > adjX) && (src->height > adjY)) {
					pixel = cvGet2D (src, adjY, adjX);
				}
				else {
					pixel.val[0] = 0;
				}

				cvSet2D(dst,i,j,pixel);
			}
		}
	}

	return dst;
}

IplImage* Scale_Bilinear(IplImage* src, double Scale_Factor) {
	IplImage *dst;
	int ceilX,ceilY,floorX,floorY;
	double x,y,val[4][4],temp[4];
	CvScalar pixel;

	dst = cvCreateImage(cvSize((int)((double)src->width*Scale_Factor),(int)((double)src->height*Scale_Factor)),src->depth,src->nChannels);   //Create an empty scaled image

	for(int i = 0;i<dst->height;i++) {
		for(int j = 0;j<dst->width;j++)	{

			//Getting original unscaled pixel coordinates
			x = (double)j/Scale_Factor;
			y = (double)i/Scale_Factor;

			//Skipping interpolation for known original pixels
			if(x == floor(x) && y == floor(y)) {
				pixel = cvGet2D(src,(int)y,(int)x);
				cvSet2D(dst,i,j,pixel);
			}
			//Interpolation
			else {
				//Calculating upper and lower bounds of the pixel value for nearest neighbour
				floorX = (int)x;
				floorY = (int)y;
				ceilX = floorX+1;
				ceilY = floorY+1;

				pixel = cvGet2D(src,floorY,floorX);
				val[0][0] = pixel.val[0];

				//Deciding which point to select
				if( src->width > ceilX) {
					pixel = cvGet2D(src,floorY,ceilX);
					val[1][0] = pixel.val[0];
				}
				else {
					val[1][0] = 0;
				}

				if(src->height > ceilY) {
					pixel = cvGet2D(src,ceilY,floorX);
					val[0][1] = pixel.val[0];
				}
				else {
					val[0][1] = 0;
				}

				//Checking if selected point exists
				if((src->width > ceilX) && (src->height > ceilY)) {
					pixel = cvGet2D(src,ceilY,ceilX);
					val[1][1] = pixel.val[0];
				}
				else {
					val[1][1] = 0;
				}

				temp[0] = (val[0][0]*(double)(ceilX-x)) + (val[1][0]*(double)(x-floorX));
				temp[1] = (val[0][1]*(double)(ceilX-x) )+ (val[1][1]*(double)(x-floorX));

				pixel.val[0] = floor((temp[0]*(ceilY-y))+(temp[1]*(y-floorY)));
				cvSet2D(dst,i,j,pixel);
			}
		}
	}

	return dst;
}

IplImage* Scale_Bicubic(IplImage *src, double Scale_Factor) {
	IplImage *dst;
	int ceilX,ceilY,floorX,floorY;
	double x,y,x_frac,y_frac,val[4][4],temp[4],bVal[4];
	CvScalar pixel;

	dst = cvCreateImage(cvSize((int)((double)src->width*Scale_Factor),(int)((double)src->height*Scale_Factor)),src->depth,src->nChannels);   //Create an empty scaled image

	for(int i = 0;i<dst->height;i++) {
		for(int j = 0;j<dst->width;j++) {

			//Getting original unscaled pixel coordinates
			x = (double)j/Scale_Factor;
			y = (double)i/Scale_Factor;

			//Skipping interpolation for known original pixels
			if(x == floor(x) && y == floor(y)) {
				pixel = cvGet2D(src,(int)y,(int)x);
				cvSet2D(dst,i,j,pixel);
			}
			//Interpolation
			else {
				//Calculating upper and lower bounds of the pixel value for nearest neighbour
				floorX = (int)x;
				floorY = (int)y;
				ceilX = floorX+1;
				ceilY = floorY+1;

				//Deciding which point to select from a neighbourhood of 16 pixels
				for(int p = floorX-1;p <= (ceilX+1);p++) {
					for(int q = floorY-1;q <= (ceilY+1);q++) {
						if ((src->height > q) && (src->width > p) && (p >= 0) && (q >= 0)) {
							pixel = cvGet2D(src,q,p);
						}
						else {
							pixel.val[0] = 0;
						}

						val[p-floorX+1][q-floorY+1] = pixel.val[0];
					}
				}

				x_frac = x-(double)floorX;
				y_frac = y-(double)floorY;

				for(int k = 0; k<4; k++) {
					bVal[k] = BSpline_Cubic(x_frac+3.0-(double)k);
				}

				for(int l = 0; l<4; l++) {
					temp[l] = val[0][l]*bVal[0] + val[1][l]*bVal[1] + val[2][l]*bVal[2] + val[3][l]*bVal[3];
				}

				pixel.val[0] = temp[0]*BSpline_Cubic(y_frac+3.0) + temp[1]*BSpline_Cubic(y_frac+2.0) + temp[2]*BSpline_Cubic(y_frac+1.0) + temp[3]*BSpline_Cubic(y_frac);
				cvSet2D(dst,i,j,pixel);
			}
		}
	}

	return dst;
}

double BSpline_Cubic(double val) {
	double y;

	if(val >= 0.0 && val<1.0) {
		y = pow(val,3)/6.0;
	}
	else if(val >= 1.0 && val<2.0) {
		y = -0.5*pow(val,3) + 2*pow(val,2) - 2*val + (4.0/6.0);
	}
	else if(val >= 2.0 && val<3.0) {
		y = 0.5*pow(val,3) - 4*pow(val,2) + 10*val - (22.0/3.0);
	}
	else if(val >= 3.0 && val<4.0) {
		y = pow((4.0-val),3)/6.0;
	}
	else {
		y = 0;
	}

	return y;
}

IplImage* Rot_NN(IplImage *src,double degree) {
	CvScalar pixel;

	//Calculations in radian domain
	float sinV = sin((float)(degree*PI/180));
	float cosV = cos((float)(degree*PI/180));

	int w = src->width;
	int h = src->height;

	//Calculating new image size, considering the rotation
	IplImage *dst = cvCreateImage(cvSize(abs(w*cosV) + abs(h*sinV),abs(w*sinV) + abs(h*cosV)),src->depth,src->nChannels);

	//Doing a geometric transform first
	for(int i = 0; i<src->height; i++) {
		for(int j = 0; j<src->width; j++) {
			pixel = cvGet2D(src, i,j);
			int newRow = i*cosV - j*sinV;
			int newCol = i*sinV + j*cosV;
			cvSet2D(dst,h*sinV+newRow,newCol,pixel);
		}
	}

	pixel = cvGet2D(dst,0,1);

	//Performing interpolation on the rotated image now
	IplImage* dst2 = cvCreateImage(cvGetSize(dst),dst->depth,dst->nChannels);

	for(int i = 0; i<dst->height; i++) {
		for(int j = 0; j<dst->width; j++) {
			pixel = cvGet2D(dst, i,j);
			if((pixel.val[0] == 205)&&(pixel.val[1] == 205)&&(pixel.val[2] == 205)&&(j >= 1)) {
				pixel = cvGet2D(dst,i,j-1);
			}
			cvSet2D(dst2,i,j,pixel);
		}
	}

	return dst2;
}

IplImage* Rot_Bilinear(IplImage *src,double degree) {
	CvScalar pixel;

	//Calculations in radian domain
	float sinV = sin((float)(degree*PI/180));
	float cosV = cos((float)(degree*PI/180));

	int w = src->width;
	int h = src->height;

	//Calculating new image size, considering the rotation
	IplImage *dst = cvCreateImage(cvSize(abs(w*cosV) + abs(h*sinV),abs(w*sinV) + abs(h*cosV)),src->depth,src->nChannels);

	//Doing a geometric transform first
	for(int i = 0; i<src->height; i++) {
		for(int j = 0; j<src->width; j++) {
			pixel = cvGet2D(src, i,j);
			int newRow = i*cosV - j*sinV;
			int newCol = i*sinV + j*cosV;
			cvSet2D(dst,h*sinV+newRow,newCol,pixel);
		}
	}

	pixel = cvGet2D(dst,0,1);

	//Performing interpolation on the rotated image now
	IplImage* dst2 = cvCreateImage(cvGetSize(dst),dst->depth,dst->nChannels);

	for(int i = 1; i<dst->height-1; i++) {
		for(int j = 1; j<dst->width-1; j++) {
			pixel = cvGet2D(dst, i,j);
			if((pixel.val[0] == 205)&&(pixel.val[1] == 205)&&(pixel.val[2] == 205)) {
				for(int colors = 0;colors<dst->nChannels;colors++) {
					pixel.val[colors] = (cvGet2D(dst,i-1,j-1).val[colors] + cvGet2D(dst,i-1,j+1).val[colors] + cvGet2D(dst,i+1,j-1).val[colors] + cvGet2D(dst,i+1,j+1).val[colors])/4;
				}
			}
			cvSet2D(dst2,i,j,pixel);
		}
	}

	return dst2;
}
