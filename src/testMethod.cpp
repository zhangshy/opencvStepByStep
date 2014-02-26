#include <iostream>
#include "testMethod.h"
#include "mathTest.h"

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

	/**
	* 输入为单通道Mat, 直方图均衡化
    * 参考： http://zh.wikipedia.org/wiki/%E7%9B%B4%E6%96%B9%E5%9B%BE%E5%9D%87%E8%A1%A1%E5%8C%96
    * 修改：因为变换函数是由[0, 255]-->[0, 255]，可以先创建数组并计算对应值，通过查表即可获得对应变换值
	*/
	Mat histogramEqualizate(const Mat src) {
		//取得像素点的行列数和通道数
		int nrow = src.rows;
		int ncol = src.cols;
		int channel = src.channels();
		Mat dst(nrow, ncol, CV_8UC1);	//直方图均衡化后的图像
		if (channel != 1) {
			cout << "请输入单通道矩阵，当前图像为channel is: " << channel << endl;
			return dst;
		}

        uchar convertBuf[256] = {0};
        getHistEqualConvert(src, convertBuf, false);
        int i=0, j=0;
		for (i=0; i<nrow; i++) {
			const uchar* data_in = src.ptr<uchar>(i);
			uchar* data_out = dst.ptr<uchar>(i);
			for (j=0; j<ncol; j++) {
//				data_out[j] = ((cdfPixelValue[data_in[j]]-cdfMin)*255)/divNum;
                data_out[j] = convertBuf[data_in[j]];
			}
		}

		return dst;
	}


	/**
	*@brief CLAHE自适应直方图均衡化
	*@param src 单通道的Mat
	*@param bIsCLAHE true使用CLAHE，false使用AHE
	* 1. 将图像分成8*8块
	* 2. 计算各块的变换函数
	*/
	Mat cladaptHistEqual(const Mat src, bool bIsCLAHE) {
		int nrow = src.rows;    //图像的高度
		int ncol = src.cols;    //列数,宽
		int channel = src.channels();
		Mat dst = Mat::zeros(nrow, ncol, CV_8UC1);
		if (channel != 1) {
			cout << "请输入单通道矩阵，当前图像为channel is: " << channel << endl;
			return dst;
		}
#if 0
        /** 测试Mat Rect的使用 */
		Mat tmpdst = histogramEqualizate(src(Rect(0, 0, ncol/4, nrow/4)));
		tmpdst.copyTo(dst(Rect(0, 0, ncol/4, nrow/4)));
		return dst;
#endif
		//分成8*8块
		int perRow = nrow/PERNUM;
		int perCol = ncol/PERNUM;
		/** 对应块的变换函数 */
		uchar histEqualConvertBufs[PERNUM*PERNUM][256] = {0};
		/** 对应块的中心点行列(x,y) */
		int areaCenters[PERNUM*PERNUM][2] = {0};
//		Mat tmpSrc = Mat::zeros(perRow, perCol, CV_8UC1);
		int i=0, j=0;
		int r=0, c=0;
		for (i=0; i<PERNUM; i++) {
			for (j=0; j<PERNUM; j++) {
//        cout << "i: " << i << ";j: " << j << endl;
        /** 注意赋值top-left = 50,50 size=100,50: Rect(50,50,100,50)
        * size为Size(cols, rows)，其第一个参数为cols，即图像的宽度。即Mat大小先是高度然后是宽度，而size大小显示宽度然后是高度。
        */
                Mat tmpSrc = src(Rect(j*perCol, i*perRow, perCol, perRow));   ///使用数据副本:Mat image2 = image1(Rect(2,2,99,99)).clone();
//                Mat tmpSrc = src(Rect(0, 399, 50, 57));
				areaCenters[i*PERNUM+j][0] = i*perRow + perRow/2;
				areaCenters[i*PERNUM+j][1] = j*perCol + perCol/2;
				getHistEqualConvert(tmpSrc, histEqualConvertBufs[i*PERNUM+j], bIsCLAHE);
			}
		}
		int uRIndex = PERNUM - 1;   //右上角upper right corner
		int lLIndex = (PERNUM-1)*PERNUM;    //左下角lower left corner
		int lRIndex = PERNUM*PERNUM-1;  //右下角lower right corner
		int r1=0, r2=0, c1=0, c2=0;
		int k=0, t=0;
		uchar z11=0, z12=0, z21=0, z22=0, zii=0;
		for (i=0; i<nrow; i++) {
		    const uchar* data_in = src.ptr<uchar>(i);
		    uchar* data_out = dst.ptr<uchar>(i);
            for (j=0; j<ncol; j++) {
                if ((i<=areaCenters[0][0]) && (j<=areaCenters[0][1])) {
                    data_out[j] = histEqualConvertBufs[0][data_in[j]];  ///左上角使用所在矩阵编号0的变换函数
                } else if ((i<=areaCenters[uRIndex][0]) && (j>=areaCenters[uRIndex][1])) {
                    data_out[j] = histEqualConvertBufs[uRIndex][data_in[j]];  ///右上角使用所在矩阵编号uRIndex的变换函数
                } else if ((i>=areaCenters[lLIndex][0]) && (j<=areaCenters[lLIndex][1])) {
                    data_out[j] = histEqualConvertBufs[lLIndex][data_in[j]];    ///左下角使用所在矩阵编号lLIndex的变换函数
                } else if ((i>=areaCenters[lRIndex][0]) && (j>=areaCenters[lRIndex][1])) {
                    data_out[j] = histEqualConvertBufs[lRIndex][data_in[j]];    ///右下角使用所在矩阵编号lRIndex的变换函数
                } else if ((i<=areaCenters[0][0]) && (j>=areaCenters[0][1]) && (j<=areaCenters[uRIndex][1])) {
                    for (k=0; k<uRIndex; k++) {
                        if ((j>=areaCenters[k][1]) && (j<=areaCenters[k+1][1])) {
                            //使用线性插值，计算两点和对应的值
                            c1 = areaCenters[k][1];
                            c2 = areaCenters[k+1][1];
                            z11 = histEqualConvertBufs[k][data_in[j]];
                            z12 = histEqualConvertBufs[k+1][data_in[j]];
                            break;
                        }
                    }
                    data_out[j] = z11 + (z12-z11)*(j-c1)/(c2-c1);
                } else if ((i>=areaCenters[lLIndex][0]) && (j>=areaCenters[lLIndex][1]) && (j<=areaCenters[lRIndex][1])) {
                    for (k=lLIndex; k<lRIndex; k++) {
                        if ((j>=areaCenters[k][1]) && (j<=areaCenters[k+1][1])) {
                            //使用线性插值，计算两点和对应的值
                            c1 = areaCenters[k][1];
                            c2 = areaCenters[k+1][1];
                            z11 = histEqualConvertBufs[k][data_in[j]];
                            z12 = histEqualConvertBufs[k+1][data_in[j]];
                            break;
                        }
                    }
                    data_out[j] = z11 + (z12-z11)*(j-c1)/(c2-c1);
                } else if ((j<=areaCenters[0][1]) && (i>=areaCenters[0][0]) && (i<=areaCenters[lLIndex][0])) {
                    for (k=0; k<lLIndex; k+=PERNUM) {
                        if ((i>=areaCenters[k][0]) && (i<=areaCenters[k+PERNUM][0])) {
                            r1 = areaCenters[k][0];
                            r2 = areaCenters[k+PERNUM][0];
                            z11 = histEqualConvertBufs[k][data_in[j]];
                            z21 = histEqualConvertBufs[k+PERNUM][data_in[j]];
                            data_out[j] = z11 + (z21-z11)*(i-r1)/(r2-r1);
                            break;
                        }
                    }
                } else if ((j>=areaCenters[uRIndex][1]) && (i>=areaCenters[uRIndex][0]) && (i<=areaCenters[lRIndex][0])) {
                    for (k=uRIndex; k<lRIndex; k+=PERNUM) {
                        if ((i>=areaCenters[k][0]) && (i<=areaCenters[k+PERNUM][0])) {
                            r1 = areaCenters[k][0];
                            r2 = areaCenters[k+PERNUM][0];
                            z11 = histEqualConvertBufs[k][data_in[j]];
                            z21 = histEqualConvertBufs[k+PERNUM][data_in[j]];
                            data_out[j] = z11 + (z21-z11)*(i-r1)/(r2-r1);
                            break;
                        }
                    }
                } else {
                    //行
                    for (k=0; k<PERNUM; k++) {
                        if ((i>=areaCenters[k*PERNUM][0]) && (i<=areaCenters[k*PERNUM+PERNUM][0])) {
                            break;
                        }
                    }
                    //列
                    for (t=0; t<PERNUM; t++) {
                        if ((j>=areaCenters[t][1]) && (j<=areaCenters[t+1][1])) {
                            break;
                        }
                    }
                    //行
                    c1 = areaCenters[k*PERNUM][0];
                    c2 = areaCenters[k*PERNUM+PERNUM][0];
                    //列
                    r1 = areaCenters[t][1];
                    r2 = areaCenters[t+1][1];
                    //计算四个点的值
                    z11 = histEqualConvertBufs[k*PERNUM+t][data_in[j]];
                    z12 = histEqualConvertBufs[k*PERNUM+t+1][data_in[j]];
                    z21 = histEqualConvertBufs[k*PERNUM+PERNUM+t][data_in[j]];
                    z22 = histEqualConvertBufs[k*PERNUM+PERNUM+t+1][data_in[j]];
                    bilinearInterpolation(c1, r1, c2, r2, i, j, z11, z12, z21, z22, &zii);
                    data_out[j] = zii;
                }
//                cout << " x: " << xi << " y: " << yi << " x1: " << x1 << " x2: " << x2\
//                                                    << " y1: " << y1 << " y2: " << y2<< endl;

//                bilinearInterpolation(x1, y1, x2, y2, xi, yi, z11, z12, z21, z22, &zii);
//                bilinearInterpolation(x1*perRow+perRow/2, y1*perRow+perRow/2, x2*perRow+perRow/2, y2*perRow+perRow/2, i, j, z11, z12, z21, z22, &zii);
            }
		}
		return dst;
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

	/**
	   RGB图像直方图均衡，在HSV模式下将V均衡化
	   测试后发现有些颜色变浅或变浅了？？效果不是很理想
	*/
	Mat rgbHistogramEqualizate(const Mat src, int method) {
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
		Mat vDst;
	    switch (method) {
			case HE:
				vDst = histogramEqualizate(vSrc);
				break;
            case AHE:
                vDst = cladaptHistEqual(vSrc, false);
                break;
			case CLAHEMETHOD:
				vDst = cladaptHistEqual(vSrc, true);
				break;
/*
            case CLAHEMETHOD: {
                //使用opencv自带的CLAHE进行测试
                Ptr<CLAHE> clahe = createCLAHE();
                clahe->setClipLimit(1.0);
				clahe->setTilesGridSize(Size(8, 8));
                clahe->apply(vSrc, vDst);
                break;
            }
*/
            case USEACE:
                vDst = useACE(vSrc, false);
                break;
            case USEACEWITHLSD:
                vDst = useACE(vSrc, true);
                break;
			default:
				vDst = histogramEqualizate(vSrc);
		}
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

	/**
	* 彩色图像均衡，转化为灰度图像均衡后按原比例还原为RGB图像
	* 效果好像还不如均衡hsv中的v通道？？
	*/
	Mat rgbHistogramEqualizateGray(const Mat src, int method) {
		int nrow = src.rows;
		int ncol = src.cols;
		int i=0, j=0;
		Mat graySrc;
#if 0
		//1. 转化为灰度图像
		graySrc = changRGB2Gray(src);
#else
		//将图像的3个通道相加后计算总体均衡
		graySrc = Mat::zeros(nrow, ncol, CV_8UC1);
		for (i=0; i<nrow; i++) {
			uchar* data_graySrc = graySrc.ptr<uchar>(i);
			for (j=0; j<ncol; j++) {
				Vec3b bgrSrc = src.at<Vec3b>(i, j);
				data_graySrc[j] = (bgrSrc[0]+bgrSrc[1]+bgrSrc[2]) / 3;
			}
		}
#endif
		//2. 灰度图像均衡
		Mat grayDst;
	    switch (method) {
			case HE:
				grayDst = histogramEqualizate(graySrc);
				break;
            case AHE:
				grayDst = cladaptHistEqual(graySrc, false);
				break;
			case CLAHEMETHOD:
				grayDst = cladaptHistEqual(graySrc, true);
				break;
			default:
				grayDst = histogramEqualizate(graySrc);
		}
		//将V通道放入hsv中, v: [0, 1]
		//3. 按照原图像RGB的比例还原回彩色图像
		Mat dst(nrow, ncol, CV_8UC3, Scalar(0, 0, 0));
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
		return dst;
	}

    /**
	*@brief 利用双线性插值法对图像进行缩放
	*
	* 1. 图像的四角上的点的值直接复制
	* 2. 四条边的值使用线性插值
	* 3. 中间的图像使用双线性插值
	*@param resize 缩放倍数
	*/
	Mat resizeMat(const Mat src, float resize)
	{
	    //原图像行列数，应为要取数组，数组索引最大值为数组长度-1
	    int nrowSrc = src.rows-1;
	    int ncolSrc = src.cols-1;
	    //目标图像行列数
        int nrow = src.rows*resize;
        int ncol = src.cols*resize;
        Mat dst = Mat::zeros(nrow, ncol, CV_8UC3);
        int i=0, j=0;
        for (i=0; i<nrow; i++) {
            for (j=0; j<ncol; j++) {
                //计算目标图像点在原图像对应的位置
                float srcRow = i/resize;
                float srcCol = j/resize;
                int x1 = srcRow;
                int x2 = x1+1;
                int y1 = srcCol;
                int y2 = y1+1;
                /** 注意使用双线性插值时，目标图像上点映射到原图像上时四个点值的获取 */
                if (x1>=nrowSrc) {
                    x1 = nrowSrc;
                    srcRow = nrowSrc;
                    x2 = nrowSrc;
                }
                if (y1>=ncolSrc) {
                    y1 = ncolSrc;
                    srcCol = ncolSrc;
                    y2 = ncolSrc;
                }
                Vec3b bgrSrc11 = src.at<Vec3b>(x1, y1);
//                cout << " x: " << nrowSrc << " y: " << ncolSrc << " x1: " << x1 << " y2: " << y2 << endl;
                Vec3b bgrSrc12 = src.at<Vec3b>(x1, y2);
                Vec3b bgrSrc21 = src.at<Vec3b>(x2, y1);
                Vec3b bgrSrc22 = src.at<Vec3b>(x2, y2);
                Vec3b &bgrDst = dst.at<Vec3b>(i, j);
                bilinearInterpolation(x1, y1, x2, y2, srcRow, srcCol, bgrSrc11[0], bgrSrc12[0], bgrSrc21[0], bgrSrc22[0], &bgrDst[0]);
                bilinearInterpolation(x1, y1, x2, y2, srcRow, srcCol, bgrSrc11[1], bgrSrc12[1], bgrSrc21[1], bgrSrc22[1], &bgrDst[1]);
                bilinearInterpolation(x1, y1, x2, y2, srcRow, srcCol, bgrSrc11[2], bgrSrc12[2], bgrSrc21[2], bgrSrc22[2], &bgrDst[2]);
            }
        }
        return dst;
	}

    /**
    * 计算图像标准差
    *@param src 单通道Mat
    *@param srcMean 平均值
    */
    float getMatStandardDeviation(const Mat src, uchar srcMean=0);
    float getMatStandardDeviation(const Mat src, uchar srcMean) {
        int nrow = src.rows;
        int ncol = src.cols;
        int i, j;
        long int varianceSum = 0;   //方差总和
        if (srcMean == 0) {
            srcMean = mean(src).val[0];
            cout << "计算平均值: " << srcMean << endl;
        }
        for (i=0; i<nrow; i++) {
            const uchar* data_in = src.ptr<uchar>(i);
            for (j=0; j<ncol; j++) {
                varianceSum += (data_in[j]-srcMean)*(data_in[j]-srcMean);
            }
        }
        varianceSum = varianceSum/(nrow*ncol);
        float ret = sqrt(varianceSum);  //开平方
//        cout << "标准差: " << ret << endl;
        return ret;
    }

	/**
	*@brief 使用局部标准差
	*
	* 1. 取区域平均值，大于平均值的更大、小于平均值的更小
	* 2. 使用局部标准差，D怎么取值合适？？CG的值有时太大
	*@param useLSD true 使用局部标准差
	* 参考：http://www.cnblogs.com/Imageshop/p/3324282.html
	*/
	Mat useACE(const Mat src, bool useLSD) {
        int n = 50; //局部矩阵大小(2n+1)*(2n+1)
        float c = 2.0;
        float max = 3.0;    //定义CG最大值
        uchar D = 0;    //全局平均值
        int nrow = src.rows;
		int ncol = src.cols;
        Mat dst = Mat::zeros(nrow, ncol, CV_8UC1);
        int i, j;
        if (useLSD) {
            D = mean(src).val[0];
        }
        for (i=0; i<nrow; i++) {
            const uchar* data_in = src.ptr<uchar>(i);
            uchar* data_out = dst.ptr<uchar>(i);
            for (j=0; j<ncol; j++) {
                /** 计算块的左上角和右下角的点 */
                int row1 = i-50;
                int col1 = j-50;
                int row2 = i+50;
                int col2 = j+50;
                col1 = col1<0 ? 0 : col1;
                row1 = row1<0 ? 0 : row1;
                col2 = col2>ncol ? ncol : col2;
                row2 = row2>nrow ? nrow : row2;
                Mat tmp = src(Rect(col1, row1, col2-col1, row2-row1));
                Scalar m = mean(tmp);
                uchar tmpM = m.val[0];
//                cout << "tmpM: " << tmpM << endl;
                if (useLSD) {
                    float standardDeviation = getMatStandardDeviation(tmp, tmpM);
                    c = D/standardDeviation;
                    cout << "c: " << c << endl;
                    if (c > max)
                        c = max;
                }
                int t = tmpM + c*(data_in[j]-tmpM);
                data_out[j] = t>255 ? 255 : (t<0 ? 0 : t);
            }
        }
        cout << "D: " << (int)D << endl;
        return dst;
	}
}


#ifdef _DEBUG
using namespace zsyTestMethod;
int main (int argc, char** argv) {
	cout << getTestMethodVersion() << endl;
	Mat image = imread("../out/test2.jpg", CV_LOAD_IMAGE_COLOR);
	if (!image.data) {
		cout <<  "Could not open or find the image" << std::endl ;
		return -1;
	}

	Mat dstHE = rgbHistogramEqualizate(image, HE);
	Mat dstAHE = rgbHistogramEqualizate(image, AHE);
	Mat dstCLAHE = rgbHistogramEqualizate(image, CLAHEMETHOD);
//	Mat dstResize = resizeMat(image, 1.7);
	imshow("Display HE", dstHE);
	imshow("Display AHE", dstAHE);
	imshow("Display CLAHE", dstCLAHE);
//	imshow("Display resize", dstResize);
	waitKey(0);
	return 0;
}
#endif
