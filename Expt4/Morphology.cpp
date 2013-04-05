#include "Morphology.h"

//For type trackbar
int morph_type = 0;
int const max_morph_type = 6;

//For structuring element trackbar
int struct_type = 0;
int const max_struct_type = 2;

//For size trackbar
int mask_size = 0;
int const max_mask_size = 9;

char *window_name = "Morphological transforms";
char *trackbar_morph_type = "Type";
char *trackbar_struct_type = "Structuring element";
char *trackbar_mask_size = "Mask size";

int MorphologicalTransforms (char *input_filename, char *output_filename) {

#pragma region setting_up

	Mat img = imread(input_filename); //Loading image
	Mat transformed(img.rows, img.cols, img.depth(), img.channels()); //Creating new matrices for transformed, recovered and difference images
	
	vector<Mat> planes; //Splitting image into planes so that we can process images of any depth
	
	int rows = img.rows;
	int cols = img.cols;

	namedWindow (window_name, CV_WINDOW_AUTOSIZE);

	//Adding trackbars
	createTrackbar (trackbar_morph_type, window_name, &morph_type, max_morph_type, NULL);
	createTrackbar (trackbar_struct_type, window_name, &struct_type, max_struct_type, NULL);
	createTrackbar (trackbar_mask_size, window_name, &mask_size, max_mask_size, NULL);

	namedWindow ("Original", CV_WINDOW_AUTOSIZE);
	imshow ("Original", img);

#pragma endregion setting_up

	while (1) {
		split (img, planes); //For handling multi-channel images
		planes = Transform (planes, rows, cols, morph_type, struct_type, mask_size); //The main control function
		merge (planes, transformed); //Merging the planes to form a final image
		imshow (window_name, transformed);

		imwrite (output_filename, transformed); //Saving the output image

		int c = waitKey(0);
		if (c == 27) { //Stop with escape key
			destroyAllWindows();
			break;
		}
	}

	return 0;
}

vector<Mat> Transform (vector<Mat> planes, int rows, int cols, int operation_type, int struct_type, int mask_size) {

	int mask = (3+2*mask_size); //Because the trackbar starts from 0
	vector<Mat> original(planes.size()), src(planes.size()); //Defining them to be the size of the original planes
	original = CopyVector (planes); //Storing a backup of the original copy of the image

#pragma region switch_control

	switch(operation_type) {
	case 0: // Erosion
		cout << "Performing EROSION of the image\n";
		Erosion (planes, rows, cols, struct_type, mask);
		cout << "Success!!!\n";
		break;
	case 1: // Dilation
		cout << "Performing DILATION of the image\n";
		Dilation (planes, rows, cols, struct_type, mask);
		cout << "Success!!!\n";
		break;
	case 2: //Opening
		cout << "Performing OPENING of the image\n";
		Erosion (planes, rows, cols, struct_type, mask);
		Dilation (planes, rows, cols, struct_type, mask);
		cout << "Success!!!\n";
		break;
	case 3: //Closing
		cout << "Performing CLOSING of the image\n";
		Dilation (planes, rows, cols, struct_type, mask);
		Erosion (planes, rows, cols, struct_type, mask);
		cout << "Success!!!\n";
		break;
	case 4: //Edge detection
		cout << "Performing EDGE DETECTION of the image\n";
		Dilation (planes, rows, cols, struct_type, mask);
		src = CopyVector (planes); //Dilated image
		planes = SubtractOperation(src, original);
		cout << "Success!!!\n";
		break;
	case 5: //Black Top-hat operation
		cout << "Performing BLACK TOP HAT transform of the image\n";
		Dilation (planes, rows, cols, struct_type, mask);
		Erosion (planes, rows, cols, struct_type, mask);
		src = CopyVector (planes); //Opened image
		//planes = CopyVector (original, planes);
		planes = SubtractOperation(src, original);
		cout << "Success!!!\n";
		break;
	case 6: //White Top-hat operation
		cout << "Performing WHITE TOP HAT transform of the image\n";
		Erosion (planes, rows, cols, struct_type, mask);
		Dilation (planes, rows, cols, struct_type, mask);
		src = CopyVector (planes); //Opened image
		//planes = CopyVector (original, planes);
		planes = SubtractOperation(original, src);
		cout << "Success!!!\n";
		break;
	default: cout << "Wrong operation!!!\n";
	}

#pragma endregion switch_control

	return planes;
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

vector<Mat> SubtractOperation (vector<Mat> src1, vector<Mat> src2) {

	vector<Mat> dst(src1.size());
	int rows = src1[0].rows, cols = src1[0].cols;

	for (int c=0; c<(int)src1.size(); c++) { //Goes through every channel
		dst[c] = Mat::zeros(rows, cols, CV_8U);
		for (int i=0; i<rows*cols; i++) {
				dst[c].data[i] = (int)((int)src1[c].data[i] - (int)src2[c].data[i]);
		}
	}

	return dst;
}

vector<Mat> CopyVector (vector<Mat> src) {

	vector<Mat> dst(src.size());

	for (int c=0; c<(int)src.size(); c++) {
		dst[c] = src[c].clone();
	}

	return dst;
}
