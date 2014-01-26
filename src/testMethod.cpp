#include <iostream>
#include "testMethod.h"

using namespace std;

namespace zsyTestMethod {
	string getTestMethodVersion() {
		return "libtestMethod.so version0.1";
	}

	/**
	一般按加权的方法转换，R ， G ，B 的比一般为3：6：1将图片转化为灰度图片
	 */
	Mat changRGB2Gray(const Mat src) {
		//取得像素点的行列数和通道数
		int nrow = src.rows;
		int ncol = src.cols;
		int channels = src.channels();	//通道数,在OpenCV中，一张3通道图像的一个像素点是按BGR的顺序存储的
		Mat dst(nrow, ncol, CV_8UC1);
		int i=0, j=0;
		//typedef Vec<uchar, 3> Vec3b;
		//uchar* Mat::ptr(int i=0) i 是行号，返回的是该行数据的指针
		for (i=0; i<nrow; i++) {
			uchar* data_out = dst.ptr<uchar>(i);
			for (j=0; j<ncol; j++) {
				Vec3b bgrSrc = src.at<Vec3b>(i, j);	//取得像素点的RGB值, 在OpenCV中，一张3通道图像的一个像素点是按BGR的顺序存储的
				//data_out[j] = (bgrSrc[0]*11+bgrSrc[1]*59+bgrSrc[2]*30)/100;	//Gray=(R*30+G*59+B*11)/100
				data_out[j] = (bgrSrc[0]*28+bgrSrc[1]*151+bgrSrc[2]*77)>>8;		//移位方法
			}
		}
		return dst;
	}
}

#ifdef _DEBUG
using namespace zsyTestMethod;
int main (int argc, char** argv) {
	cout << getTestMethodVersion() << endl;
	Mat image = imread("../out/test.jpg", CV_LOAD_IMAGE_COLOR);
	if (!image.data) {
		cout <<  "Could not open or find the image" << std::endl ;
		return -1;
	}

	Mat dst = changRGB2Gray(image);
	imshow("Display", dst);
	waitKey(0);
	return 0;
}
#endif
