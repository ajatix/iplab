README

To run this program, enter the file parameters in the main function, 
ReadBitmap (const char * input_directory, const char * output_directory);

Make sure the images folder in the same parent directory as that of the codes.

Provide correct filenames, without spaces and with proper subdirectories

All our functions and bitmap structures are defined in the header ReadBitmap.h

Our functions used are:

void meanFilter (IplImage *img, IplImage *out, int size) - Mean filter
void medianFilter (IplImage *img, IplImage *out) - Median filter
void gradientFilter (IplImage *img, IplImage *out) - Gradient filter
void laplacianFilter (IplImage *img, IplImage *out) - Laplacian filter
void sobelFilter (IplImage *img, IplImage *out, int size) - Sobel filter
int SpatialFilter (char* input_textfilename, char* output_filename) - Function for processing one image
void Filter_control (int, void *) - Trackbar control function
int MainSpatialFilterControl (char* input_directory, char* output_directory) - Main function