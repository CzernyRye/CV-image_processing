#include<opencv2/core/saturate.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include<iostream>
#include<cmath>
using namespace std;
using namespace cv;


void MyGaussianFilter(int pos, void* usrdata)
{ 
	const Mat SRC = *(Mat*)(usrdata);
	Mat src;
	SRC.copyTo(src);
	double sigma = pos * 0.01;
	Mat dst = Mat::zeros(src.rows, src.cols, src.type());

	int ksize = sigma * 6 - 1;
	if (ksize < 3) {
		cout << "ksize is too small!error" << endl;
		return;
	}
	int iRes = ksize % 2;
	if (iRes == 0) {
		ksize += 1;
	}
	
	CV_Assert(src.channels() == 1 || src.channels() == 3); // ֻ����ͨ��������ͨ��ͼ��
	// ����һά�ĸ�˹�˲�ģ���
	double *GS_filter = new double[ksize];
	//�õ�ǰ���ϵ����ĸ���ڹ�һ��
	double sum = 0;
	int half = ksize / 2;
	//��ʼ���˲���
	for (int i = 0; i < ksize; i++)
	{
		// ֻ�����ָ�����֣���˹����ǰ�ĳ������Բ��ü��㣬���ڹ�һ���Ĺ����и���ȥ
		//�ԣ�half,half��Ϊ���Ľ�������ϵ���м���
		double g = exp(-(i - half) * (i - half) / (2 * sigma * sigma));
		sum += g;
		GS_filter[i] = g;
	}
	// ��һ��
	for (int i = 0; i < ksize; i++)
		GS_filter[i] /= sum;

	// ��ģ��Ӧ�õ�ͼ����
	int border = ksize / 2;//ksize������������Ļ����ܻ��bug
	//��Ե���������Ե
	//BORDER_REFLECT_101���ԳƷ���Ҳ���������Ե����Ϊ��Գƽ�������
	copyMakeBorder(src, dst, border, border, border, border, BorderTypes::BORDER_CONSTANT);//BORDER_REFLECT_101BORDER_REPLICATE

	int channels = dst.channels();
	int rows = dst.rows - border;
	int cols = dst.cols - border;
	// ˮƽ����
	for (int i = border; i < rows; i++)
	{
		for (int j = border; j < cols; j++)
		{
			double sum[3] = { 0 };
			for (int k = -border; k <= border; k++)
			{
				//�Ե�ͨ������ͨ��ͼ��ֱ���
				if (channels == 1)
				{
					sum[0] += GS_filter[border + k] * dst.at<uchar>(i, j + k); // �в��䣬�б仯������ˮƽ����ľ��
				}
				else if (channels == 3)
				{
					Vec3b rgb = dst.at<Vec3b>(i, j + k);
					sum[0] += GS_filter[border + k] * rgb[0];
					sum[1] += GS_filter[border + k] * rgb[1];
					sum[2] += GS_filter[border + k] * rgb[2];
				}
			}
			//�����ؽ��й�������������
			for (int k = 0; k < channels; k++)
			{
				if (sum[k] < 0)
					sum[k] = 0;
				else if (sum[k] > 255)
					sum[k] = 255;
			}
			if (channels == 1)
				src.at<uchar>(i- border, j- border) = static_cast<uchar>(sum[0]);
			else if (channels == 3)
			{
				Vec3b rgb = { static_cast<uchar>(sum[0]), static_cast<uchar>(sum[1]), static_cast<uchar>(sum[2]) };
				src.at<Vec3b>(i- border, j- border) = rgb;
			}
		}
	}
	// ��ֱ����
	for (int i = border; i < rows; i++)
	{
		for (int j = border; j < cols; j++)
		{
			double sum[3] = { 0 };
			for (int k = -border; k <= border; k++)
			{
				if (channels == 1)
				{
					sum[0] += GS_filter[border + k] * dst.at<uchar>(i + k, j); // �в��䣬�б仯����ֱ����ľ��
				}
				else if (channels == 3)
				{
					Vec3b rgb = dst.at<Vec3b>(i + k, j);
					sum[0] += GS_filter[border + k] * rgb[0];
					sum[1] += GS_filter[border + k] * rgb[1];
					sum[2] += GS_filter[border + k] * rgb[2];
				}
			}
			for (int k = 0; k < channels; k++)
			{
				if (sum[k] < 0)
					sum[k] = 0;
				else if (sum[k] > 255)
					sum[k] = 255;
			}
			if (channels == 1)
				src.at<uchar>(i- border, j- border) = static_cast<uchar>(sum[0]);
			else if (channels == 3)
			{
				Vec3b rgb = { static_cast<uchar>(sum[0]), static_cast<uchar>(sum[1]), static_cast<uchar>(sum[2]) };
				src.at<Vec3b>(i- border, j- border) = rgb;
			}
		}
	}
	delete[] GS_filter;
	imshow("��˹ƽ��ͼƬ", src);
}

void MyIntegralImage(const Mat &SRC,int width,int height, int *Integral) {

	Mat src;
	SRC.copyTo(src);
	
	int channels = src.channels();

	if (channels == 1) {
		cout << "1ͨ��ͼƬ��ֵ�˲�..." << endl;
		int *prow = new int[width];
		memset(prow, 0, sizeof(int)*(width));

		for (int s = 0; s < width + 1; s++) {
			Integral[s] = 0;
		}
		Integral += (width + 1);
		for (int yi = 1; yi < height+1; ++yi,Integral += (width+1)) {
		
			//�Ե�һ������ֵ��������	
			
			Integral[0] = 0;		
			prow[0] += src.at<uchar>(yi - 1, 0);
			
			Integral[1] = prow[0];
			
			for (int xi = 2; xi < width+1; ++xi)
			{
				prow[xi-1] += src.at<uchar>(yi-1,xi-1);
				Integral[xi] = Integral[xi - 1] + prow[xi-1];
			}
		}
		delete[]prow;
	}
	else if (channels == 3) {
		cout << "3ͨ��ͼƬ��ֵ�˲�..." << endl;

		int *prow = new int[width*3];
		memset(prow, 0, sizeof(int)*width*3);

		for (int s = 0; s < 3*(width + 1); s++) {
			Integral[s] = 0;
		}
		Integral += 3*(width + 1);
		
		for (int yi = 1; yi < height+1; ++yi, Integral += 3 * (width + 1)) {

			//�Ե�һ������ֵ��������
			Integral[0] = 0;
			Integral[1] = 0;
			Integral[2] = 0;

			Vec3b rgb = src.at<Vec3b>(yi-1, 0);
			prow[0] += rgb[0];
			prow[1] += rgb[1];
			prow[2] += rgb[2];
		
			Integral[3] = prow[0];
			Integral[4] = prow[1];
			Integral[5] = prow[2];
			
			for (int xi = 2; xi < width+1; ++xi)
			{
				rgb = src.at<Vec3b>(yi-1, xi-1);
				prow[3*(xi-1)+0] += rgb[0];
				prow[3*(xi-1)+1] += rgb[1];
				prow[3*(xi-1)+2] += rgb[2];

				Integral[3 * xi + 0] = Integral[3 * (xi - 1) + 0] + prow[3 * (xi-1) + 0];
				Integral[3 * xi + 1] = Integral[3 * (xi - 1) + 1] + prow[3 * (xi-1) + 1];
				Integral[3 * xi + 2] = Integral[3 * (xi - 1) + 2] + prow[3 * (xi-1) + 2];
			}
		}		
		delete[]prow;
	}	
}

void MyMeanFliter(int w_size, void* usrdata) {

	if (w_size < 3) {
		cout << "ksize is too small!error" << endl;
		return;
	}
	int iRes = w_size % 2;
	if (iRes == 0) {
		w_size += 1;
	}

	const Mat SRC = *(Mat*)(usrdata);
	Mat src;
	SRC.copyTo(src);
	int border = w_size / 2;//ksize������������Ļ����ܻ��bug
	//��Ե���������Ե
	//BORDER_REFLECT_101���ԳƷ���Ҳ���������Ե����Ϊ��Գƽ�������
	copyMakeBorder(SRC, src, border, border, border, border, BorderTypes::BORDER_REFLECT_101);//BORDER_REFLECT_101BORDER_REPLICATEBORDER_CONSTANT

	int width = src.cols;
	int height = src.rows;

	cout << width << ' ' << height << ' ' << endl;

	//���ڴ洢����ͼ
	int *Integral;
	int channels = src.channels();
	//Ŀ��ͼ��
	Mat dst = Mat::zeros(SRC.size(), SRC.type());

	if (channels == 1) {
		cout << "��ͨ��ͼƬ��ֵ�˲�..." << endl;
		Integral = new int[(width+1)*(height+1)];
		MyIntegralImage(src, width,height,Integral);
		int *p = Integral;
	
		int guiyi = (2 * border + 1)*(2 * border + 1);

		for (int yi = 0; yi < dst.rows; ++yi) {
			for (int xi = 0; xi < dst.cols; ++xi) {
				/*int arr[4][2] = { 0 };
				arr[0][0] = yi;
				arr[0][1] = xi;

				arr[1][0] = yi + border + border+1;
				arr[1][1] = xi;

				arr[2][0] = yi;
				arr[2][1] = xi + border + border+1;

				arr[3][0] = arr[1][0];
				arr[3][1] = arr[2][1];*/

				int s1 = *(Integral + yi * (width + 1) + xi);
				int s2 = *(Integral + (yi + border + border + 1)*(width + 1) + xi);
				int s3 = *(Integral + yi * (width + 1) + xi + border + border + 1);
				int s4 = *(Integral + (yi + border + border + 1)*(width + 1) + (xi + border + border + 1));
				
				int gole = (s4 + s1 - s2 - s3) / guiyi;

				if (gole < 0)
					gole = 0;
				else if(gole > 255)
					gole = 255;

				dst.at<uchar>(yi,xi) = static_cast<uchar>(gole);
			}
		}
	}
	else if (channels == 3) {

		cout << "��ͨ��ͼƬ��ֵ�˲�..." << endl;
		Integral = new int[(width + 1)*(height + 1) * 3];
		MyIntegralImage(src, width, height,Integral);

		int guiyi = (2 * border + 1)*(2 * border + 1);


		for (int yi = 0; yi < dst.rows; ++yi) {
			for (int xi = 0; xi < dst.cols; ++xi) {
				int s11 = *(Integral + yi * 3 * (width + 1) + xi * 3);
				int s12 = *(Integral + yi * 3 * (width + 1) + xi * 3 + 1);
				int s13 = *(Integral + yi * 3 * (width + 1) + xi * 3 + 2);

				int s21 = *(Integral + (yi + border + border + 1) * 3 * (width + 1) + xi * 3);
				int s22 = *(Integral + (yi + border + border + 1) * 3 * (width + 1) + xi * 3 + 1);
				int s23 = *(Integral + (yi + border + border + 1) * 3 * (width + 1) + xi * 3 + 2);

				int s31 = *(Integral + yi * 3 * (width + 1) + (xi + border + border + 1) * 3);
				int s32 = *(Integral + yi * 3 * (width + 1) + (xi + border + border + 1) * 3 + 1);
				int s33 = *(Integral + yi * 3 * (width + 1) + (xi + border + border + 1) * 3 + 2);

				int s41 = *(Integral + (yi + border + border + 1) * 3 * (width + 1) + (xi + border + border + 1) * 3);
				int s42 = *(Integral + (yi + border + border + 1) * 3 * (width + 1) + (xi + border + border + 1) * 3 + 1);
				int s43 = *(Integral + (yi + border + border + 1) * 3 * (width + 1) + (xi + border + border + 1) * 3 + 2);

				int gole[3];
				 gole[0] = (s41 + s11 - s21 - s31) / guiyi;
				 gole[1] = (s42 + s12 - s22 - s32) / guiyi;
				 gole[2] = (s43 + s13 - s23 - s33) / guiyi;
				for (int i = 0; i < 3; i++) {
					if (gole[i] < 0)
						gole[i] = 0;
					else if (gole[i] > 255)
						gole[i] = 255;
				}		
				Vec3b rgb = { static_cast<uchar>(gole[0]), static_cast<uchar>(gole[1]), static_cast<uchar>(gole[2]) };
				dst.at<Vec3b>(yi, xi) = rgb;			
			}
		}
	}
	imshow("��ֵ�˲�ͼƬ", dst);
}


int main() {
	
	Mat image;
	image = imread("1234.png",0);
	if (!image.data)
	{
		cout << "ͼƬ��ȡʧ�ܣ�" << endl;
		return -1;
	}	
	cout << "ͼ���˹ƽ������ʼ..." << endl;
	namedWindow("ԭͼ", WINDOW_AUTOSIZE);
	imshow("ԭͼ", image);
	cout << image.cols << ' ' << image.rows << ' ' << endl;

	int sigma = 80;
	namedWindow("��˹ƽ��ͼƬ", WINDOW_AUTOSIZE);
	createTrackbar("sigma*0.01", "��˹ƽ��ͼƬ", &sigma, 500, MyGaussianFilter,&image);
	MyGaussianFilter(80, &image);

	int win_size = 3;
	namedWindow("��ֵ�˲�ͼƬ", WINDOW_AUTOSIZE);
	createTrackbar("window_size", "��ֵ�˲�ͼƬ", &win_size, 30, MyMeanFliter, &image);
	MyMeanFliter(3, &image);
	
	waitKey(0);
	return 0;
}