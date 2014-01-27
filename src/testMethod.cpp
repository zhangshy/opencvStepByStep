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
	
	/**
	  输入为灰度图像
	  灰度直方图均衡化
	  参考： http://zh.wikipedia.org/wiki/%E7%9B%B4%E6%96%B9%E5%9B%BE%E5%9D%87%E8%A1%A1%E5%8C%96
	*/
	Mat histogramEqualizate(const Mat src) {
		//取得像素点的行列数和通道数
		int nrow = src.rows;
		int ncol = src.cols;
		int channel = src.channels();
		Mat dst(nrow, ncol, CV_8UC1);	//直方图均衡化后的图像
		if (channel != 1) {
			cout << "请输入单通道灰度图像，当前图像为channel is: " << channel << endl;
			return dst;
		}
		int pixelValue[256] = {0};	//统计各灰度值出现次数
		int cdfPixelValue[256] = {0};	//累计分布函数
		int i=0, j=0;
		for (i=0; i<nrow; i++) {
			const uchar* data_src = src.ptr<uchar>(i);	//输入的每一行
			for (j=0; j<ncol; j++) {
				pixelValue[data_src[j]]++;	//计算各灰度值出现的次数
			}
		}
		
		cdfPixelValue[0] = pixelValue[0];
		for (i=1; i<256; i++) {
			cdfPixelValue[i] = cdfPixelValue[i-1] + pixelValue[i];
		}
		//公式：(cdf(v)-cdfmin)*(L-1)/((M*N)-cdfmin) M N分别代表图像的长宽像素个数，L为灰度级数，本例图像8位深度，灰度级数为 2^8=256
		int totalPixelNum = nrow*ncol;	//总像素数
		int cdfMin = 0;
		for (i=0; i<256; i++) {
			if (cdfPixelValue[i]>0) {
				cdfMin = cdfPixelValue[i];
				cout << "cdfMin: " << cdfMin << ";;i: " << i << endl;
				break;
			}
		}
		int divNum = totalPixelNum - cdfMin;
		for (i=0; i<nrow; i++) {
			const uchar* data_in = src.ptr<uchar>(i);
			uchar* data_out = dst.ptr<uchar>(i);
			for (j=0; j<ncol; j++) {
				data_out[j] = ((cdfPixelValue[data_in[j]]-cdfMin)*255)/divNum;
			}
		}
		
		return dst;
	}
}

#ifdef _DEBUG
using namespace zsyTestMethod;
int main (int argc, char** argv) {
	cout << getTestMethodVersion() << endl;
	Mat image = imread("../out/Unequalized.jpg", CV_LOAD_IMAGE_COLOR);
	if (!image.data) {
		cout <<  "Could not open or find the image" << std::endl ;
		return -1;
	}

	Mat dst = changRGB2Gray(image);
//	imshow("Display", dst);
//	waitKey(0);
	Mat dst2 = histogramEqualizate(dst);
	imshow("Display", dst2);
	waitKey(0);
	return 0;
}
#endif
