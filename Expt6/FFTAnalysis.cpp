#include "FFTAnalysis.h"

using namespace cv;
using namespace std;

int main (int argc, char *argv[]) {
	//Extracting info from input commands
	int type = atoi(argv[3]); //Filter type
	int size = atoi(argv[4]); //Mask size
	int mode = atoi(argv[5]); //Color or grayscale
	int order = atoi(argv[6]); //Order

	//Setting up
	IplImage *img, *out;

	cout << "LOADING IMAGE...";

	img = cvLoadImage (argv[1], mode);
	out = cvCreateImage (cvSize(img->width, img->height), img->depth, img->nChannels);

	cout << "DONE!!!\n";

	int w = img->width, h = img->height;

	//Creating the complex image
	complexImg temp;

	//Memory allocation
	for (int c=0; c<img->nChannels; c++) {
		temp.channel[c].data = (complexNum**) malloc ((h) * sizeof (complexNum*));
		for (int i=0; i<h; i++) {
			temp.channel[c].data[i] = (complexNum*) malloc ((w) * sizeof (complexNum));
		}
	}

	//Object for each pixel
	CvScalar pixel;

	//Converting image to complex image
	for (int i=0; i<h; i++) {
		for (int j=0; j<w; j++) {
			//Get individual pixel data
			pixel = cvGet2D (img,i,j);
			for (int c=0; c<img->nChannels; c++) { //Number of channels
				temp.channel[c].data[i][j].real = pixel.val[c];
				temp.channel[c].data[i][j].imag = 0;
			}
		}
	}

	cout << "CONVERTING TO FOURIER DOMAIN...";

	//Forward FFT
	for (int c=0; c<img->nChannels; c++) {
		FFT2D (temp.channel[c].data,h,w);
	}

	cout << "DONE!!!\n";

	//Performing the filter operation
	switch (type) {
	case 0: 
		cout << "PERFORMING IDEAL LOW PASS FILTER...";
		for (int c=0; c<img->nChannels; c++) {
			for (int o=0; o<order; o++) {
				LPF (temp.channel[c].data,h,w,size);
			}
		}
		cout << "DONE!!!\n";
		break;
	case 1:
		cout << "PERFORMING IDEAL HIGH PASS FILTER...";
		for (int c=0; c<img->nChannels; c++) {
			for (int o=0; o<order; o++) {
				HPF (temp.channel[c].data,h,w,size);
			}
		}
		cout << "DONE!!!\n";
		break;
	case 2:
		cout << "PERFORMING BUTTERWORTH LOW PASS FILTER...";
		for (int c=0; c<img->nChannels; c++) {
			for (int o=0; o<order; o++) {
				Butterworth_LPF (temp.channel[c].data,h,w,size);
			}
		}
		cout << "DONE!!!\n";
		break;
	case 3:
		cout << "PERFORMING BUTTERWORTH HIGH PASS FILTER...";
		for (int c=0; c<img->nChannels; c++) {
			for (int o=0; o<order; o++) {
				Butterworth_HPF (temp.channel[c].data,h,w,size);
			}
		}
		cout << "DONE!!!\n";
		break;
	case 4:
		cout << "PERFORMING GAUSSIAN LOW PASS FILTER...";
		for (int c=0; c<img->nChannels; c++) {
			for (int o=0; o<order; o++) {
				Gaussian_LPF (temp.channel[c].data,h,w,size);
			}
		}
		cout << "DONE!!!\n";
		break;
	case 5:
		cout << "PERFORMING GAUSSIAN HIGH PASS FILTER...";
		for (int c=0; c<img->nChannels; c++) {
			for (int o=0; o<order; o++) {
				Gaussian_HPF (temp.channel[c].data,h,w,size);
			}
		}
		cout << "DONE!!!\n";
		break;
	default: cout << "Wrong entry\n";
	}

	cout << "CONVERTING BACK TO SPATIAL DOMAIN...";

	//Inverse FFT
	for (int c=0; c<img->nChannels; c++) {
		IFFT2D (temp.channel[c].data,h,w);
	}

	cout << "DONE!!!\n";

	for (int i=0; i<h; i++) {
		for (int j=0; j<w; j++) {
			for (int c=0; c<img->nChannels; c++) { //Number of channels
				pixel.val[c] = (unsigned char)sqrt((temp.channel[c].data[i][j].real)*(temp.channel[c].data[i][j].real) + (temp.channel[c].data[i][j].imag)*(temp.channel[c].data[i][j].imag));
				//Unsigned char to ensure pixel values are within range
			}
			cvSet2D (out,i,j,pixel);
		}
	}

	//Setting up display
	cvNamedWindow ("Input");
	cvNamedWindow ("Output");

	cvShowImage ("Input", img);
	cvShowImage ("Output", out);

	cvWaitKey(0);

	cout << "SAVING IMAGE...";
	cvSaveImage (argv[2], out);	
	cout << "DONE!!!\n";

	//Cleaning up after the program
	for (int c=0; c<out->nChannels; c++) {
		free(temp.channel[c].data);
	}
	cvReleaseImage(&img);
	cvReleaseImage(&out);

	cvDestroyAllWindows();

	return 0;
}

void FFT (complexNum *x, complexNum *y, int N) {
	complexNum *e,*o; //For even and odd indices
	complexNum *fe,*fo;

	if(N==1) { //One-element fft
		y[0].real = x[0].real;
		y[0].imag = x[0].imag;
		return;
	}

	//Memory allocation of temporary variables
	e = (complexNum*)malloc(sizeof(complexNum)*N/2);
	o = (complexNum*)malloc(sizeof(complexNum)*N/2);
	fe = (complexNum*)malloc(sizeof(complexNum)*N/2);
	fo = (complexNum*)malloc(sizeof(complexNum)*N/2);

	for(int i=0;i<N;i++) {
		if(i%2==0) {
			e[i/2].real = x[i].real;
			e[i/2].imag = x[i].imag;
		}
		else {
			o[(i-1)/2].real = x[i].real;
			o[(i-1)/2].imag = x[i].imag;
		}
	}

	//Performing one-dimensional FFT
	FFT (e,fe,N/2);
	FFT (o,fo,N/2);

	//Writing back transformed output to image pointer
	for(int i=0;i<N/2;i++) {
		y[i].real = fe[i].real + cos((2*PI*i)/N)*fo[i].real + sin((2*PI*i)/N)*fo[i].imag;
		y[i].imag = fe[i].imag + cos((2*PI*i)/N)*fo[i].imag - sin((2*PI*i)/N)*fo[i].real;
		y[i+N/2].real = fe[i].real - cos((2*PI*i)/N)*fo[i].real - sin((2*PI*i)/N)*fo[i].imag;
		y[i+N/2].imag = fe[i].imag - cos((2*PI*i)/N)*fo[i].imag + sin((2*PI*i)/N)*fo[i].real;
	}
}

void IFFT (complexNum *y,complexNum *x,int N) {
	complexNum *e,*o; //For even and odd indices
	complexNum *fe,*fo;

	if(N==1) { //One-element fft
		x[0].real = y[0].real;
		x[0].imag = y[0].imag;
		return;
	}

	//Memory allocation of temporary variables
	e = (complexNum*)malloc(sizeof(complexNum)*N/2);
	o = (complexNum*)malloc(sizeof(complexNum)*N/2);
	fe = (complexNum*)malloc(sizeof(complexNum)*N/2);
	fo = (complexNum*)malloc(sizeof(complexNum)*N/2);

	for(int i=0;i<N;i++) {
		if(i%2==0) {
			e[i/2].real = y[i].real;
			e[i/2].imag = y[i].imag;
		}
		else {
			o[(i-1)/2].real = y[i].real;
			o[(i-1)/2].imag = y[i].imag;
		}
	}

	//Performing one-dimensional Inverse FFT
	IFFT(e,fe,N/2);
	IFFT(o,fo,N/2);

	//Writing back transformed output to image pointer
	for(int i=0;i<N/2;i++) {
		x[i].real = (fe[i].real + cos((2*PI*i)/N)*fo[i].real - sin((2*PI*i)/N)*fo[i].imag);
		x[i].imag = (fe[i].imag + cos((2*PI*i)/N)*fo[i].imag + sin((2*PI*i)/N)*fo[i].real);
		x[i+N/2].real = (fe[i].real - cos((2*PI*i)/N)*fo[i].real + sin((2*PI*i)/N)*fo[i].imag);
		x[i+N/2].imag = (fe[i].imag - cos((2*PI*i)/N)*fo[i].imag - sin((2*PI*i)/N)*fo[i].real);
	}
}

void FFT2D(complexNum **c,int h,int w) {
	complexNum *temp, *temp2;

	temp = (complexNum*) malloc (h*sizeof(complexNum));
	temp2 = (complexNum*) malloc (h*sizeof(complexNum));

	//Error checking
	if (temp == NULL) {
		exit(0);
	}

	//Column transform first
	for (int j=0;j<w;j++) {
		for (int i=0;i<h;i++) {
			temp[i].real = pow (-1, (float) (i+j) ) * c[i][j].real;
			temp[i].imag = 0;
		}

		FFT (temp,temp2,h);

		for (int i=0;i<h;i++) {
			c[i][j].real = temp2[i].real;
			c[i][j].imag = temp2[i].imag;
		}
	}

	free (temp);
	free (temp2);

	temp = (complexNum*) malloc (w*sizeof(complexNum));
	temp2 = (complexNum*) malloc (h*sizeof(complexNum));

	//Error checking
	if (temp == NULL) {
		exit(0);
	}

	//Row transform next
	for (int i=0;i<h;i++) {
		for (int j=0;j<w;j++) {
			temp[j].real = c[i][j].real;
			temp[j].imag = c[i][j].imag;
		}

		FFT (temp,temp2,w);

		for (int j=0;j<w;j++) {
			c[i][j].real = temp2[j].real;
			c[i][j].imag = temp2[j].imag;
		}
	}

	free (temp);
	free (temp2);

	//cout << "Transformed channel...\n";
}

void IFFT2D(complexNum **c,int h,int w) {
	complexNum *temp, *temp2;

	temp = (complexNum*) malloc (h*sizeof(complexNum));
	temp2 = (complexNum*) malloc (h*sizeof(complexNum));

	//Error checking
	if (temp == NULL) {
		exit(0);
	}

	//Column transform first
	for (int j=0;j<w;j++) {
		for (int i=0;i<h;i++) {
			temp[i].real = c[i][j].real;
			temp[i].imag = c[i][j].imag;
		}

		IFFT (temp,temp2,h);

		for (int i=0;i<h;i++) {
			c[i][j].real = temp2[i].real / (float) h;
			c[i][j].imag = temp2[i].imag / (float) h;
		}
	}

	free (temp);
	free (temp2);

	temp = (complexNum*) malloc (w*sizeof(complexNum));
	temp2 = (complexNum*) malloc (w*sizeof(complexNum));

	//Error checking
	if (temp == NULL) {
		exit(0);
	}

	//Row transform next
	for (int i=0;i<h;i++) {
		for (int j=0;j<w;j++) {
			temp[j].real = c[i][j].real / (float) w;
			temp[j].imag = c[i][j].imag / (float) w;
		}

		IFFT (temp,temp2,w);

		for (int j=0;j<w;j++) {
			c[i][j].real = temp2[j].real;
			c[i][j].imag = temp2[j].imag;
		}
	}

	free (temp);
	free (temp2);

	//cout << "Transformed channel...\n";
}

void LPF(complexNum **array_2D, int height, int width, int D) {
	for(int i=0;i<height;i++) {
		for(int j=0;j<width;j++) {
			if(((i-height/2)*(i-height/2)+(j-width/2)*(j-width/2))<(D/2*D/2)) {
				array_2D[i][j].real = array_2D[i][j].real;
				array_2D[i][j].imag = array_2D[i][j].imag;
			}
			else {
				array_2D[i][j].real = 0;
				array_2D[i][j].imag = 0;
			}
		}
	}
}

void HPF(complexNum **array_2D, int height, int width, int D) {
	for(int i=0;i<height;i++) {
		for(int j=0;j<width;j++) {
			if ((i<(height/2+D/2)) && (i>(height/2-D/2)) && (j<(width/2+D/2)) && (j>(width/2-D/2))) {
			//if(((i-height/2)*(i-height/2)+(j-width/2)*(j-width/2))>(D/2*D/2)) {
				array_2D[i][j].real = 0;
				array_2D[i][j].imag = 0;
			}
			else {
				array_2D[i][j].real = array_2D[i][j].real;
				array_2D[i][j].imag = array_2D[i][j].imag;
			}
		}
	}
}

void Butterworth_LPF(complexNum **array_2D, int height, int width, int D) {
	for(int i=0;i<height;i++) {
		for(int j=0;j<width;j++) {
			array_2D[i][j].real = array_2D[i][j].real*(1/(1+pow(sqrt((float)(i-height/2)*(float)(i-height/2)+(float)(j-width/2)*(float)(j-width/2))/(float)D,2)));
			array_2D[i][j].imag = array_2D[i][j].imag*(1/(1+pow(sqrt((float)(i-height/2)*(float)(i-height/2)+(float)(j-width/2)*(float)(j-width/2))/(float)D,2)));
		}
	}
}

void Butterworth_HPF(complexNum **array_2D, int height, int width, int D) {
	for(int i=0;i<height;i++) {
		for(int j=0;j<width;j++) {
			array_2D[i][j].real = array_2D[i][j].real*(1/(1+pow((float)D/sqrt((float)(i-height/2)*(float)(i-height/2)+(float)(j-width/2)*(float)(j-width/2)),2)));
			array_2D[i][j].imag = array_2D[i][j].imag*(1/(1+pow((float)D/sqrt((float)(i-height/2)*(float)(i-height/2)+(float)(j-width/2)*(float)(j-width/2)),2)));
		}
	}
}

void Gaussian_LPF(complexNum **array_2D, int height, int width, int D) {
	for(int i=0;i<height;i++) {
		for(int j=0;j<width;j++) {
			array_2D[i][j].real = array_2D[i][j].real*exp(-1*(pow(sqrt((float)(i-height/2)*(float)(i-height/2)+(float)(j-width/2)*(float)(j-width/2))/(float)D,2)/2));
			array_2D[i][j].imag = array_2D[i][j].imag*exp(-1*(pow(sqrt((float)(i-height/2)*(float)(i-height/2)+(float)(j-width/2)*(float)(j-width/2))/(float)D,2)/2));
		}
	}
}

void Gaussian_HPF(complexNum **array_2D, int height, int width, int D) {
	for(int i=0; i<height; i++) {
		for(int j=0; j<width; j++) {
			array_2D[i][j].real = array_2D[i][j].real*(1-exp(-1*(pow(sqrt((float)(i-height/2)*(float)(i-height/2)+(float)(j-width/2)*(float)(j-width/2))/(float)D,2)/2)));
			array_2D[i][j].imag = array_2D[i][j].imag*(1-exp(-1*(pow(sqrt((float)(i-height/2)*(float)(i-height/2)+(float)(j-width/2)*(float)(j-width/2))/(float)D,2)/2)));
		}
	}
}