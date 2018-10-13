#include <opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include"opencv2/imgproc/imgproc.hpp"
#include<math.h>
#include <iostream>

using namespace std;
using namespace cv;

int maind22(){
	Mat f_image, b_image;

	f_image = imread("123.png");
	b_image = imread("123b.png");

	//Mat gray_f;
	//cvtColor(f_image, gray_f, CV_BGR2GRAY);  //��ɫͼƬת���ɺڰ�ͼƬ



	if (!f_image.data||!b_image.data)
	{
		cout << "��ȡͼƬʧ�ܣ�" << endl;
		return -1;
	}

	if (f_image.rows != b_image.rows || f_image.cols != b_image.cols)
		cout << "ǰ��ͼ���ͼ�ߴ粻�����޷������" << endl;

	Mat t_image = Mat::zeros(f_image.size(), f_image.type());

	namedWindow("<Ч��ͼ����>", WINDOW_AUTOSIZE);

	double sum = 0.0;

	for (int y = 0; y < f_image.rows; y++)
	{
		for (int x = 0; x < f_image.cols; x++)
		{
			sum = 0.0;
			for (int c = 0; c < 3; c++)
			{
				sum += pow((f_image.at<Vec3b>(y, x)[c] - b_image.at<Vec3b>(y, x)[c]), 2);	
			}
			sum = sqrt(sum);
			if (sum >= 75) {
				for (int c = 0; c < 3; c++)
				{
					t_image.at<Vec3b>(y, x)[c] = saturate_cast < uchar>(255);
				}
			}
			else {
				for (int c = 0; c < 3; c++)
				{
					t_image.at<Vec3b>(y, x)[c] = saturate_cast < uchar>(0);
				}
			}				
		}
	}

	//cvtColor(t_image, t_image, CV_BGR2GRAY);
	imshow("<Ч��ͼ����>", t_image);
	waitKey(0);
	return 0;
}