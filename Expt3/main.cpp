//#include "ReadBitmap.h"
//#include "SpatialFilter.h"
#include "WaveletTransforms.h"

int main () {
	//ReadBitmap("images/lena.bmp","images/square7_result.bmp");
	//MainSpatialFilterControl ("Sample/", "images/filters/");
	WaveletTransforms ("images/lena.bmp", "images/filters/WaveletOutput.bmp");

	return 0;
}
