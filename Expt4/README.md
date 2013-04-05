README

To run this program, enter the file parameters in the main function, 
WaveletTransforms char * input_directory, char * output_directory);

Make sure the images folder in the same parent directory as that of the codes.

Provide correct filenames, without spaces and with proper subdirectories

All our functions and bitmap structures are defined in the header ReadBitmap.h

Our functions used are:

int MorphologicalTransforms (char *input_filename, char *output_filename); //This is the main function for the program. It initializes the matrices and vectors to be used and runs the control loop that makes the program run in an infinite loop
vector<Mat> Transform (vector<Mat> planes, int rows, int cols, int operation_type, int struct_type, int mask_size); //This is control function for the trackbar. Calls the appropriate sequence of functions for every operation
void Erosion (vector<Mat> planes, int rows, int cols, int struct_type, int mask_size); //Performs erosion for a generic multi-channel image
void Dilation (vector<Mat> planes, int rows, int cols, int struct_type, int mask_size); //Performs dilation for a generic multi-channel image
vector<Mat> SubtractOperation (vector<Mat> src1, vector<Mat> src2); //Subtracts two vectors
vector<Mat> CopyVector (vector<Mat> src); //Copies a vector to another