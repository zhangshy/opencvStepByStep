/**
* 使用opencv的CLAHE算法，查看一下把彩色图像各通道累加计算均衡的效果
*/
#include <cv.h>
#include <highgui.h>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    Mat src = imread("../out/test2.jpg", CV_LOAD_IMAGE_COLOR);
    if (!src.data) {
        cout <<  "Could not open or find the src" << std::endl;
        return -1;
    }

    Mat graySrc;
    cvtColor(src, graySrc, CV_BGR2GRAY);

    //调用CLAHE
    Ptr<CLAHE> clahe = createCLAHE();
    clahe->setClipLimit(2.0);
    Mat grayDst;
    clahe->apply(graySrc, grayDst);

    Mat dst = Mat::zeros(src.size(), CV_8UC3);
    int nrow=src.rows, ncol=src.cols;
    int i, j;
    int tb=0, tg=0, tr=0;
    for (i=0; i<nrow; i++) {
        uchar* data_grayDst = grayDst.ptr<uchar>(i);
        uchar* data_graySrc = graySrc.ptr<uchar>(i);
        for (j=0; j<ncol; j++) {
            Vec3b bgrSrc = src.at<Vec3b>(i, j);
            Vec3b &bgrDst = dst.at<Vec3b>(i, j);
            //按照比例回复bgrSrc/data_graySrc = bgrDst/data_grayDst -->  bgrDst=bgrSrc/data_graySrc*data_grayDst
            if (data_graySrc[j] != 0) {
                tb = bgrSrc[0]*data_grayDst[j]/data_graySrc[j];
                tg = bgrSrc[1]*data_grayDst[j]/data_graySrc[j];
                tr = bgrSrc[2]*data_grayDst[j]/data_graySrc[j];
                bgrDst[0] = tb>255 ? 255 : (tb<0 ? 0 :tb);	//这里比较重要，消除颜色怪异的点
                bgrDst[1] = tg>255 ? 255 : (tg<0 ? 0 :tg);
                bgrDst[2] = tr>255 ? 255 : (tr<0 ? 0 :tr);
            }
        }
    }
    imshow("dst", dst);
    waitKey(0);
    return 0;
}
