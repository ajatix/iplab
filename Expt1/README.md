README

To run this program, enter the file parameters in the main function, 
ReadBitmap (const char * input_filename, const char * output_filename);

Make sure the images folder in the same parent directory as that of the codes.

Provide correct filenames, without spaces and with proper subdirectories

All our functions and bitmap structures are defined in the header ReadBitmap.h

Our functions used are:
bitmap_image readBMP (char *input) - It stores bitmap data in a structure img given an input filename

bitmap_image convertFlipGrayScale (bitmap_image img) - Converts an input bitmap (8-bit or 24-bit) to an 8-bit flipped image

int writeBMP (char *output, bitmap_image img) - Writes the changes to the desired output filename

int ReadBitmap(char* input_filename, char* output_filename) - This is the main function executed by the compiler
