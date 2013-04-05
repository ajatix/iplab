#pragma once

#ifndef _EXPT5_H_
#define _EXPT5_H_

#include "opencv2\opencv.hpp"
#include <stdio.h>

#define PI 3.1415

IplImage* Scale_NN(IplImage *src, double Scale_Factor); //Performs NN scaling
IplImage* Scale_Bilinear(IplImage* src, double Scale_Factor); //Performs bilinear scaling
IplImage* Scale_Bicubic(IplImage *src, double Scale_Factor); //Performs bicubic scaling
double BSpline_Cubic(double val); //To calculate B-spline value
IplImage* Rot_NN(IplImage *src,double degree); //Performs NN rotation
IplImage* Rot_Bilinear(IplImage *src,double degree); //Performs Bilinear rotation

#endif

