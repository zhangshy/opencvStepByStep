/**
*@file histRedistribute.h
*@brief 调整图像的直方图分布
*/
#ifndef _HISTREDISTRIBUTE_H_
#define _HISTREDISTRIBUTE_H_
#include "cv.h"
#include "highgui.h"
using namespace cv;


/**
* 获取矩阵直方图均衡的变换函数，变换函数为[256]的数组，通过查表得到变换值
*@param src 输入但通道Mat
*@param convertBuf  [256]长度的数组，可通过查表法变换矩阵
*@param bUseCLAHE true将原直方图超出阈值的部分先平均分配后在计算分布函数
*/
bool getHistEqualConvert(const Mat src, uchar* convertBuf, bool bUseCLAHE);


bool getAutoCutConvert(const Mat src, uchar* convertBuf);
#endif
