README

Set the image path in the main function in RegionGrowing.cpp

Make sure the images folder in the same parent directory as that of the codes.

Provide correct filenames, without spaces and with proper subdirectories

All our functions and bitmap structures are defined in the header HistogramEqualization.h

Our functions used are:

void plantSeeds (Mat img, IplImage *seeds, int thres, int val);
int *get8Neighbours (Mat img, int &length, int i, int j, int thres, int val);
void growSeed (Mat img, IplImage *seeds, int i, int j, int thres, int val);
void on_mouse (int evt, int x, int y, int flags, void *param);
int getImage(Mat img, Mat output, int x_pos, int y_pos);
void findBlobs(const Mat &binary, vector <vector<Point2i>> &blobs);
