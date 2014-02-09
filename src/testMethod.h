#ifndef _TESTMETHOD_H_
#define _TESTMETHOD_H_
#include <cstring>
#include "cv.h"
#include "highgui.h"
using namespace cv;

namespace zsyTestMethod {
	//获取版本信息
	string getTestMethodVersion();

	//将RGB图像转化为灰度图像
	Mat changRGB2Gray(const Mat src);

	//直方图均衡
	Mat histogramEqualizate(const Mat src);

	//RGB转换为HSV
	Mat rgb2hsv(const Mat src);
	
	//HSV转换为RGB
	Mat hsv2rgb(const Mat src);

	//RGB图像均衡
	Mat rgbHistogramEqualizate(const Mat src);
}
#endif
