#pragma once

#ifndef _EXPT8_H_
#define _EXPT8_H_

#include "opencv2\opencv.hpp"
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

typedef struct {
	int x, y; //The pixel coordinates
} point;

int getHistogramValues (int hist[256], IplImage *img);
float getCostFunction (IplImage *img, int hist[256], int totalNum, int index);
int getParams (float params[3], int weights[256], int start_index, int stop_index, int totalNum);
void OtsuThreshold (IplImage *img, IplImage *out, int threshold);
int DetectSegments (Mat binary, vector <vector<Point2i>> &segments);
void Erosion (vector<Mat> planes, int rows, int cols, int struct_type, int mask_size);
void Dilation (vector<Mat> planes, int rows, int cols, int struct_type, int mask_size);

#endif