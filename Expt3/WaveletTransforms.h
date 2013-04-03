#pragma once

#ifndef _EXPT3_H_
#define _EXPT3_H_

#include <iostream>
#include <cmath>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>

using namespace std;
using namespace cv;

int WaveletTransforms(char *input_filename, char *output_filename); //This is the main function
void ForwardTransform (vector<Mat> planes, int rows, int cols, int type); //For deconstructing the image
void BackwardTransform (vector<Mat> planes, int rows, int cols, int type); //For recovering the image
void Forward1D (uchar *vec, int N, int L, int type); //The function for 1D forward transform
void Backward1D (uchar *vec, int N, int L, int type); //The function for 1D backward transform
float CalculatePSNR (vector<Mat> planes, int rows, int cols); //To calculate the PSNR

#endif