#include <iostream>
//#include "cv.h"
//#include "highgui.h"
#include "testMethod.h"
#include "threshold.h"

using namespace std;
//using namespace cv;
using namespace zsyTestMethod;

int main(int argc, char** argv) {
	cout << getTestMethodVersion() << endl;
	Mat image = imread("../out/test.jpg", CV_LOAD_IMAGE_COLOR);
	if (!image.data) {
		cout <<  "Could not open or find the image" << std::endl ;
		return -1;
	}
	Mat dstGray = changRGB2Gray(image);
	Mat dstThreshold = matThreshold(dstGray);
	Mat dstAThreshold = adaptiveMatThreshold(dstGray, USEOTSU);
	imshow("src", image);
	imshow("gray", dstGray);
	imshow("threshold", dstThreshold);
	imshow("athreshold", dstAThreshold);
    waitKey(0);
#if 0
	Mat dstHE = rgbHistogramEqualizate(image, HE);
//	Mat dstAHE = rgbHistogramEqualizate(image, AHE);
	Mat dstCLAHE = rgbHistogramEqualizate(image, CLAHEMETHOD);
	Mat dstACE = rgbHistogramEqualizate(image, USEACE);
	Mat dstauto = rgbAutoContrast(image);
    imshow("Display dstauto", dstauto);
//	Mat dstACEWITHLSD = rgbHistogramEqualizate(image, USEACEWITHLSD);
//	Mat dstResize = resizeMat(image, 1.7);
    imshow("Display src", image);
	imshow("Display HE", dstHE);
//	imshow("Display AHE", dstAHE);
	imshow("Display CLAHE", dstCLAHE);
//	imshow("Display resize", dstResize);
    imshow("Display ACE", dstACE);
//    imshow("Display ACEWITHLSD", dstACEWITHLSD);
	waitKey(0);
#endif
	return 0;
}
