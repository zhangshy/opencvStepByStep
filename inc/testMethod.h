#ifndef _TESTMETHOD_H_
#define _TESTMETHOD_H_
#include <cstring>
#include "cv.h"
#include "highgui.h"
using namespace cv;

/**
* 将图像分成PERNUM*PERNUM块,
* AHE算法时分成8*8块，在使用局部自适应自动色阶时分成4*4块。
*/
#define                 PERNUM 4
#define HE 		        100
#define AHE             101
#define CLAHEMETHOD	    102
/** 使用局部平均值，大于平均值的更大，小于平均值的更小, CG使用固定值 */
#define USEACE          103
/** 使用局部平均值，大于平均值的更大，小于平均值的更小，CG有局部标准差LSD获得 */
#define USEACEWITHLSD   104

/** 将图像高低两端的像素裁剪后，在变换到[0, 255] */
#define ATUOCUTVALUE       105


/** 将int类型的值限定在[0, 255] */
#define Value0to255(x) ((x)>255 ? 255 : ((x)<0 ? 0 : (x)))

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

	/**
	* 对rgb三个通道分别使用局部自动适应的方式
	* 参考：http://www.cnblogs.com/Imageshop/p/3395968.html
	*/
	Mat rgbAutoContrast(const Mat src);
}
#endif
