README

To run this program, enter the file parameters in the main function, type on the commandline
OtsuSegmentation.exe <input image> <otsu output image> <labelled output image> <mask size for opening>

Make sure the images folder in the same parent directory as that of the codes.

Provide correct filenames, without spaces and with proper subdirectories

All our functions and bitmap structures are defined in the header HistogramEqualization.h

Our functions used are:

int getHistogramValues (int hist[256], IplImage *img);
float getCostFunction (IplImage *img, int hist[256], int totalNum, int index);
int getParams (float params[3], int weights[256], int start_index, int stop_index, int totalNum);
void OtsuThreshold (IplImage *img, IplImage *out, int threshold);
int DetectSegments (Mat binary, vector <vector<Point2i>> &segments);
void Erosion (vector<Mat> planes, int rows, int cols, int struct_type, int mask_size);
void Dilation (vector<Mat> planes, int rows, int cols, int struct_type, int mask_size);
