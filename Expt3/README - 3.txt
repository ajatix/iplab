README

To run this program, enter the file parameters in the main function, 
WaveletTransforms char * input_directory, char * output_directory);

Make sure the images folder in the same parent directory as that of the codes.

Provide correct filenames, without spaces and with proper subdirectories

All our functions and bitmap structures are defined in the header ReadBitmap.h

Our functions used are:

int WaveletTransforms(char *input_filename, char *output_filename); //This is the main function
void ForwardTransform (vector<Mat> planes, int rows, int cols, int type); //For deconstructing the image
void BackwardTransform (vector<Mat> planes, int rows, int cols, int type); //For recovering the image
void Forward1D (uchar *vec, int N, int L, int type); //The function for 1D forward transform
void Backward1D (uchar *vec, int N, int L, int type); //The function for 1D backward transform
float CalculatePSNR (vector<Mat> planes, int rows, int cols); //To calculate the PSNR