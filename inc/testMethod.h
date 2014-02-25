#ifndef _TESTMETHOD_H_
#define _TESTMETHOD_H_
#include <cstring>
#include "cv.h"
#include "highgui.h"
using namespace cv;

/** AHE算法时将图像分成PERNUM*PERNUM块 */
#define PERNUM 8
#define HE 		100
#define AHE     101
#define CLAHEMETHOD	102
#define USEACE  103
#define USEACEWITHLSD  104

namespace zsyTestMethod {
	//获取版本信息
	string getTestMethodVersion();

	//将RGB图像转化为灰度图像
	Mat changRGB2Gray(const Mat src);

	//直方图均衡
	Mat histogramEqualizate(const Mat src);

	//自适应直方图均衡化(Adaptive histgram equalization/AHE)
	Mat adaptHistEqual(const Mat src);

	//RGB转换为HSV
	Mat rgb2hsv(const Mat src);

	//HSV转换为RGB
	Mat hsv2rgb(const Mat src);

	//RGB图像均衡，V通道均衡
	Mat rgbHistogramEqualizate(const Mat src, int method);

	//RGB图像均衡，灰度均衡
	Mat rgbHistogramEqualizateGray(const Mat src, int method);

	/**
	*@brief 利用双线性插值法对图像进行缩放
	*@param resize 缩放倍数
	*/
	Mat resizeMat(const Mat src, float resize);

	/**
	*@brief 使用局部标准差
	*
	* 参考：http://www.cnblogs.com/Imageshop/p/3324282.html
	*/
	Mat useACE(const Mat src, bool useLSD);
}
#endif
