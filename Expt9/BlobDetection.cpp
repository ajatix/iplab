#include "RegionGrowing.h"

int getImage(Mat img, Mat output, int x_pos, int y_pos) {
    vector <vector<Point2i>> blobs;

    findBlobs(img, blobs);

	size_t i, j;
	int f=0;

	//Find the blob with the clicked point
	for (i=0; i<blobs.size(); i++) {
		for (j=0; j<blobs[i].size(); j++) {
			if ((blobs[i][j].x == x_pos) && (blobs[i][j].y == y_pos)) {
				f = 1;
				break;
			}
		}
		if (f) {
			break;
		}
	}

    for(size_t k=0; k<blobs[i].size(); k++) {
		int x = blobs[i][k].x;
		int y = blobs[i][k].y;

		output.at<Vec3b>(y,x)[0] = (char)255;
		output.at<Vec3b>(y,x)[1] = (char)255;
		output.at<Vec3b>(y,x)[2] = (char)255;
    }

    return 0;
}

void findBlobs(const Mat &binary, vector <vector<Point2i>> &blobs)
{
    blobs.clear();

    // Fill the label_image with the blobs
    // 0  - background
    // 1  - unlabelled foreground
    // 2+ - labelled foreground

    Mat label_image;
    binary.convertTo(label_image, CV_32FC1); // weird it doesn't support CV_32S!

    int label_count = 2; // starts at 2 because 0,1 are used already

    for(int y=0; y < binary.rows; y++) {
        for(int x=0; x < binary.cols; x++) {
            if((int)label_image.at<float>(y,x) != 255) {
                continue;
            }

            Rect rect;
            floodFill(label_image, Point(x,y), Scalar(label_count), &rect, Scalar(0), Scalar(0), 4);

            vector <Point2i> blob;

            for(int i=rect.y; i < (rect.y+rect.height); i++) {
                for(int j=rect.x; j < (rect.x+rect.width); j++) {
                    if((int)label_image.at<float>(i,j) != label_count) {
                        continue;
                    }

                    blob.push_back(Point2i(j,i));
                }
            }

            blobs.push_back(blob);

            label_count++;
        }
    }
}

