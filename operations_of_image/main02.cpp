#include<iostream>  
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;


int mainff()
{
	Mat img = imread("a.png", -1);//-1ʹ�ܶ�ȡ��4��RGBA�е�Aͨ������Ϊalphaͨ��
	Mat mat(img.rows, img.cols, CV_8UC1);//���Դ���----8λ�޷��ŵĵ�ͨ��---�Ҷ�ͼƬ------grayImg
	for (int i = 0; i < img.rows; ++i)
	{
		for (int j = 0; j < img.cols; ++j)
		{
			mat.at<uchar>(i, j) = img.at<Vec4b>(i, j)[3];
		}
	}
	namedWindow("alphaͨ��ͼ��");
	imshow("alphaͨ��ͼ��", mat);
	waitKey();
	return 0;
}