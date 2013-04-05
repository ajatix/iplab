#pragma once

#ifndef _EXPT4_H_
#define _EXPT4_H_

#include "opencv2\opencv.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

int MorphologicalTransforms (char *input_filename, char *output_filename);
vector<Mat> Transform (vector<Mat> planes, int rows, int cols, int operation_type, int struct_type, int mask_size);
void Erosion (vector<Mat> planes, int rows, int cols, int struct_type, int mask_size);
void Dilation (vector<Mat> planes, int rows, int cols, int struct_type, int mask_size);
vector<Mat> SubtractOperation (vector<Mat> src1, vector<Mat> src2);
vector<Mat> CopyVector (vector<Mat> src);
int CountConnected (Mat img, int rows, int cols);

#endif

