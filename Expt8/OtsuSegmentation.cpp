#include "OtsuSegmentation.h"
#include "Morphology.h"

using namespace cv;
using namespace std;

int main(int argc, char *argv[]) {

	//Setting up
	IplImage *img, *out;
	img = cvLoadImage (argv[1], 0);
	out = cvCreateImage (cvSize(img->width, img->height), img->depth, img->nChannels);
	cvSet(out, cvScalar(0)); //Setting the output image to zero

	//Initializing histogram parameters
	int hist[256] = {0};
	int totalNum;
	float params[3];
	int start;

	totalNum = getHistogramValues (hist, img); //Returns the total number of pixels in the image, while updating the values in the hist array
	getParams (params, hist, 0, 255, totalNum); //Gets the mean, variance values of background and foreground

	start = (int)params[1];

	cout << "Weighted average point is " << start << endl;

	//float min = (float)INT_MAX;
	float min = getCostFunction (img, hist, totalNum, start);
	float temp;
	int index = start;
	int it1 = 0, it2 = 0;

	//Forward search
	for (int i=start + 1; i<255; i++) {
		temp = getCostFunction (img, hist, totalNum, i);
		it1++;
		if (temp < min ) {
			min = temp;
			index = i;
		}
		else {
			break;
		}
	}

	//Reverse search
	for (int i=start - 1; i>0; i--) {
		temp = getCostFunction (img, hist, totalNum, i);
		it2++;
		if (temp < min ) {
			min = temp;
			index = i;
		}
		else {
			break;
		}
	}

	cout << "Index is " << index << endl;
	cout << "Forward " << it1 << endl << "Reverse " << it2 << endl;

	OtsuThreshold (img, out, index);

	//Display images
	cvNamedWindow ("Original image");
	cvNamedWindow ("Otsu image");

	cvShowImage ("Original image", img);
	cvShowImage ("Otsu image", out);

	//Saving Otsu output
	cvSaveImage (argv[2], out);

	//Counting connected components
	Mat binary(out); //Typecasting to Mat
	Mat output = Mat::zeros(binary.size(), CV_8UC3);
	vector <vector<Point2i>> segments;

	int rows = binary.rows;
	int cols = binary.cols;

	//Morphological transform
	vector<Mat> planes; //Splitting image into planes so that we can process images of any depth
	split (binary, planes); //For handling multi-channel images

	int mask = atoi(argv[4]);

	Erosion (planes, rows, cols, 0, mask);
	Dilation (planes, rows, cols, 0, mask);

	merge (planes, binary); //Merging the planes to form a final image

	cout << "The number of segments in the image is " << DetectSegments (binary, segments) << endl;

    // Randomy color the segments
    for(size_t i=0; i < segments.size(); i++) {
        unsigned char r = 255 * (rand()/(1.0 + RAND_MAX));
        unsigned char g = 255 * (rand()/(1.0 + RAND_MAX));
        unsigned char b = 255 * (rand()/(1.0 + RAND_MAX));

        for(size_t j=0; j < segments[i].size(); j++) {
            int x = segments[i][j].x;
            int y = segments[i][j].y;

            output.at<Vec3b>(y,x)[0] = b;
            output.at<Vec3b>(y,x)[1] = g;
            output.at<Vec3b>(y,x)[2] = r;
        }
    }

	//Display image
	namedWindow ("Labelled image");
	imshow ("Labelled image", output);

	cvWaitKey(0);

	//Saving labelled output
	imwrite (argv[3], output);

	//Cleaning up
	cvReleaseImage(&img);
	cvReleaseImage(&out);

	cvDestroyAllWindows();

	return 0;
}

int getHistogramValues (int hist[256], IplImage *img) {
	
	int rows = img->height;
	int cols = img->width;

	int totalNum = rows*cols;

	CvScalar pixel;

	//Generating the histogram
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			pixel = cvGet2D (img, i, j);
			hist[(int)pixel.val[0]]++;
		}
	}

	return totalNum;
}

float getCostFunction (IplImage *img, int hist[256], int totalNum, int index) {

	//Getting the histogram weights, mean and varianc values
	float bgParams[3], fgParams[3];
	getParams (bgParams, hist, 0, index, totalNum); 
	getParams (fgParams, hist, index + 1, 255, totalNum); 

	float cost;
	float num, den;
	//Calculating the cost for each index
	num = (bgParams[0]*bgParams[2] + fgParams[0]*fgParams[2]);
	den = (bgParams[0]*fgParams[0]*(bgParams[1]-fgParams[1])*(bgParams[1]-fgParams[1]));

	if (den != 0) {
		cost = num/den;
	}
	else {
		cost = (float)INT_MAX;
	}

	return cost;
}

int getParams (float params[3], int weights[256], int start_index, int stop_index, int totalNum) {

	float sum = 0.0;
	int num = 0;
	
	//Looping to count the number of pixels and weighted values of pixels in the given range of indices
	for (int i=start_index; i<stop_index; i++) {
		num += weights[i];
		sum += (i*weights[i]);
	}

	params[0] = (float)num/totalNum;
	params[1] = (float)sum/num;

	sum = 0.0;

	//Calculating the variance based on the mean value
	for (int i=start_index; i<stop_index; i++) {
		sum += ((float)(i-params[1])*(i-params[1])*weights[i]);
	}

	params[2] = (float)sum/num;

	//To indicate correct output
	return 0;
}

void OtsuThreshold (IplImage *img, IplImage *out, int threshold) {

	int rows = img->height;
	int cols = img->width;

	CvScalar pixel;

	//Thresholding the image
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			pixel = cvGet2D (img, i, j);
			if (pixel.val[0] > threshold) {
				cvSet2D (out, i, j, cvScalar(255));
			}
		}
	}
}

int DetectSegments (Mat binary, vector <vector<Point2i>> &segments) {
    segments.clear();

    // Fill the label_image with the segments
    // 0  - background
    // 1  - unlabelled foreground
    // 2+ - labelled foreground

    Mat label_image;
    binary.convertTo(label_image, CV_32FC1); // weird it doesn't support CV_32S!

    int label_count = 2; // starts at 2 because 0,1 are used already

    for(int y=0; y < binary.rows; y++) {
        for(int x=0; x < binary.cols; x++) {
            if((int)label_image.at<float>(y,x) != 255) {
                continue;
            }

            Rect rect;
            floodFill(label_image, Point(x,y), Scalar(label_count), &rect, Scalar(0), Scalar(0), 4);

            vector <Point2i> blob;

            for(int i=rect.y; i < (rect.y+rect.height); i++) {
                for(int j=rect.x; j < (rect.x+rect.width); j++) {
                    if((int)label_image.at<float>(i,j) != label_count) {
                        continue;
                    }

                    blob.push_back(Point2i(j,i));
                }
            }

            segments.push_back(blob);

            label_count++;
        }
    }

	return (label_count-2);
}

void Erosion (vector<Mat> planes, int rows, int cols, int struct_type, int mask_size) {

#pragma region kernel_generate

	Mat kernel; //Kernel 
	int row_shift, col_shift; //Indicates the position offset for the current pixel while accessing element

	//To generate appropriate kernel
	switch (struct_type) { 
	case 0: kernel = Mat::ones(mask_size,mask_size,CV_8U); //Makes a kernel matrix of 255's
		row_shift = kernel.rows/2;
		col_shift = kernel.cols/2;
		break;
	case 1: kernel = Mat::ones(mask_size,mask_size,CV_8U); //Makes a diamond shaped kernel matrix of 255's
		row_shift = kernel.rows/2;
		col_shift = kernel.cols/2;
		for (int a=0; a<(kernel.rows)/2; a++) {
			for (int b=0; b<(kernel.cols/2-a); b++) {
				kernel.data[a*kernel.cols + b] = 0;
				kernel.data[a*kernel.cols + (kernel.cols-b-1)] = 0;
				kernel.data[(kernel.rows-a-1)*kernel.cols + b] = 0;
				kernel.data[(kernel.rows-a-1)*kernel.cols + (kernel.cols-b-1)] = 0;
			}
		}
		break;
	case 2: kernel = Mat::ones(1,mask_size,CV_8U); //Makes a row matrix of 255's
		row_shift = kernel.rows/2;
		col_shift = 0;
		break;
	default: cout << "Wrong choice!!!\n";
		return;
	}

#pragma endregion kernel_generate
	
#pragma region convolution

	Mat temp_img, prod_img, morphed; //To store temp MxM mat from image, prod of mat with kernel, the final morphed image mat
	int min;

	for (int c=0; c<(int)planes.size(); c++) { //Goes through every channel

		morphed = Mat::zeros(rows, cols, CV_8U); //Creates an empty temporary Mat element of 1's

		for (int i=0; i<rows-mask_size+1; i++) { //Goes upto which a mask can be created
			for (int j=0; j<cols-mask_size+1; j++) {
				temp_img = planes[c](Range(i,i+kernel.rows), Range(j,j+kernel.cols)); //Creates temporary mask
				multiply (kernel, temp_img, prod_img); //Element-wise multiplication with kernel matrix
				min = 255;
				for (int k=0; k<kernel.rows*kernel.cols; k++) {
					if ((prod_img.data[k] < min) && (kernel.data[k])) {
						min = prod_img.data[k];
					}
				}
				morphed.data[cols*(i+row_shift) + (j+col_shift)] = min;
			}
		}
		morphed.copyTo(planes[c]); //Relays output back to called function
	}

#pragma endregion convolution

}

void Dilation (vector<Mat> planes, int rows, int cols, int struct_type, int mask_size) {

#pragma region kernel_generate

	Mat kernel; //Kernel 
	int row_shift, col_shift; //Indicates the position offset for the current pixel while accessing element

	//To generate appropriate kernel
	switch (struct_type) { 
	case 0: kernel = Mat::ones(mask_size,mask_size,CV_8U); //Makes a kernel matrix of 255's
		row_shift = kernel.rows/2;
		col_shift = kernel.cols/2;
		break;
	case 1: kernel = Mat::ones(mask_size,mask_size,CV_8U); //Makes a diamond shaped kernel matrix of 255's
		row_shift = kernel.rows/2;
		col_shift = kernel.cols/2;
		for (int a=0; a<(kernel.rows)/2; a++) {
			for (int b=0; b<(kernel.cols/2-a); b++) {
				kernel.data[a*kernel.cols + b] = 0;
				kernel.data[a*kernel.cols + (kernel.cols-b-1)] = 0;
				kernel.data[(kernel.rows-a-1)*kernel.cols + b] = 0;
				kernel.data[(kernel.rows-a-1)*kernel.cols + (kernel.cols-b-1)] = 0;
			}
		}
		break;
	case 2: kernel = Mat::ones(1,mask_size,CV_8U); //Makes a row matrix of 255's
		row_shift = kernel.rows/2;
		col_shift = 0;
		break;
	default: cout << "Wrong choice!!!\n";
		return;
	}

#pragma endregion kernel_generate
	
#pragma region convolution

	Mat temp_img, prod_img, morphed; //To store temp MxM mat from image, prod of mat with kernel, the final morphed image mat
	int max;

	for (int c=0; c<(int)planes.size(); c++) { //Goes through every channel

		//morphed = Mat::ones(rows-(2*mask_size-4), cols-(2*mask_size-4), CV_8U)*255; //Creates an empty temporary Mat element of 1's for Dilation
		//copyMakeBorder(morphed, morphed, mask_size-2, mask_size-2, mask_size-2, mask_size-2, BORDER_CONSTANT, 0); //Pad with zeros for the border
		morphed = Mat::zeros(rows, cols, CV_8U); //Creates an empty temporary Mat element of 1's

		for (int i=0; i<rows-mask_size+1; i++) { //Goes upto which a mask can be created
			for (int j=0; j<cols-mask_size+1; j++) {
				temp_img = planes[c](Range(i,i+kernel.rows), Range(j,j+kernel.cols)); //Creates temporary mask
				multiply (kernel, temp_img, prod_img); //Element-wise multiplication with kernel matrix
				max = 0;
				for (int k=0; k<kernel.rows*kernel.cols; k++) {
					if ((prod_img.data[k] > max) && (kernel.data[k])) {
						max = prod_img.data[k];
					}
				}
				morphed.data[cols*(i+row_shift) + (j+col_shift)] = max;
			}
		}
		morphed.copyTo(planes[c]); //Relays output back to called function
	}

#pragma endregion convolution

}
