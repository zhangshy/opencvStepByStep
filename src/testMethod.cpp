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

	//像素点处理
	void perrgb2hsv(float r, float g, float b, float* h, float* s, float* v) {
		// r,g,b values are from 0 to 1
		// h = [0,360], s = [0,1], v = [0,1]
		// if s == 0, then h = -1 (undefined)
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

	//参考http://zh.wikipedia.org/wiki/HSL%E5%92%8CHSV%E8%89%B2%E5%BD%A9%E7%A9%BA%E9%97%B4#.E4.BB.8ERGB.E5.88.B0HSL.E6.88.96HSV.E7.9A.84.E8.BD.AC.E6.8D.A2
	Mat rgb2hsv(const Mat src) {
		int nrow = src.rows;
		int ncol = src.cols;
		Mat tmpMat;
		tmpMat = Mat::zeros(nrow, ncol, CV_32FC3);	//转换为float类型，hsv是float型
		src.convertTo(tmpMat, CV_32FC3, 1/255.0);

		Mat dst;
		dst = Mat::zeros(nrow, ncol, CV_32FC3);
		int i=0, j=0;
		float r, g, b;
		float h, s, v;
		for (i=0; i<nrow; i++) {
			for (j=0; j<ncol; j++) {
				Vec3f bgrSrc = tmpMat.at<Vec3f>(i, j);	//BGR顺序排列
				b = bgrSrc[0];
				g = bgrSrc[1];
				r = bgrSrc[2];	
				perrgb2hsv(r, g, b, &h, &s, &v);
				Vec3f &bgrDst = dst.at<Vec3f>(i, j);
				bgrDst[0] = h;
				bgrDst[1] = s;
				bgrDst[2] = v;				
			}
		}
		return dst;
	}

	//像素点处理
	void perhsv2rgb(float h, float s, float v, float* r, float* g, float* b) {
		int i;
		float f, p, q, t;

		if( s == 0 )
		{
		// achromatic (grey)
		    *r = *g = *b = v;
		    return;
		}

		h /= 60; // sector 0 to 5
		i = floor( h );
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

	Mat hsv2rgb(const Mat src) {
		int nrow = src.rows;
		int ncol = src.cols;
		Mat tmpMat;
		tmpMat = Mat::zeros(nrow, ncol, CV_32FC3);

		Mat dst;
		dst = Mat::zeros(nrow, ncol, CV_8UC3);
		int i=0, j=0;
		float r, g, b;
		float h, s, v;
		for (i=0; i<nrow; i++) {
			for (j=0; j<ncol; j++) {
				Vec3f bgrSrc = src.at<Vec3f>(i, j);	
				h = bgrSrc[0];
				s = bgrSrc[1];
				v = bgrSrc[2];	
				perhsv2rgb(h, s, v, &r, &g, &b);
				Vec3f &bgrDst = tmpMat.at<Vec3f>(i, j);	//BGR顺序排列
				bgrDst[0] = b;
				bgrDst[1] = g;
				bgrDst[2] = r;				
			}
		}
		tmpMat.convertTo(dst, CV_8UC3, 255);
		return dst;
	}

	//RGB图像直方图均衡，在HSV模式下将V均衡化
	Mat rgbHistogramEqualizate(const Mat src) {
		int nrow = src.rows;
		int ncol = src.cols;
		//1. 将RGB转化为HSV，CV_32FC3，v:[0, 1]
		Mat hsvSrc = rgb2hsv(src);
		//2. 将V通道均衡，提取V通道：[0, 255]
		int i=0, j=0;
		Mat vSrc(nrow, ncol, CV_8UC1);
		for (i=0; i<nrow; i++) {
			uchar* data_out = vSrc.ptr<uchar>(i);
			for (j=0; j<ncol; j++) {
				Vec3f hsv = hsvSrc.at<Vec3f>(i, j);
				data_out[j] = hsv[2]*255;
			}
		}
		//V通道直方图均衡化
		Mat vDst = histogramEqualizate(vSrc);
		//将V通道放入hsv中, v: [0, 1]
		for (i=0; i<nrow; i++) {
			uchar* data_out = vDst.ptr<uchar>(i);
			for (j=0; j<ncol; j++) {
				Vec3f &hsv = hsvSrc.at<Vec3f>(i, j);
				hsv[2] = data_out[j]/255.0;
			}
		}
		//3. 将HSV转化为RGB
		Mat dst = hsv2rgb(hsvSrc);
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

	Mat tmp = rgb2hsv(image);
	imshow("Display tmp", tmp);
//	waitKey(0);
	Mat dst = hsv2rgb(tmp);
	imshow("Display dst", dst);
	waitKey(0);
	return 0;
}
#endif
