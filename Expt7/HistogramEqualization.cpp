#include "HistogramEqualization.h"

int main(int argc, char *argv[]) {

	//Setting up images
	IplImage *img = cvLoadImage (argv[1]);
	IplImage *out = cvCreateImage (cvSize(img->width, img->height), img->depth, img->nChannels);

	EnhanceHistogram (img, out);

	//Setting up histograms
	IplImage *originalHist, *enhancedHist;

	originalHist = GenerateHistogram (img);
	enhancedHist = GenerateHistogram (out);

	//Display images
	cvNamedWindow ("Original image");
	cvNamedWindow ("Enhanced image");

	cvShowImage ("Original image", img);
	cvShowImage ("Enhanced image", out);

	//Display histograms
	cvNamedWindow ("Combined histogram of original image");
	cvNamedWindow ("Combined histogram of enhanced image");

	cvShowImage ("Combined histogram of original image", originalHist);
	cvShowImage ("Combined histogram of enhanced image", enhancedHist);

	cvWaitKey(0);

	//Saving stuff
	cvSaveImage (argv[2], out);
	cvSaveImage ("original_hist.bmp", originalHist);
	cvSaveImage ("enhanced_hist.bmp", enhancedHist);

	//Cleaning up
	cvReleaseImage(&img);
	cvReleaseImage(&out);
	cvReleaseImage(&originalHist);
	cvReleaseImage(&enhancedHist);

	cvDestroyAllWindows();

	return 0;
}

void EnhanceHistogram (IplImage *img, IplImage *out) {

	//Generating the arrays for calculating histograms
	float R_p_hist[LEVELS]={0.0}, R_sk[LEVELS]={0.0}, R_smin = 1; 
	int R_sk_mod[LEVELS]={0}; 
	float G_p_hist[LEVELS]={0.0}, G_sk[LEVELS]={0.0}, G_smin = 1; 
	int G_sk_mod[LEVELS]={0};
	float B_p_hist[LEVELS]={0.0}, B_sk[LEVELS]={0.0}, B_smin = 1; 
	int B_sk_mod[LEVELS]={0};

	int TOTAL = img->height * img->width;

	int ***array_3D;

	array_3D = new int**[img->height];

	for (int i=0; i<img->height; i++) {
		array_3D[i] = new int*[img->width];
		for (int j=0; j<img->width; j++) {
			array_3D [i][j] = new int[img->nChannels]; 
		}
	}

	CvScalar pixel;
	
	for (int row = 0; row < img->height; row++) {
		for (int col = 0; col < img->width; col++) {
			//Get pixel data
			pixel = cvGet2D (img, row, col);
			//Set array
			for (int channel = 0; channel < img->nChannels; channel++) {
				array_3D[row][col][channel] = pixel.val [channel];
			}
		}
	}

	//To update the bins
	for (int row = 0; row < img->height; row++)	{
		for (int col = 0; col < img->width; col++) {
			for (int channel = 0; channel < img->nChannels; channel++) {
				//array_3D[row][col][channel]
				if (channel == 0) {
					R_p_hist[array_3D[row][col][channel]]+=1; 
				}
				else if (channel == 1) {
					G_p_hist[array_3D[row][col][channel]]+=1; 
				}
				else if (channel == 2) {
					B_p_hist[array_3D[row][col][channel]]+=1; 
				}
			}
		}
	}

	//To normalize the values
	for(int i=0;i<LEVELS;i++) {
		R_p_hist[i] = R_p_hist[i]/TOTAL;
		G_p_hist[i] = G_p_hist[i]/TOTAL;
		B_p_hist[i] = B_p_hist[i]/TOTAL;
	}
	
	//The equalization algorithm
	for (int r = 0; r < LEVELS; r++ ) {
		if (r == 0) {
			R_sk[r] = R_p_hist[r];
			G_sk[r] = G_p_hist[r];
			B_sk[r] = B_p_hist[r];
		}
		else {
			R_sk[r] = R_p_hist[r] + R_sk[r-1];
			G_sk[r] = G_p_hist[r] + G_sk[r-1];
			B_sk[r] = B_p_hist[r] + B_sk[r-1];
		}

		if (R_sk[r] < R_smin) {
			R_smin = R_sk[r]; 
		}
		if (G_sk[r] < G_smin) {
			G_smin = G_sk[r];
		}
		if (B_sk[r] < B_smin) {
			B_smin = B_sk[r];
		}
	}
	
	for (int r = 0; r < LEVELS; r++ ) {
		R_sk_mod[r] = (int)(((R_sk[r] - R_smin)/(1 - R_smin)) * (LEVELS-1) + 0.5 );
		G_sk_mod[r] = (int)(((G_sk[r] - G_smin)/(1 - G_smin)) * (LEVELS-1) + 0.5 );
		B_sk_mod[r] = (int)(((B_sk[r] - B_smin)/(1 - B_smin)) * (LEVELS-1) + 0.5 );
	}

	//Updating changes to the output image	
	CvScalar pixel_out;
	
	for (int row = 0; row < img->height; row++) {
		for (int col = 0; col < img->width; col++) {
			// get pixel data
			pixel_out = cvGet2D (out, row, col);
			for (int channel = 0; channel < img->nChannels; channel++) {
				if (channel == 0) {
					pixel_out.val [channel] = R_sk_mod[array_3D[row][col][channel]];
				}
				else if (channel == 1) {
					pixel_out.val [channel] = G_sk_mod[array_3D[row][col][channel]];
				}
				else if (channel == 2) {
					pixel_out.val [channel] = B_sk_mod[array_3D[row][col][channel]];
				}
			}
			cvSet2D (out, row, col, pixel_out);
		}
	}

	//Cleaning up
	free (array_3D);
}

IplImage *GenerateHistogram (IplImage *img) {

	//Histogram params
	int numBins = 256;
	float range[] = {0, 255};
	float *ranges[] = {range};

	CvHistogram *hist = cvCreateHist (1, &numBins, CV_HIST_ARRAY, ranges, 1);
	cvClearHist (hist);

	IplImage *imgRed = cvCreateImage (cvGetSize(img), 8, 1);
	IplImage *imgGreen = cvCreateImage (cvGetSize(img), 8, 1);
	IplImage *imgBlue = cvCreateImage (cvGetSize(img), 8, 1);

	cvSplit (img, imgBlue, imgGreen, imgRed, NULL);

	//Rendering histograms
	IplImage *imgCombinedHistogram = cvCreateImage (cvSize(256*1, 64*3), 8, 3);

	cvCalcHist (&imgRed, hist, 0, 0);
	IplImage *imgHistRed = DisplayHistogram (hist);
	cvClearHist (hist);

	cvCalcHist (&imgGreen, hist, 0, 0);
	IplImage *imgHistGreen = DisplayHistogram (hist);
	cvClearHist (hist);

	cvCalcHist (&imgBlue, hist, 0, 0);
	IplImage *imgHistBlue = DisplayHistogram (hist);
	cvClearHist (hist);

	cvMerge (imgHistBlue, imgHistGreen, imgHistRed, NULL, imgCombinedHistogram);

	//Cleaning up
	cvReleaseImage (&imgRed);
	cvReleaseImage (&imgGreen);
	cvReleaseImage (&imgBlue);
	cvReleaseImage (&imgHistRed);
	cvReleaseImage (&imgHistGreen);
	cvReleaseImage (&imgHistBlue);

	return imgCombinedHistogram;
}

IplImage* DisplayHistogram(CvHistogram *hist, float scaleX, float scaleY) {

	//To figure out the range of values
	float histMax = 0;
	cvGetMinMaxHistValue (hist, 0, &histMax, 0, 0);
	
	IplImage *imgHist = cvCreateImage (cvSize(256*scaleX, 64*scaleY), 8, 1);

	for (int i=0; i<255; i++) {
		float histValue = cvQueryHistValue_1D (hist, i);
		float nextValue = cvQueryHistValue_1D (hist, i+1);

		CvPoint pt1 = cvPoint(i*scaleX, 64*scaleY);
        CvPoint pt2 = cvPoint(i*scaleX+scaleX, 64*scaleY);
        CvPoint pt3 = cvPoint(i*scaleX+scaleX, (64-nextValue*64/histMax)*scaleY);
        CvPoint pt4 = cvPoint(i*scaleX, (64-histValue*64/histMax)*scaleY);
 
        int numPts = 5;
        CvPoint pts[] = {pt1, pt2, pt3, pt4, pt1};
 
        cvFillConvexPoly(imgHist, pts, numPts, cvScalar(255), CV_AA);
	}

	return imgHist;
}