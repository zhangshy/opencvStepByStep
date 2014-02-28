#include <iostream>
#include "histRedistribute.h"
using namespace std;

/**
* 获取矩阵直方图均衡的变换函数，变换函数为[256]的数组，通过查表得到变换值
*@param bUseCLAHE true将原直方图超出阈值的部分先平均分配后在计算分布函数
*/
bool getHistEqualConvert(const Mat src, uchar* convertBuf, bool bUseCLAHE)
{
    int nrow = src.rows;
    int ncol = src.cols;
    int channel = src.channels();
    if (channel != 1) {
        cout << "请输入单通道矩阵，当前图像为channel is: " << channel << endl;
        return false;
    }
    int pixelValue[256] = {0};	//统计各灰度值出现次数
    int cdfPixelValue[256] = {0};	///计算累计分布函数cdf
    int i=0, j=0;
    for (i=0; i<nrow; i++) {
        const uchar* data_src = src.ptr<uchar>(i);	//输入的每一行
        for (j=0; j<ncol; j++) {
            pixelValue[data_src[j]]++;	//计算各灰度值出现的次数
        }
    }

    if (bUseCLAHE) {
        /** 阈值=climit*nrow*ncol/256，climit=[1, 256]，现在用2测试使用limitNum = nrow*ncol>>7 */
        int limitNum = nrow*ncol>>7;
        int excessNums = 0; //超出部分阈值部分的总和
        int perAddNum = 0;
//            cout << "limitNum:::" << limitNum << ";;total sum: " << nrow*ncol << endl;
        for (i=0; i<256; i++) {
            if (pixelValue[i]>limitNum) {
                    excessNums += (pixelValue[i]-limitNum);
                    pixelValue[i] = limitNum;
            }
        }
        perAddNum = excessNums >> 8;
//            cout << "excessNums:::" << excessNums << ";;;perAddNum: " << perAddNum << endl;
        for (i=0; i<256; i++) {
            pixelValue[i] += perAddNum;
        }
    }
    cdfPixelValue[0] = pixelValue[0];
    for (i=1; i<256; i++) {
        cdfPixelValue[i] = cdfPixelValue[i-1] + pixelValue[i];
    }
    /** 变换函数公式：(cdf(v)-cdfmin)*(L-1)/((M*N)-cdfmin) M N分别代表图像的长宽像素个数，L为灰度级数，本例图像8位深度，灰度级数为 2^8=256 */
    int totalPixelNum = nrow*ncol;	//总像素数
    int cdfMin = 0;
    /**
    * 对于Mat中出现的点直接利用公式计算对应变换值
    * 对于Mat中未出现的点利用线性插值法计算变换值？？
    */
    for (i=0; i<256; i++) {
        if (cdfPixelValue[i]>0) {
            cdfMin = cdfPixelValue[i];
//				cout << "cdfMin: " << cdfMin << ";;i: " << i << endl;
            break;
        }
    }
    int divNum = totalPixelNum - cdfMin;
    /** appearData保存出现的点，appearNums出现的点的个数*/
    uchar appearData[256] = {0};
    uchar appearNums = 0;
    for (i=cdfMin; i<256; i++) {
        convertBuf[i] = ((cdfPixelValue[i]-cdfMin)*255)/divNum;
    }

#if 0
    cout << "------------------------" << endl;
    for (i=0; i<256; i++) {
        printf("%d ", convertBuf[i]);
    }
    cout << "------------------------" << endl;
#endif
    return true;
}

bool getAutoCutConvert(const Mat src, uchar* convertBuf) {
    int nrow = src.rows;
    int ncol = src.cols;
    int channel = src.channels();
    if (channel != 1) {
        cout << "请输入单通道矩阵，当前图像为channel is: " << channel << endl;
        return false;
    }
    float lowCutPercent=0.001;
    float highCutPercent=0.001;
    int hist[256] = {0};    //统计直方图
    int pixelAmount = nrow*ncol;    //所有像素的数目
    uchar minValue=0, maxValue=255;
    int i, j;
    for (i=0; i<nrow; i++) {
        const uchar* data_src = src.ptr<uchar>(i);
        for (j=0; j<ncol; j++) {
            hist[data_src[j]]++;
        }
    }
    int sum=0;
    int sumMin = pixelAmount*lowCutPercent;
    for (i=0; i<256; i++) {
        sum += hist[i];
        if (sum>=sumMin) {
            minValue = i;
            break;
        }
    }
    sum = 0;
    int sumMax = pixelAmount*highCutPercent;
    for (i=255; i>=0; i--) {
        sum += hist[i];
        if (sum>=sumMax) {
            maxValue = i;
            break;
        }
    }
//    cout << "maxValue: " << (int)maxValue << ";minValue: " << (int)minValue << endl;
    if (maxValue > minValue) {
        for (i=0; i<256; i++) {
            if (i<minValue)
                convertBuf[i] = 0;
            else if (i>maxValue)
                convertBuf[i] = 255;
            else
                convertBuf[i] = 255*(i-minValue)/(maxValue-minValue);
        }
    }else if (maxValue==minValue) {
        for (i=0; i<256; i++) {
            convertBuf[i] = minValue;
        }
    }else {
        cout << "maxValue < minValue" << endl;
        return false;
    }
    #if 0
    cout << "------------------------" << endl;
    for (i=0; i<256; i++) {
        printf("%d ", convertBuf[i]);
    }
    cout << "------------------------" << endl;
    #endif
    return true;
}
