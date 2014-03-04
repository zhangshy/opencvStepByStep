/**
*@file threshold.cpp
*/
#include <iostream>
#include "threshold.h"
#include "mathTest.h"
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

Mat adaptiveMatThreshold(const Mat src, int method) {
    int nrow = src.rows;    //图像的高度
    int ncol = src.cols;    //列数,宽
    int channel = src.channels();
    Mat dst = Mat::zeros(nrow, ncol, CV_8UC1);
    if (channel != 1) {
        cout << "请输入单通道矩阵，当前图像为channel is: " << channel << endl;
        return dst;
    }
    int perRow = nrow/THRESHOLDAREAS;
    int perCol = ncol/THRESHOLDAREAS;
    uchar thresholds[THRESHOLDAREAS*THRESHOLDAREAS] = {0};
    int areaCenters[THRESHOLDAREAS*THRESHOLDAREAS][2] = {0};
    int i=0, j=0;
    for (i=0; i<THRESHOLDAREAS; i++) {
        for (j=0; j<THRESHOLDAREAS; j++) {
//        cout << "i: " << i << ";j: " << j << endl;
    /** 注意赋值top-left = 50,50 size=100,50: Rect(50,50,100,50)
    * size为Size(cols, rows)，其第一个参数为cols，即图像的宽度。即Mat大小先是高度然后是宽度，而size大小显示宽度然后是高度。
    */
            Mat tmpSrc = src(Rect(j*perCol, i*perRow, perCol, perRow));   ///使用数据副本:Mat image2 = image1(Rect(2,2,99,99)).clone();
//                Mat tmpSrc = src(Rect(0, 399, 50, 57));
            areaCenters[i*THRESHOLDAREAS+j][0] = i*perRow + perRow/2;
            areaCenters[i*THRESHOLDAREAS+j][1] = j*perCol + perCol/2;
            switch (method) {
                case USEOTSU:
                    thresholds[i*THRESHOLDAREAS+j] = otsu(tmpSrc);
                    break;
                default:
                    thresholds[i*THRESHOLDAREAS+j] = otsu(tmpSrc);
            }
        }
    }
    int uRIndex = THRESHOLDAREAS - 1;   //右上角upper right corner
    int lLIndex = (THRESHOLDAREAS-1)*THRESHOLDAREAS;    //左下角lower left corner
    int lRIndex = THRESHOLDAREAS*THRESHOLDAREAS-1;  //右下角lower right corner
    int r1=0, r2=0, c1=0, c2=0;
    int k=0, t=0;
    uchar z11=0, z12=0, z21=0, z22=0, zii=0;
    const uchar* data_in = NULL;
    uchar* data_out = NULL;
    int threshold = 0;
    for (i=0; i<nrow; i++) {
        data_in = src.ptr<uchar>(i);
        data_out = dst.ptr<uchar>(i);
        for (j=0; j<ncol; j++) {
            if ((i<=areaCenters[0][0]) && (j<=areaCenters[0][1])) {
                threshold = thresholds[0];  ///左上角使用所在矩阵编号0的变换函数
            } else if ((i<=areaCenters[uRIndex][0]) && (j>=areaCenters[uRIndex][1])) {
                threshold = thresholds[uRIndex];  ///右上角使用所在矩阵编号uRIndex的变换函数
            } else if ((i>=areaCenters[lLIndex][0]) && (j<=areaCenters[lLIndex][1])) {
                threshold = thresholds[lLIndex];    ///左下角使用所在矩阵编号lLIndex的变换函数
            } else if ((i>=areaCenters[lRIndex][0]) && (j>=areaCenters[lRIndex][1])) {
                threshold = thresholds[lRIndex];    ///右下角使用所在矩阵编号lRIndex的变换函数
            } else if ((i<=areaCenters[0][0]) && (j>=areaCenters[0][1]) && (j<=areaCenters[uRIndex][1])) {
                for (k=0; k<uRIndex; k++) {
                    if ((j>=areaCenters[k][1]) && (j<=areaCenters[k+1][1])) {
                        //使用线性插值，计算两点和对应的值
                        c1 = areaCenters[k][1];
                        c2 = areaCenters[k+1][1];
                        z11 = thresholds[k];
                        z12 = thresholds[k+1];
                        break;
                    }
                }
                threshold = z11 + (z12-z11)*(j-c1)/(c2-c1);
            } else if ((i>=areaCenters[lLIndex][0]) && (j>=areaCenters[lLIndex][1]) && (j<=areaCenters[lRIndex][1])) {
                for (k=lLIndex; k<lRIndex; k++) {
                    if ((j>=areaCenters[k][1]) && (j<=areaCenters[k+1][1])) {
                        //使用线性插值，计算两点和对应的值
                        c1 = areaCenters[k][1];
                        c2 = areaCenters[k+1][1];
                        z11 = thresholds[k];
                        z12 = thresholds[k+1];
                        break;
                    }
                }
                threshold = z11 + (z12-z11)*(j-c1)/(c2-c1);
            } else if ((j<=areaCenters[0][1]) && (i>=areaCenters[0][0]) && (i<=areaCenters[lLIndex][0])) {
                for (k=0; k<lLIndex; k+=THRESHOLDAREAS) {
                    if ((i>=areaCenters[k][0]) && (i<=areaCenters[k+THRESHOLDAREAS][0])) {
                        r1 = areaCenters[k][0];
                        r2 = areaCenters[k+THRESHOLDAREAS][0];
                        z11 = thresholds[k];
                        z21 = thresholds[k+THRESHOLDAREAS];
                        threshold = z11 + (z21-z11)*(i-r1)/(r2-r1);
                        break;
                    }
                }
            } else if ((j>=areaCenters[uRIndex][1]) && (i>=areaCenters[uRIndex][0]) && (i<=areaCenters[lRIndex][0])) {
                for (k=uRIndex; k<lRIndex; k+=THRESHOLDAREAS) {
                    if ((i>=areaCenters[k][0]) && (i<=areaCenters[k+THRESHOLDAREAS][0])) {
                        r1 = areaCenters[k][0];
                        r2 = areaCenters[k+THRESHOLDAREAS][0];
                        z11 = thresholds[k];
                        z21 = thresholds[k+THRESHOLDAREAS];
                        threshold = z11 + (z21-z11)*(i-r1)/(r2-r1);
                        break;
                    }
                }
            } else {
                //行
                for (k=0; k<THRESHOLDAREAS; k++) {
                    if ((i>=areaCenters[k*THRESHOLDAREAS][0]) && (i<=areaCenters[k*THRESHOLDAREAS+THRESHOLDAREAS][0])) {
                        break;
                    }
                }
                //列
                for (t=0; t<THRESHOLDAREAS; t++) {
                    if ((j>=areaCenters[t][1]) && (j<=areaCenters[t+1][1])) {
                        break;
                    }
                }
                //行
                c1 = areaCenters[k*THRESHOLDAREAS][0];
                c2 = areaCenters[k*THRESHOLDAREAS+THRESHOLDAREAS][0];
                //列
                r1 = areaCenters[t][1];
                r2 = areaCenters[t+1][1];
                //计算四个点的值
                z11 = thresholds[k*THRESHOLDAREAS+t];
                z12 = thresholds[k*THRESHOLDAREAS+t+1];
                z21 = thresholds[k*THRESHOLDAREAS+THRESHOLDAREAS+t];
                z22 = thresholds[k*THRESHOLDAREAS+THRESHOLDAREAS+t+1];
                bilinearInterpolation(c1, r1, c2, r2, i, j, z11, z12, z21, z22, &zii);
                threshold = zii;
            }
            data_out[j] = data_in[j]>=threshold ? 255 : 0;
        }
    }
    return dst;
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
