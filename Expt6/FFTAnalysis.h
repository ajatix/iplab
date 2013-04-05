#pragma once

#ifndef _EXPT6_H_
#define _EXPT6_H_

#include "opencv2\opencv.hpp"
#include <stdio.h>
#include <iostream>
#include <cmath>

#define PI 3.1415

typedef struct {
	double real;
	double imag;
} complexNum;

typedef struct {
	complexNum **data;
} complexChannel;

typedef struct {
	complexChannel channel[3];
} complexImg;

void FFT (complexNum *x, complexNum *y, int N);
void IFFT (complexNum *y,complexNum *x,int N);
void FFT2D(complexNum **c,int h,int w);
void IFFT2D(complexNum **c,int h,int w);

void LPF(complexNum **array_2D, int ht, int wd, int D);
void HPF(complexNum **array_2D, int ht, int wd, int D);
void Butterworth_LPF(complexNum **array_2D, int ht, int wd, int D);
void Butterworth_HPF(complexNum **array_2D, int ht, int wd, int D);
void Gaussian_LPF(complexNum **array_2D, int ht, int wd, int D);
void Gaussian_HPF(complexNum **array_2D, int ht, int wd, int D);

#endif