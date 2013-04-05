#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace std;

#pragma pack(push,1)

//File header
typedef struct {
    char         filetype[2]; //Always 'BM' for bitmap images
    unsigned int filesize; //Total byte-size of the bitmap file
    short        reserved1;
    short        reserved2;
    unsigned int dataoffset; //Bytes offset to image pixel data
} file_header;

//Bitmap header
typedef struct {
    file_header  fileheader;
    unsigned int headersize; //40 bytes
    int          width;
    int          height;
    short        planes;
    short        bitsperpixel; //24 for colour images, 8 bit for grayscale
    unsigned int compression;
    unsigned int bitmapsize;
    int          horizontalres; //Pixel print density
    int          verticalres; //Pixel print density
    unsigned int numcolors;
    unsigned int importantcolors;
} bitmap_header;

typedef struct {
	bitmap_header *hp;
    char *data, *data_out, *data_flip, *color_table; //The extracted pixel data, modified pixel data and the colour table
} bitmap_image;

#pragma pack(pop)

bitmap_image readBMP (char *input); //Function to read BMP
bitmap_image convertFlipGrayScale (bitmap_image img); //Function to convert
int writeBMP (char *output, bitmap_image img); //Write converted BMP file
int ReadBitmap(char* input_filename, char* output_filename); //Main function for the exercise

bitmap_image readBMP (char *input) {
	bitmap_image img;
	FILE *fp; //File pointer for reading input image
	int n;

	//Opening bitmap file
	fp = fopen (input, "rb");
	if (fp == NULL) {
		//Incorrect bitmap file input
	}

	//Reading file headers
	img.hp = (bitmap_header*)malloc(sizeof(bitmap_header));
	if (img.hp == NULL) {
		//Memory allocation error
	}
	n = fread (img.hp, sizeof(bitmap_header), 1, fp); //fread (void * ptr,  size, count, FILE * stream);
	if (n<1) {
		//Error in reading
	}

	//Reading image data
	img.data = (char*)malloc(sizeof(char)*img.hp->bitmapsize);
	if (img.data == NULL) {
		//Error in memory allocation
	}

	fseek (fp,sizeof(char)*img.hp->fileheader.dataoffset,SEEK_SET); //Takes care to skip to the pixel data using dataoffset
	n = fread (img.data,sizeof(char),img.hp->bitmapsize,fp);
	if (n<1) {
		//Error in reading data
	}

	fclose(fp);

	return img;
}

bitmap_image convertFlipGrayScale (bitmap_image img) {
	int i,j;

	//Grayscale conversion. Making changes to file header first.
	short f = img.hp->bitsperpixel;
	img.hp->bitsperpixel = 8;
	img.hp->bitmapsize = img.hp->bitmapsize / (f/8);
	img.hp->fileheader.dataoffset += 1024;
	img.hp->fileheader.filesize = img.hp->bitmapsize + img.hp->fileheader.dataoffset;

	//Initializing data_out
	img.data_out = (char*)malloc(sizeof(char)*img.hp->bitmapsize);
	if(img.data_out==NULL){
		//Memory allocation error
	}

	if (f!=8) { //The image is already b/w
		//Conversion process
		unsigned int b,g,r;

		for (i=0;i<(int)img.hp->bitmapsize*3;i+=3) {
			b = (int)img.data[i]; g = (int)img.data[i+1]; r = (int)img.data[i+2];
			img.data_out[i/3] = (int)(g);
		}
	}
	else {
		//Conversion process
		for (i=0;i<(int)img.hp->bitmapsize;i++) {
			img.data_out[i] = (int)(img.data[i]);
		}
	}

	img.color_table = (char*)malloc(sizeof(char)*1024);
	if(img.color_table==NULL) {
		//Memory allocation error
	}

	for (j=0;j<1024;j+=4) {
		img.color_table[j] = img.color_table[j+1] = img.color_table[j+2] = j/4;
		//img.color_table[j+2] = (j/4+1)%256;
		img.color_table[j+3] = 0;
	}

	//Initializing data_out
	img.data_flip = (char*)malloc(sizeof(char)*img.hp->bitmapsize);
	if(img.data_flip==NULL){
		//Memory allocation error
	}

	char temp1,temp2; //To use for swapping
	int ratio = img.hp->width/img.hp->height;

	//Transpose algorithm
	for (j=0;j<img.hp->width;j++) {
		for (i=0;i<img.hp->height;i++) {
			img.data_flip[j*img.hp->height+i] = img.data_out[i*img.hp->width+j];
		}
	}

	free (img.data_out); //Since we'll be replacing it with an updated pixel data

	img.data_out = img.data_flip; 

	img.hp->height = img.hp->height + img.hp->width;
	img.hp->width = img.hp->height - img.hp->width;
	img.hp->height = img.hp->height - img.hp->width;

	free (img.data); //Since we don't need the original data anymore.

	return img;
}

int writeBMP (char *output, bitmap_image img) {
	FILE *out; //File pointer for writing input image
	int n;
	out = fopen(output, "wb");
	if(out==NULL){
		//Error in opening filestream
	}

	//Writing file and bitmap headers
	n = fwrite(img.hp,sizeof(char),sizeof(bitmap_header),out);
	if(n<1){
		//Header does not exist
	}

	//Writing colour table for 8-bit image
	n = fwrite(img.color_table,sizeof(char),1024,out);
	if(n<1){
		//Colour table does not exist
	}

	//Writing pixel data
	fseek (out,sizeof(char)*img.hp->fileheader.dataoffset,SEEK_SET); //We add dataoffset and not headersize to account for any other optional header data that may be required. Eg. colour table
	n = fwrite(img.data_out,sizeof(char),img.hp->bitmapsize,out);
	if(n<1){
		//Pixel data does not exist
	}

	fclose(out);
	free(img.hp);
	free(img.data_out);
	free(img.color_table);

	return 0;
}

int ReadBitmap(char* input_filename, char* output_filename) {
	bitmap_image img;
	img = readBMP(input_filename);
	img = convertFlipGrayScale (img);
	writeBMP (output_filename, img);

	return 0;
}
