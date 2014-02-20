/**
*@file mathTest.h
*@brief 实现一些数学算法，不调用opencv的接口
*/

#ifndef _MATH_TEST_H
#define _MATH_TEST_H

#ifndef uchar
#define uchar unsigned char
#endif
/**
*@brief 双线性插值
* x1<=x2 y1<=y2，z11=(x1, y1), z12=(x1, y2), z21=(x2, y1), z22=(x2, y2), x0在x1和x2之间，y0在y1和y2之间, x0,y0为插值点坐标, z00为(x0, y0)点插值
*@return true成功
*/
bool bilinearInterpolation(int x1, int y1, int x2, int y2, float x0, float y0, uchar z11, uchar z12, uchar z21, uchar z22, uchar* z00);

/**
*@brief rgb空间转hsv空间
*
* r,g,b values are from 0 to 1
* h = [0,360], s = [0,1], v = [0,1]
* if s == 0, then h = -1 (undefined)
*/
void perrgb2hsv(float r, float g, float b, float* h, float* s, float* v);

/**
*@brief hsv空间转rgb空间
*
* r,g,b values are from 0 to 1
* h = [0,360], s = [0,1], v = [0,1]
* if s == 0, then h = -1 (undefined)
*/
void perhsv2rgb(float h, float s, float v, float* r, float* g, float* b);
#endif