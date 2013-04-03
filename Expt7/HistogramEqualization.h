#pragma once

#ifndef _EXPT7_H_
#define _EXPT7_H_

#include "opencv2\opencv.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <conio.h>

using namespace cv;
using namespace std;

#define cvQueryHistValue_1D( hist, idx0 ) \
    cvGetReal1D( (hist)->bins, (idx0) )

#define LEVELS 256

void EnhanceHistogram (IplImage *img, IplImage *out); //Enhances the image
IplImage *GenerateHistogram (IplImage *img); //Returns a histogram graph
IplImage *DisplayHistogram (CvHistogram *hist, float scaleX=1, float scaleY=3);

#endif