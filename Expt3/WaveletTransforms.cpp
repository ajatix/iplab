#include "WaveletTransforms.h"

#define NUM_LEVELS 3

int WaveletTransforms (char *input_filename, char *output_filename) {
	Mat img = imread(input_filename, CV_LOAD_IMAGE_COLOR); //Loading image
	Mat transformed(img.rows, img.cols, img.depth(), img.channels()); //Creating new matrices for transformed, recovered and difference images
	Mat recovered(img.rows, img.cols, img.depth(), img.channels()); 
	Mat difference(img.rows, img.cols, img.depth(), img.channels()); 
	
	vector<Mat> planes; //Splitting image into planes so that we can process images of any depth
	split (img, planes);

	int rows = img.rows;
	int cols = img.cols;

	ForwardTransform (planes, rows, cols, 1); //1 - HAAR, 2 - DB2, 3 - CDT 9/7 
	
	merge (planes, transformed); //Merging the planes to form a final image
	split (transformed, planes);

	BackwardTransform (planes, rows, cols, 1); //1 - HAAR, 2 - DB2, 3 - CDT 9/7 

	merge (planes, recovered);
	subtract (img,recovered,difference);
	split (difference, planes);

	cout << "The PSNR of this image is: " << CalculatePSNR (planes, rows, cols) << endl;

	namedWindow ("Original", CV_WINDOW_AUTOSIZE);
	imshow ("Original", img);

	namedWindow ("Transformed", CV_WINDOW_AUTOSIZE);
	imshow ("Transformed", transformed);

	namedWindow ("Recovered", CV_WINDOW_AUTOSIZE);
	imshow ("Recovered", recovered);

	namedWindow ("Difference", CV_WINDOW_AUTOSIZE);
	imshow ("Difference", difference);

	waitKey(0);

	//imwrite (output_filename, transformed); //Saving the output image
	//imwrite (output_filename, recovered); //Saving the output image
	//imwrite (output_filename, difference); //Saving the output image

	destroyAllWindows();

	return 0;
}

void ForwardTransform (vector<Mat> planes, int rows, int cols, int type) {
	uchar *temp_row = new uchar[cols];
	uchar *temp_col = new uchar[rows];

	int r = rows, c = cols;
	int i,j,ch;

	int r_limit = (rows/(pow((float)2,NUM_LEVELS))), c_limit = (cols/(pow((float)2,NUM_LEVELS))); //The lower limit upto which the ROI can go

	while (r>r_limit || c>c_limit) {
		if (c>c_limit) {
			for (ch = 0; ch<planes.size(); ch++) {
				for (i=0; i<r; i++) { //The region of interest reduces to just include the LL quadrant every time
					for (j=0; j<c; j++) {
						temp_row[j] = planes[ch].data[cols*i + j];
					}
					Forward1D (temp_row, cols, c, type);
					for (j=0; j<c; j++) {
						planes[ch].data[cols*i + j] = temp_row[j];
					}
				}
			}
		}
		if (r>r_limit) {
			for (ch = 0; ch<planes.size(); ch++) {
				for (i=0; i<c; i++) {
					for (j=0; j<r; j++) {
						temp_col[j] = planes[ch].data[cols*j + i];
					}
					Forward1D (temp_col, rows, r, type);
					for (j=0; j<c; j++) {
						planes[ch].data[cols*j + i] = temp_col[j];
					}
				}
			}
		} //We divide the row and col parameters every iterations
		if (c>c_limit) {
			c/=2;
		}
		if (r>r_limit) {
			r/=2;
		}
	}
	//Cleaning up memory
	delete [] temp_row;
	delete [] temp_col;
}

void Forward1D (uchar *vec, int N, int L, int type) {
	int i;
	uchar *vecp = new uchar[L];
	
	float kernel_DB2[4] = {0.1294095226, 0.2241438680, 0.8365163037, 0.4829629131};
	float kernel_CDT[4] = {-1.586134342, -0.05298011854, 0.8829110762, 0.4435068522};

	for (i=0; i<L; i++) {
		vecp[i] = 0;
	}

	switch (type) {
	case 1: //HAAR Transform
		L/=2;
		for (i=0; i<L; i++) {
			//vecp[i] = (int)(vec[2*i] + vec[2*i+1])/sqrt(2.0);
			//vecp[i+L] = (int)(-vec[2*i] + vec[2*i+1])/sqrt(2.0);
			vecp[i] = (int)(vec[2*i] + vec[2*i+1])/2;
			vecp[i+L] = (int)(-vec[2*i] + vec[2*i+1])/2;
		}
		for (i=0; i<(L*2); i++) {
			vec[i] = vecp[i];
		}
		break;
	case 2: //DB2 Transform
		L/=2;	
		for (i=1; i<L-1; i+=2) {
			vecp[i] = (vec[i-1] + vec[i+1])*-kernel_DB2[0];
			vecp[i+2] = (vec[i-1] + vec[i+1])*kernel_DB2[2];
			vecp[i+L] = (-vec[i-1] + vec[i+1])*-kernel_DB2[3];
			vecp[i+2+L] = (-vec[i-1] + vec[i+1])*-kernel_DB2[1];
		}
		for (i=2; i<L-2; i+=2) {
			vecp[i+1] = (vec[i-1] + vec[i+1])*kernel_DB2[1];
			vecp[i+3] = (vec[i-1] + vec[i+1])*kernel_DB2[3];
			vecp[i+1+L] = (-vec[i-1] + vec[i+1])*kernel_DB2[2];
			vecp[i+3+L] = (-vec[i-1] + vec[i+1])*-kernel_DB2[0];
		}
		for (i=0; i<(L*2); i++) {
			vec[i] = vecp[i];
		}
		break;
	case 3: //CDT 9/7 Transform
		L/=2;	
		for (i=1; i<L-1; i+=2) {
			vecp[i] = (vec[i-1] + vec[i+1])*kernel_CDT[0];
			vecp[i+L] = (vec[i-1] + vec[i+1])*kernel_CDT[2];
		}
		for (i=2; i<L-2; i+=2) {
			vecp[i+1] = (vec[i] + vec[i+2])*kernel_CDT[1];
			vecp[i+1+L] = (vec[i] + vec[i+2])*kernel_CDT[3];
		}
		for (i=0; i<(L*2); i++) {
			vec[i] = vecp[i];
		}
		break;
	default: 
		cout << "Wrong type entered\n";
	}

	delete [] vecp;
}

void BackwardTransform (vector<Mat> planes, int rows, int cols, int type) {
	uchar *temp_row = new uchar[cols];
	uchar *temp_col = new uchar[rows];

	int r = (rows/(pow((float)2,NUM_LEVELS))), c = (cols/(pow((float)2,NUM_LEVELS))); //We go from smallest size to bigger size
	int i,j,ch;
	
	int r_limit = rows, c_limit = cols; //Here, the limits are the final image size

	while (r<r_limit || c<c_limit) {
		if (r<r_limit) {
			for (ch = 0; ch<planes.size(); ch++) {
				for (i=0; i<2*c; i++) {
					for (j=0; j<2*r; j++) {
						temp_col[j] = planes[ch].data[cols*j + i];
					}
					Backward1D (temp_col, rows, 2*r, type); //We send double the ROI of the LL image
					for (j=0; j<2*c; j++) {
						planes[ch].data[cols*j + i] = temp_col[j];
					}
				}
			}
		}
		if (c<c_limit) {
			for (ch = 0; ch<planes.size(); ch++) {
				for (i=0; i<2*r; i++) {
					for (j=0; j<2*c; j++) {
						temp_row[j] = planes[ch].data[cols*i + j];
					}
					Backward1D (temp_row, cols, 2*c, type);
					for (j=0; j<2*c; j++) {
						planes[ch].data[cols*i + j] = temp_row[j];
					}
				}
			}
		} //Multiplying the row and col parameters on every iteration
		if (c<c_limit) {
			c*=2;
		}
		if (r<r_limit) {
			r*=2;
		}

	}
	//Cleaning up memory
	delete [] temp_row;
	delete [] temp_col;
}

void Backward1D (uchar *vec, int N, int L, int type) {
	int i;
	uchar *vecp = new uchar[L];

	for (i=0; i<L; i++) {
		vecp[i] = 0;
	}

	switch (type) {
	case 1: //HAAR Inverse Transform
		L/=2;
		
		for (i=0; i<L; i++) {
			//vecp[2*i] = (int)(vec[i] - vec[i+L])/sqrt(2.0);
			//vecp[2*i+1] = (int)(vec[i] + vec[i+L])/sqrt(2.0);
			vecp[2*i] = (int)(vec[i] - vec[i+L]);
			vecp[2*i+1] = (int)(vec[i] + vec[i+L]);
		}
		for (i=0; i<(L*2); i++) {
			vec[i] = vecp[i];
		}
		break;
	case 2: //DB2 Inverse Transform
		L/=2;
		
		for (i=0; i<L-1; i++) {
			vecp[2*i] = (vec[i]*(0.4829629131) + vec[i+1]*(0.8365163037) + vec[i+L]*(0.2241438680) + vec[i+1+L]*(-0.1294095226));
			vecp[2*i+1] = (vec[i]*(-0.1294095226) + vec[i+1]*(-0.2241438680) + vec[i+L]*(0.8365163037) + vec[i+1+L]*(-0.4829629131));
		}
		for (i=0; i<(L*2); i++) {
			vec[i] = vecp[i];
		}
		break;
	default: 
		cout << "Wrong type entered\n";
	}

	delete [] vecp;
}

float CalculatePSNR (vector<Mat> planes, int rows, int cols) {
	float sum=0;
	for (int c=0; c<planes.size(); c++) {
		for (int i=0; i<rows; i++) {
			uchar* rowi = planes[c].ptr/*<uchar>*/(i);
			for (int j=0; j<cols; j++) {
				sum += pow((float)rowi[j],2); //Adding the square error of every pixel
			}
		}
	}
	sum = log(sqrt(sum)); //Log of the root of the total squared error
	return sum;
}

//void DB21D (uchar *vec, int N, int L) {
//	int i;
//	uchar *vecp = new uchar[L];
//
//	for (i=0; i<L; i++) {
//		vecp[i] = 0;
//	}
//
//	L/=4;
//		
//	for (i=0; i<L; i++) {
//		vecp[i] = (vec[4*i]*(-0.1294095226) + vec[4*i+1]*(0.2241438680) + vec[4*i+2]*(0.8365163037) + vec[4*i+3]*(0.4829629131));
//		vecp[i+L] = (vec[4*i]*(-0.4829629131) + vec[4*i+1]*(0.8365163037) + vec[4*i+2]*(-0.2241438680) + vec[4*i+3]*(-0.1294095226));
//		vecp[i+1] = vecp[i];
//		vecp[i+1+L] = vecp[i+L];
//		//vecp[i+2*L] = (vec[4*i]*(0.4829629131) + vec[4*i+1]*(0.8365163037) + vec[4*i+2]*(0.2241438680) + vec[4*i+3]*(-0.1294095226));
//		//vecp[i+3*L] = (vec[4*i]*(-0.1294095226) + vec[4*i+1]*(-0.2241438680) + vec[4*i+2]*(0.8365163037) + vec[4*i+3]*(-0.4829629131));
//	}
//	for (i=0; i<(L*4); i++) {
//		vec[i] = vecp[i];
//	}
//
//	delete [] vecp;
//}
//
//void Inv_DB21D (uchar *vec, int N, int L) {
//	int i;
//	uchar *vecp = new uchar[L];
//
//	for (i=0; i<L; i++) {
//		vecp[i] = 0;
//	}
//
//	L/=2;
//		
//	for (i=0; i<L-1; i++) {
//		vecp[2*i] = (vec[i]*(0.4829629131) + vec[i+1]*(0.8365163037) + vec[i+L]*(0.2241438680) + vec[i+1+L]*(-0.1294095226));
//		vecp[2*i+1] = (vec[i]*(-0.1294095226) + vec[i+1]*(-0.2241438680) + vec[i+L]*(0.8365163037) + vec[i+1+L]*(-0.4829629131));
//	}
//	for (i=0; i<(L*2); i++) {
//		vec[i] = vecp[i];
//	}
//
//	delete [] vecp;
//}
//
//void CDF_97 (vector<Mat> planes, int rows, int cols) {
//	double a[4] = {-1.586134342, -0.05298011854, 0.8829110762, 0.4435068522};
//	double k[2] = {0.81289306611596146, 0.61508705245700002};
//
//	int i,j,ch;
//
//	int s1 = planes[0].step[0], s2 = planes[0].step[1] ;
//	
//	double ***temp = new double**[3];
//	for (i=0;i<3;i++) {
//		temp[i] = new double *[cols];
//		for (j=0;j<cols;j++) {
//			temp[i][j] = new double [rows];
//		}
//	}
//	int width = cols, height = rows;
//	
//	for (ch = 0; ch<planes.size(); ch++) 
//	{
//		for (i = 0; i < width; i++)
//		{
//			for(j = 1; j < height-1; j+=2)
//			{
//				planes[ch].data[s1*j+ s2*i] +=  a[0] * (planes[ch].data[s1*(j-1)+ s2*i] + planes[ch].data[s1*(j+1)+ s2*i]) ;
//			}
//			planes[ch].data[s1*(height-1)+ s2*i] += 2 * a[0] * planes[ch].data[s1*(height - 2) + s2*i] ;
//			
//			for(j = 2; j < height; j+=2)
//			{	
//				planes[ch].data[s1*j+ s2*i] += a[1] * (planes[ch].data[s1*(j-1)+ s2*i] + planes[ch].data[s1*(j+1)+ s2*i]) ;
//			}
//			planes[ch].data[s1*0+ s2*i] += 2 * a[1] * planes[ch].data[s1*1 + s2*i] ;
//
//			for(j = 1; j < height-1; j+=2)
//			{
//				planes[ch].data[s1*j+ s2*i] += a[2] * (planes[ch].data[s1*(j-1)+ s2*i] + planes[ch].data[s1*(j+1)+ s2*i]) ;
//			}
//			planes[ch].data[s1*(height-1)+ s2*i] += 2 * a[2] * planes[ch].data[s1*(height - 2) + s2*i] ;
//				 
//			for(j = 2; j < height; j+=2)
//			{
//				planes[ch].data[s1*j+ s2*i] += a[3] * (planes[ch].data[s1*(j-1)+ s2*i] + planes[ch].data[s1*(j+1)+ s2*i]) ;
//			}
//			planes[ch].data[s1*0+ s2*i] += 2 * a[3] * planes[ch].data[s1*1 + s2*i] ;
//
//		}
//		for (j = 0; j < height; j++)
//		{
//			for (i = 0; i < width; i++)
//			{
//				if (j % 2 == 0)
//					temp[ch][i][j/2] = k[0] * planes[ch].data[s1*j + s2*i] ;
//				else
//					temp[ch][i][j/2 + height/2] = k[1] * planes[ch].data[s1*j + s2*i] ;
//			}
//		}
//	}
//	
//	for (ch = 0; ch<planes.size(); ch++) 
//	{
//		for (j = 0; j < height; j++)
//		{
//			for (i = 0; i < width; i++)
//			{
//				planes[ch].data[s1*j + s2*i]  = temp[ch][j][i] ;
//			}
//		}
//	}
//
//	
//}
//
