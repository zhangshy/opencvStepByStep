#include <iostream>
#include "cv.h"
#include "highgui.h"
#include "testMethod.h"

using namespace std;
using namespace cv;
using namespace zsyTestMethod;

int main(int argc, char** argv) {
	Mat image = imread("../out/test.jpg", CV_LOAD_IMAGE_COLOR);
	if (!image.data) {
		cout <<  "Could not open or find the image" << std::endl ;
		return -1;
	}
	Mat dst = changRGB2Gray(image);
	imwrite("test2Gray.jpg", dst);
	imshow("Display", dst);
	waitKey(0);
	return 0;
}
