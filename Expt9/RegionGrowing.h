#pragma once

#ifndef _EXPT9_H_
#define _EXPT9_H_

#include "opencv2\opencv.hpp"
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

#define ITER_MAX 1000

void plantSeeds (Mat img, IplImage *seeds, int thres, int val);
int *get8Neighbours (Mat img, int &length, int i, int j, int thres, int val);
void growSeed (Mat img, IplImage *seeds, int i, int j, int thres, int val);
void on_mouse (int evt, int x, int y, int flags, void *param);
int getImage(Mat img, Mat output, int x_pos, int y_pos);
void findBlobs(const Mat &binary, vector <vector<Point2i>> &blobs);

#endif