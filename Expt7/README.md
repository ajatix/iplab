README

To run this program, enter the file parameters in the main function, type on the commandline
HistogramEqualization.exe <input image> <output image>

Make sure the images folder in the same parent directory as that of the codes.

Provide correct filenames, without spaces and with proper subdirectories

All our functions and bitmap structures are defined in the header HistogramEqualization.h

Our functions used are:

void EnhanceHistogram (IplImage *img, IplImage *out); //Enhances the image
IplImage *GenerateHistogram (IplImage *img); //Returns a histogram graph
IplImage *DisplayHistogram (CvHistogram *hist, float scaleX=3, float scaleY=3);