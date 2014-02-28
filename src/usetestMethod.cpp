#include <iostream>
//#include "cv.h"
//#include "highgui.h"
#include "testMethod.h"

using namespace std;
//using namespace cv;
using namespace zsyTestMethod;

int main(int argc, char** argv) {
	cout << getTestMethodVersion() << endl;
	Mat image = imread("../out/test2.jpg", CV_LOAD_IMAGE_COLOR);
	if (!image.data) {
		cout <<  "Could not open or find the image" << std::endl ;
		return -1;
	}
	Mat dstauto = rgbAutoContrast(image);
    imshow("Display dstauto", dstauto);
    waitKey(0);
#if 0
	Mat dstHE = rgbHistogramEqualizate(image, HE);
//	Mat dstAHE = rgbHistogramEqualizate(image, AHE);
	Mat dstCLAHE = rgbHistogramEqualizate(image, CLAHEMETHOD);
	Mat dstACE = rgbHistogramEqualizate(image, USEACE);
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
