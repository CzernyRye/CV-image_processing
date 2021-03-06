#include <opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include"opencv2/imgproc/imgproc.hpp"
#include<math.h>
#include <iostream>

using namespace std;
using namespace cv;

static void ContrastAndBright(int, void *);

int c_p;
int b_value;
Mat s_image, t_imag;

int main()
{
	s_image = imread("22.png");

	if (!s_image.data)
	{
		cout << "图片读取失败！" << endl;
		return false;
	}
	t_imag = Mat::zeros(s_image.size(), s_image.type());

	c_p = 20;
	b_value = 100;

	namedWindow("效果图窗口", WINDOW_AUTOSIZE);

	createTrackbar("对比度", "效果图窗口", &c_p, 200, ContrastAndBright);
	createTrackbar("亮  度", "效果图窗口", &b_value, 200, ContrastAndBright);

	ContrastAndBright(c_p, 0);
	ContrastAndBright(b_value, 0);

	waitKey(0);
	return 0;

}

static void ContrastAndBright(int, void *)
{

	namedWindow("窗口02", WINDOW_AUTOSIZE);
	// g_dstImage(i,j) =a*g_srcImage(i,j) + b

	for (int y = 0; y < s_image.rows; y++)
	{
		for (int x = 0; x < s_image.cols; x++)
		{
			for (int c = 0; c < 3; c++)
			{
				//0<=c_p<=200
				double t = ((s_image.at<Vec3b>(y, x)[c] - 127) / 225.00)*c_p*0.1;
				t_imag.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(s_image.at<Vec3b>(y, x)[c] * ((1.00 / (1.00 + exp(-t))) + 0.3) + b_value - 100);
				//int t = s_image.at<Vec3b>(y, x)[c];
				//t_imag.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(t * ((c_p*0.1 / (1.00 + exp(-t))) + 1));
			}
		}
	}
	imshow("窗口02", s_image);
	imshow("效果图窗口", t_imag);
}