#include "RegionGrowing.h"

using namespace cv;
using namespace std;

static int iters;
Mat done;

//For trackbar
int val = 10;
int const max_sensitivity = 100;
char *window_name = "Region growing";
char *trackbar_name = "Sensitivity";

void growSeed (Mat img, IplImage *seeds, int i, int j, int thres, int val);
int GetImage(cv::Mat img, cv::Mat output, int x_pos, int y_pos);

void plantSeeds (Mat img, IplImage *seeds, int thres, int val) {
	int pixel;

	for (int i=0; i<img.rows; i++) {
		for (int j=0; j<img.cols; j++) {
			pixel = img.data[i*img.cols + j];
			if (abs((int)pixel - thres) < val/2) {
				cvSet2D (seeds, i, j, cvScalar(255));
				iters = 0;
				growSeed (img, seeds, i, j, val, (int)pixel);
			}
		}
	}
}

int *get8Neighbours (Mat img, int &length, int i, int j, int thres, int val) {
	int *n;
	Mat n_mat;

	int rows = img.rows;
	int cols = img.cols;
	
	if ((i<1) || (j<1) || (i>rows-3) || (j>cols-3)) { //Assert if i,j within limits
		return 0;
	}

	if (i) { //If not first row
		if (j) { //If not first column
			//Get all 8 elements - 3x3
			n = new int[9];
			n_mat = img (Range(i-1, i+2), Range(j-1, j+2)); //Creates a matrix containing the adjacent elements
			for (int p=0; p<9; p++) {
				if (abs(n_mat.data[p] - val) < thres) {
					if (done.data[(i-1 + p/3) * done.cols + (j-1+p%3)] == 0) {
						n[p] = 1;
					}
				}
				else {
					n[p] = 0;
				}
			}

			length = 9;
			n_mat.release();
			return n;
		}
		else {
			//6 elements - 3x2
			n = new int[6];
			n_mat = img (Range(i-1, i+2), Range(j-1, j+1)); //Creates a matrix containing the adjacent elements
			for (int p=0; p<6; p++) {
				if (abs(n_mat.data[p] - val) < thres) {
					if (done.data[(i-1 + p/3) * done.cols + (j-1+p%2)] == 0) {
						n[p] = 1;
					}
				}
				else {
					n[p] = 0;
				}
			}

			length = 6;
			n_mat.release();
			return n;
		}
	}
	else {
		if (j) {
			//6 elements - 2x3
			n = new int[6];
			n_mat = img (Range(i-1, i+1), Range(j-1, j+2)); //Creates a matrix containing the adjacent elements
			for (int p=0; p<6; p++) {
				if (abs(n_mat.data[p] - val) < thres) {
					if (done.data[(i-1 + p/2) * done.cols + (j-1+p%3)] == 0) {
						n[p] = 1;
					}
				}
				else {
					n[p] = 0;
				}
			}

			length = 6;
			n_mat.release();
			return n;
		}
		else {
			//4 elements - 2x2
			n = new int[4];
			n_mat = img (Range(i-1, i+1), Range(j-1, j+1)); //Creates a matrix containing the adjacent elements
			for (int p=0; p<4; p++) {
				if (abs(n_mat.data[p] - val) < thres) {
					if (done.data[(i-1 + p/2) * done.cols + (j-1+p%2)] == 0) {
						n[p] = 1;
					}
				}
				else {
					n[p] = 0;
				}
			}

			length = 4;
			n_mat.release();
			return n;
		}
	}
}

void growSeed (Mat img, IplImage *seeds, int i, int j, int thres, int val) {
	int *neighbours = 0;
	int *p = 0;
	int i_new, j_new;
	int length; //Length of the neighbours array

	neighbours = get8Neighbours (img, length, i, j, thres, val); //Fills neighbours array with membership values
	p = new int;

	for (*p=0; *p<length; *p = *p + 1) {
		if (neighbours[*p] == 1) {
			i_new = i-1 + (*p)/3;
			j_new = j-1 + (*p)%3;
			cvSet2D (seeds, i_new, j_new, cvScalar(255));
			done.data[i_new * done.cols + j_new] = (int)255;
			if (!i_new || !j_new) {
				return;
			}
			iters ++;
			if (iters > ITER_MAX) {
				return;
			}
			if ((i-i_new) || (j-j_new)) {
				growSeed (img, seeds, i_new, j_new, thres, val);
			}
			else {
				return;
			}
		}
	}

	free (neighbours);
	free (p);
}

void on_mouse (int evt, int x, int y, int flags, void *param) {
	IplImage *img = (IplImage*) param;
	IplImage *seeds = cvCreateImage (cvSize(img->width, img->height), 8, 1);
	cvSet(seeds, cvScalar(0)); //Setting the output image to zero

	Mat image (img); //Converting to Mat
	done = Mat::zeros(image.rows, image.cols, CV_8UC1);

	CvScalar pixel;

	if (evt == CV_EVENT_LBUTTONDOWN) {
		printf ("Clicked at (%d, %d)\n", y, x);
		pixel = cvGet2D (img, y, x);
		printf ("The pixel value is %d\n", (int)pixel.val[0]);
		plantSeeds (image, seeds, (int)pixel.val[0], val);
		//iters = 0;
		//growSeed (image, seeds, y, x, 10, (int)pixel.val[0]);
		printf ("Done growing seeds\n");
		//cvShowImage ("Seeds", seeds);

		Mat seed (seeds);
		Mat blend, extract;

		Mat output = Mat::zeros(image.size(), CV_8UC3);
		getImage (seed, output, x, y); //Returns the selected blob from all the grown ones

		vector<Mat> planes;
		Mat overlay;

		split (output, planes);

		int rows = image.rows;
		int cols = image.cols;

		overlay = Mat::zeros(rows, cols, CV_8UC1);
		planes[0].copyTo(overlay);

		double alpha = 0.25;
		double beta = ( 1.0 - alpha );
		addWeighted( image, alpha, overlay, beta, 0.0, blend);

		bitwise_and (image, overlay, extract);
		
		namedWindow ("Blend");
		namedWindow ("Extract");
		imshow ("Blend", blend);
		imshow ("Extract", extract);

		imwrite ("blend.png", blend);
		imwrite ("extract.png", extract);
	}
}

int main() {
	IplImage *img, *draw;
	img = cvLoadImage ("images/coins.bmp", 0);
	draw = cvCreateImage (cvSize(img->width, img->height), img->depth, img->nChannels);
	cvSet(draw, cvScalar(255)); //Setting the output image to zero

	namedWindow (window_name, CV_WINDOW_AUTOSIZE);

	//Adding trackbars
	createTrackbar (trackbar_name, window_name, &val, max_sensitivity, NULL);

	cvSetMouseCallback (window_name, on_mouse, (void *)img);
	cvShowImage (window_name, img);

	cvWaitKey(0);

	cvReleaseImage (&img);
	cvDestroyAllWindows ();

	return 0;
}
