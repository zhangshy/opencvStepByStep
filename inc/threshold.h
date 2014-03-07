/**
*@file threshold.h
* 图像二值化相关
*/

#ifndef _THRESHOLD_H_
#define _THRESHOLD_H_

#include "cv.h"
#include "highgui.h"
using namespace cv;

#define THRESHOLDAREAS 4
#define USEOTSU 201
/**
* 大津算法: http://zh.wikipedia.org/wiki/%E5%A4%A7%E6%B4%A5%E7%AE%97%E6%B3%95
*/
uchar otsu(const Mat src);

Mat matThreshold(const Mat src);

/**
*参考CLAHE，分块二值化
*/
Mat adaptiveMatThreshold(const Mat src, int method);

/**
* 高斯模糊
*@param k 矩阵size为(2k+1)*(2k+1)
*@param sigma 标准差
*/
Mat GaussianBlur(const Mat src, int k, double sigma);
#endif
