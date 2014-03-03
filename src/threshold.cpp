/**
*@file threshold.cpp
*/
#include <iostream>
#include "threshold.h"
using namespace std;

uchar otsu(const Mat src) {
    if (src.channels() != 1) {
        cout << "src channel should be 1" << endl;
        return 0;
    }
    int nrow = src.rows;
    int ncol = src.cols;
    int hist[256] = {0};
    int i, j;
    int total = nrow*ncol;
    long sum = 0;
    const uchar* data_src = NULL;
    for (i=0; i<nrow; i++) {
        data_src = src.ptr<uchar>(i);
        for (j=0; j<ncol; j++) {
            hist[data_src[j]]++;
            sum += data_src[j];
        }
    }
    int wB = 0;
    int wF = 0;
    int mB = 0;
    int mF = 0;
    int between = 0;
    int max = 0;
    int sumB = 0;
    uchar threshold1=0, threshold2=0;
    for (i=0; i<256; i++) {
        wB += hist[i];
        if (wB==0)
            continue;
        wF = total - wB;
        if (wF == 0)
            break;
        sumB += i*hist[i];
        mB = sumB/wB;
        mF = (sum-sumB)/wF;
        between = wB*wF*(mB-mF)*(mB-mF);
        if (between>=max) {
            threshold1 = i;
            if (between>max) {
                threshold2 = i;
            }
            max = between;
        }
    }
    return (threshold1+threshold2)/2;
}

Mat matThreshold(const Mat src) {
    int nrow = src.rows;
    int ncol = src.cols;
    uchar threshold = otsu(src);
    int i, j;
    Mat dst = Mat::zeros(nrow, ncol, CV_8UC1);
    const uchar* data_src = NULL;
    uchar* data_dst = NULL;
    for (i=0; i<nrow; i++) {
        data_src = src.ptr<uchar>(i);
        data_dst = dst.ptr<uchar>(i);
        for (j=0; j<ncol; j++) {
            if (data_src[j]>threshold)
                data_dst[j] = 255;
            else
                data_dst[j] = 0;
        }
    }
    return dst;
}
