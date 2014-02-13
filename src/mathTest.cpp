#include <iostream>
#include "mathTest.h"

using namespace std;

/**
* 双线性插值，参考http://zh.wikipedia.org/wiki/%E5%8F%8C%E7%BA%BF%E6%80%A7%E6%8F%92%E5%80%BC
* x1<x2 y1<y2，z11=(x1, y1), z12=(x1, y2), z21=(x2, y1), z22=(x2, y2), x0在x1和x2之间，y0在y1和y2之间
*/
bool bilinearInterpolation(int x1, int y1, int x2, int y2, float x0, float y0, uchar z11, uchar z12, uchar z21, uchar z22, uchar* z00)
{
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
