README

To run this program, enter the file parameters in the command line
<input file> <output file> <filter type> <mask size> <color/gray> <order>

Make sure the images folder in the same parent directory as that of the codes.

Provide correct filenames, without spaces and with proper subdirectories

All our functions and bitmap structures are defined in the header ReadBitmap.h

Our functions used are:

//To handle fourier transforms
void FFT (complexNum *x, complexNum *y, int N);
void IFFT (complexNum *y,complexNum *x,int N);
void FFT2D(complexNum **c,int h,int w);
void IFFT2D(complexNum **c,int h,int w);

//To handle filter types
void LPF(complexNum **array_2D, int ht, int wd, int D);
void HPF(complexNum **array_2D, int ht, int wd, int D);
void Butterworth_LPF(complexNum **array_2D, int ht, int wd, int D);
void Butterworth_HPF(complexNum **array_2D, int ht, int wd, int D);
void Gaussian_LPF(complexNum **array_2D, int ht, int wd, int D);
void Gaussian_HPF(complexNum **array_2D, int ht, int wd, int D);