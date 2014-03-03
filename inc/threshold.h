/**
*@file threshold.h
* 图像二值化相关
*/

#ifndef _THRESHOLD_H_
#define _THRESHOLD_H_

#include "cv.h"
#include "highgui.h"
using namespace cv;
/**
* 大津算法: http://zh.wikipedia.org/wiki/%E5%A4%A7%E6%B4%A5%E7%AE%97%E6%B3%95
*/
uchar otsu(const Mat src);

Mat matThreshold(const Mat src);
#endif
