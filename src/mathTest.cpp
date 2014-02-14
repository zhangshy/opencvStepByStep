#include <iostream>
#include<cmath>
#include "mathTest.h"

using namespace std;

/**
* 双线性插值，参考http://zh.wikipedia.org/wiki/%E5%8F%8C%E7%BA%BF%E6%80%A7%E6%8F%92%E5%80%BC
* x1<=x2 y1<=y2，z11=(x1, y1), z12=(x1, y2), z21=(x2, y1), z22=(x2, y2), x0在x1和x2之间，y0在y1和y2之间
*/
bool bilinearInterpolation(int x1, int y1, int x2, int y2, float x0, float y0, uchar z11, uchar z12, uchar z21, uchar z22, uchar* z00)
{
    if ((x1>x2) || (y1>y2)) {
        cout << "input error, need x1<=x2, y1<=y2, but x1: " << x1 << " x2: " << x2
            << " y1: " << y1 << " y2: " << y2 << endl;
        return false;
    }
    if ((x0<x1) || (x0>x2) || (y0<y1) || (y0>y2)) {
        cout << "input error, need x0 between x1,x2 and y0 between y1,y2"<< " x1: " << x1
        << " x2: " << x2 << " y1: " << y1 << " y2: " << y2 << " x0: " << x0 << " y0: " << y0 << endl;
        return false;
    }

    if ((x1==x2) && (y1==y2)) {
        //5个点重合
        *z00 = z11;
    } else if (x1==x2) {
        *z00 = z11 + (z12-z11)*(y0-y1)/(y2-y1);
    } else if (y1==y2) {
        *z00 = z11 + (z21-z11)*(x0-x1)/(x2-x1);
    } else {
        int div = (x2-x1)*(y2-y1);
        float x20 = x2-x0;
        float y20 = y2-y0;
        float x01 = x0-x1;
        float y01 = y0-y1;
        //根据公式
        *z00 = (z11*x20*y20 + z21*x01*y20 + z12*x20*y01 + z22*x01*y01)/div;
    }
    return true;
}

/**
*@brief rgb空间转hsv空间
*
* r,g,b values are from 0 to 1
* h = [0,360], s = [0,1], v = [0,1]
* if s == 0, then h = -1 (undefined)
*/
void perrgb2hsv(float r, float g, float b, float* h, float* s, float* v)
{
    float min, max, delta,tmp;
    tmp = r>g ? g : r;
    min = tmp>b ? b : tmp;
    tmp = r>g ? r : g;
    max = tmp>b ? tmp : b;
    *v = max;
    delta = max - min;
    if (max!=0)
        *s = delta / max;
    else {
        // r = g = b = 0 // s = 0, v is undefined
        *s = 0;
        *h = 0;
        return;
    }
    if (delta == 0) {
        *h=0;
        return;
    }else if (r == max) {
        if (g>=b) {
            *h = (g-b)/delta;
        }else {
            *h = (g-b)/delta + 6.0;
        }
    }else if (g == max) {
        *h = 2.0 + (b-r)/delta;
    }else if (b == max) {
        *h = 4.0 + (r-g)/delta;
    }
    *h *= 60.0;
    if (*h<0)
        *h+=360;
}

/**
*@brief hsv空间转rgb空间
*
* r,g,b values are from 0 to 1
* h = [0,360], s = [0,1], v = [0,1]
* if s == 0, then h = -1 (undefined)
*/
void perhsv2rgb(float h, float s, float v, float* r, float* g, float* b)
{
    int i;
    float f, p, q, t;

    if( s == 0 )
    {
    // achromatic (grey)
        *r = *g = *b = v;
        return;
    }

    h /= 60; // sector 0 to 5
    i = floor( h ); //floor(x),有时候也写做Floor(x)，其功能是“向下取整”，或者说“向下舍入”，即取不大于x的最大整数
    f = h - i; // factorial part of h
    p = v * ( 1 - s );
    q = v * ( 1 - s * f );
    t = v * ( 1 - s * ( 1 - f ) );

    switch( i )
    {
    case 0:
        *r = v;
        *g = t;
        *b = p;
       break;
    case 1:
       *r = q;
       *g = v;
       *b = p;
       break;
    case 2:
       *r = p;
       *g = v;
       *b = t;
       break;
    case 3:
       *r = p;
       *g = q;
       *b = v;
       break;
    case 4:
       *r = t;
       *g = p;
       *b = v;
       break;
    default: // case 5:
       *r = v;
       *g = p;
       *b = q;
       break;
    }
}
