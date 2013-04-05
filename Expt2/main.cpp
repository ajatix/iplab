#include <stdio.h>
//#include "ReadBitmap.h"
#include "SpatialFilter.h"

int main () {
	//ReadBitmap("images/lena.bmp","images/square7_result.bmp");
	MainSpatialFilterControl ("images/", "images/filters/");

	return 0;
}
