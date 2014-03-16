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

/**
* sobel算子，计算图像边缘
*@param 输入单通道Mat
*/
Mat sobelOperator(const Mat src, bool getGxGy = false, Mat Gx = Mat::zeros(1, 1, CV_16SC1), Mat Gy = Mat::zeros(1, 1, CV_16SC1));

/**
*Non-Maximum Suppression 非极大值抑制
*/
Mat nmsOperator(const Mat src, const Mat Gx, const Mat Gy);

/**
* 滞后阈值: 最后一步，Canny 使用了滞后阈值，滞后阈值需要两个阈值(高阈值和低阈值):
*
* 如果某一像素位置的幅值超过 高 阈值, 该像素被保留为边缘像素。
* 如果某一像素位置的幅值小于 低 阈值, 该像素被排除。
* 如果某一像素位置的幅值在两个阈值之间,该像素仅仅在连接到一个高于 高 阈值的像素时被保留。
* Canny 推荐的 高:低 阈值比在 2:1 到3:1之间。
*/
Mat hysteresis(const Mat src, uchar thresholdLower, uchar thresholdUpper);
#endif
