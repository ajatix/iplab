#include "Morphology.h" //Defined header for the experiment
#include <Windows.h>
#include <string.h>

string openfilename (char *filter = "Image files (*.bmp)\0*.*\0", HWND owner = NULL) {

	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = owner;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "";

	string fileNameStr;
	if ( GetOpenFileName(&ofn) ) {
		fileNameStr = fileName;
	}

	return fileNameStr;
}

int main () {

	char *fileName = new char[MAX_PATH]; //Need to convert string to const char
	strcpy(fileName, openfilename().data()); //File dialog open function
	MorphologicalTransforms (fileName, "Morphology.bmp");

	return 0;
}