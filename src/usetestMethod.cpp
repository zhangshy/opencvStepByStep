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
	Mat dstGaussianBlur = GaussianBlur(dstGray, 2, 1.5);
	Mat dstGx = Mat::zeros(image.size(), CV_16SC1);
	Mat dstGy = Mat::zeros(image.size(), CV_16SC1);
	Mat dstSobel = sobelOperator(dstGaussianBlur, true, dstGx, dstGy);
	Mat dstNms = nmsOperator(dstSobel, dstGx, dstGy);
	uchar threMax = otsu(dstNms);
	uchar threMin = threMax/3;
	cout << "max: " << (int)threMax << " ;min: " << (int)threMin << endl;
	Mat dstHys = hysteresis(dstNms, threMin,threMax);
	Mat dstThreshold = matThreshold(dstNms);
	imshow("src", image);
	imshow("gray", dstGray);
	imshow("dstGaussianBlur", dstGaussianBlur);
	imshow("dstSobel", dstSobel);
	imshow("dstNms", dstNms);
	imshow("dstHys", dstHys);
	imshow("threshold", dstThreshold);
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
