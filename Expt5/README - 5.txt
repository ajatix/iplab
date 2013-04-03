README

To run this program, enter the file parameters in the main function, type on the commandline
GeometricTransform.exe <scale> <scale_type> <angle> <rotation_type>

Scale_type allowed: 0 - Nearest neighbour, 1 - Bilinear, 2 -Bicubic

Make sure the images folder in the same parent directory as that of the codes.

Provide correct filenames, without spaces and with proper subdirectories

All our functions and bitmap structures are defined in the header ReadBitmap.h

Our functions used are:

IplImage* Scale_NN(IplImage *src, double Scale_Factor); //Performs NN scaling
IplImage* Scale_Bilinear(IplImage* src, double Scale_Factor); //Performs bilinear scaling
IplImage* Scale_Bicubic(IplImage *src, double Scale_Factor); //Performs bicubic scaling
double BSpline_Cubic(double val); //To calculate B-spline value
IplImage* Rot_NN(IplImage *src,double degree); //Performs NN rotation
IplImage* Rot_Bilinear(IplImage *src,double degree); //Performs Bilinear rotation

