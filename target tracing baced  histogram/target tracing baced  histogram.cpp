#include <iostream>
#include <core/core.hpp>  
#include<opencv2/core/saturate.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include<cmath>
using namespace std;
using namespace cv;

Mat image; 
Mat imageCopy; //���ƾ��ο�ʱ��������ԭͼ��ͼ��
Mat targetImage;//��ȡ��Ŀ��ͼ��
bool leftButtonDownFlag=false; //�����������Ƶ��ͣ���ŵı�־λ
Point originalPoint; //���ο����
Point processPoint; //���ο��յ�
const char* videoPath = "c505.mp4";

//��һ���ֱ�����Ϊȫ�ֺ������������Ч��
int cccc = 0;//�ж�Ŀ�������
//H��Sͨ��
int channels[] = { 0, 1 };
int histSize[] = { 30, 32 };
float HRanges[] = { 0, 180 };
float SRanges[] = { 0, 256 };
const float *ranges[] = { HRanges, SRanges };

void onMouse(int event, int x, int y, int flags, void* ustc); //���ص�����
void getTarget();
void drawHist(Mat rectImage);
void tracing(const MatND srcHist);
double compHist(const MatND srcHist, Mat compareImage);

int main()
{
	getTarget();
	imshow("Target object", targetImage);
	cout << originalPoint << processPoint << endl;
	drawHist(targetImage);
	Mat srcHsvImage;
	cvtColor(targetImage, srcHsvImage, CV_BGR2HSV);
	//����H-Sֱ��ͼ���д���
	//���ȵ�����ֱ��ͼ�Ĳ���
	MatND srcHist;
	//����ԭͼֱ��ͼ�ļ���
	calcHist(&srcHsvImage, 1, channels, Mat(), srcHist, 2, histSize, ranges, true, false);
	//��һ��
	normalize(srcHist, srcHist, 0, 1, NORM_MINMAX);
	
	tracing(srcHist);
	waitKey(0);
	return 0;
}

void tracing(const MatND srcHist) {
	int width = abs(processPoint.x - originalPoint.x);
	int height = abs(processPoint.y - originalPoint.y);
	cout << width << height << endl;
	//Ŀ�����������趨Ϊԭ�������Χ�����Ϊԭ������
	int X1 = originalPoint.x - width;
	int X2 = originalPoint.x + width;
	int Y1 = originalPoint.y - height;
	int Y2 = originalPoint.y + height;
	//Խ����
	if (X1 < 0)
		X1 = 0;
	if (Y1 < 0)
		Y1 = 0;

	Point preStart;
	Point preEnd;

	Point get1(0, 0);
	Point get2(0, 0);
	
	VideoCapture video(videoPath);
	if (!video.isOpened())
	{
		cout << "video not open.error!" << std::endl;
		return;
	}
	double fps = video.get(CV_CAP_PROP_FPS); //��ȡ��Ƶ֡��
	double pauseTime = 1000 / fps; //���������м���
	int w = static_cast<int>(video.get(CV_CAP_PROP_FRAME_WIDTH));
	int h = static_cast<int>(video.get(CV_CAP_PROP_FRAME_HEIGHT));
	cv::Size S(w, h);
	namedWindow("targetTracing");
	
	VideoWriter write;
	write.open("c501_test.avi", -1, fps, S, true);
	while (true)
	{
		//ȡ��Ƶ��һ֡
		video >> image;
		if (!image.data || waitKey(pauseTime) == 27)  //ͼ��Ϊ�ջ�Esc�������˳�����
		{
			break;
		}
		//ֱ��ͼ�Աȵ�����ֵ
		double comnum = 1.0;
		//��ʼ����������
		for (int Cy = Y1; Cy <= Y2; Cy += 10) {
			for (preStart.x = X1, preStart.y = Cy; preStart.x <= X2; preStart.x += 10) {

				if ((preStart.x + width) < image.cols)
					preEnd.x = preStart.x + width;
				else
					preEnd.x = image.cols - 1;
				if ((preStart.y + height) < image.rows)
					preEnd.y = preStart.y + height;
				else
					preEnd.y = image.rows - 1;
				//cout << preStart << preEnd << endl;

				Mat compareImg;
				compareImg = image(Rect(preStart, preEnd));
				double c = compHist(srcHist, compareImg);
				if (comnum > c) {
					get1 = preStart;
					get2 = preEnd;
					comnum = c;
				}
			}
		}
		//��ԭʼ��Ƶͼ����ˢ�¾��Σ�ֻ�е���Ŀ��ֱ��ͼ������ʱ�Ÿ������������������Ŀ������ƶ��ĳ���
		if (comnum < 0.15) {
			X1 = get1.x - width;
			X2 = get1.x + width;
			Y1 = get1.y - height;
			Y2 = get1.y + height;

			if (X1 < 0)
				X1 = 0;
			if (Y1 < 0)
				Y1 = 0;
			
		}
		if(comnum<0.5)
			rectangle(image, get1, get2, Scalar(0, 0, 255), 2);
		//д��һ֡
		write.write(image);
		imshow("targetTracing", image);
	}
	video.release();
	write.release();
}

//����ֱ��ͼ
void drawHist(const Mat rectImage)
{
	//���庯����Ҫ��һЩ����
	//ͼƬ����nimages
	int nimages = 1;
	//ͨ������,��������ϰ������������ʾ������ὲԭ��
	int channels[3] = { 0,1,2 };
	//���ֱ��ͼ
	cv::Mat outputHist_red, outputHist_green, outputHist_blue;
	//ά��
	int dims = 1;
	//���ÿ��ά��ֱ��ͼ�ߴ磨bin������������histSize
	int histSize[3] = { 256,256,256 };
	//ÿһά��ֵ��ȡֵ��Χranges
	float hranges[2] = { 0, 255 };
	//ֵ��Χ��ָ��
	const float *ranges[3] = { hranges,hranges,hranges };
	//�Ƿ����
	bool uni = true;
	//�Ƿ��ۻ�
	bool accum = false;

	//����ͼ���ֱ��ͼ(��ɫͨ������)
	cv::calcHist(&rectImage, nimages, &channels[0], cv::Mat(), outputHist_red, dims, &histSize[0], &ranges[0], uni, accum);
	//����ͼ���ֱ��ͼ(��ɫͨ������)
	cv::calcHist(&rectImage, nimages, &channels[1], cv::Mat(), outputHist_green, dims, &histSize[1], &ranges[1], uni, accum);
	//����ͼ���ֱ��ͼ(��ɫͨ������)
	cv::calcHist(&rectImage, nimages, &channels[2], cv::Mat(), outputHist_blue, dims, &histSize[2], &ranges[2], uni, accum);

	//����ÿ������(bin)���飬��������ڿ���̨����ġ�
	//for (int i = 0; i < 256; i++)
		//std::cout << "bin/value:" << i << "=" << outputHist_red.at<float>(i) << std::endl;

	//����ֱ��ͼ
	int scale = 1;
	//ֱ��ͼ��ͼƬ,��Ϊ�ߴ���һ�����,���Ծ���histSize[0]����ʾȫ����.
	cv::Mat histPic(histSize[0], histSize[0] * scale * 3, CV_8UC3, cv::Scalar(0, 0, 0));
	//�ҵ����ֵ����Сֵ,������0��2�ֱ��Ǻ�,��,��
	double maxValue[3] = { 0, 0, 0 };
	double minValue[3] = { 0, 0, 0 };
	cv::minMaxLoc(outputHist_red, &minValue[0], &maxValue[0], NULL, NULL);
	cv::minMaxLoc(outputHist_green, &minValue[1], &maxValue[1], NULL, NULL);
	cv::minMaxLoc(outputHist_blue, &minValue[2], &maxValue[2], NULL, NULL);
	//����
	std::cout << minValue[0] << " " << minValue[1] << " " << minValue[2] << std::endl;
	std::cout << maxValue[0] << " " << maxValue[1] << " " << maxValue[2] << std::endl;

	//���������ű���
	double rate_red = (histSize[0] / maxValue[0])*0.9;
	double rate_green = (histSize[0] / maxValue[1])*0.9;
	double rate_blue = (histSize[0] / maxValue[2])*0.9;

	for (int i = 0; i < histSize[0]; i++)
	{
		float value_red = outputHist_red.at<float>(i);
		float value_green = outputHist_green.at<float>(i);
		float value_blue = outputHist_blue.at<float>(i);
		//�ֱ𻭳�ֱ��
		cv::line(histPic, cv::Point(i*scale, histSize[0]), cv::Point(i*scale, histSize[0] - value_red * rate_red), cv::Scalar(0, 0, 255));
		cv::line(histPic, cv::Point((i + 256)*scale, histSize[0]), cv::Point((i + 256)*scale, histSize[0] - value_green * rate_green), cv::Scalar(0, 255, 0));
		cv::line(histPic, cv::Point((i + 512)*scale, histSize[0]), cv::Point((i + 512)*scale, histSize[0] - value_blue * rate_blue), cv::Scalar(255, 0, 0));
	}
	cv::imshow("histgram", histPic);
	
}

//��������������Ƶ�����������Ŀ��ͼ��
void getTarget() {
	VideoCapture video(videoPath);
	if (!video.isOpened())
	{
		cout << "video not open.error!" << std::endl;
		return;
	}
	double fps = video.get(CV_CAP_PROP_FPS); //��ȡ��Ƶ֡��
	double pauseTime = 1000 / fps; //���������м���
	namedWindow("Friends");
	setMouseCallback("Friends", onMouse);
	while (true)
	{
		if (!leftButtonDownFlag) //�ж�������û�а��£���ȡ������Ƶ��������ͣ
		{
			video >> image;
		}
		if (!image.data || waitKey(pauseTime) == 27)  //ͼ��Ϊ�ջ�Esc�������˳�����
		{
			break;
		}
		//��������²���ԭʼ��Ƶͼ����ˢ�¾���
		//1. �������յ�
		//2. ���������δ̧��
		if (originalPoint != processPoint && !leftButtonDownFlag)
		{
			rectangle(image, originalPoint, processPoint, Scalar(255, 0, 0), 2);
		}
		imshow("Friends", image);
		if (cccc == 1) {
			destroyWindow("Friends");
			break;
		}
			
	}
	video.release();
}

//���ص�����  
void onMouse(int event, int x, int y, int flags, void *ustc)
{
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		leftButtonDownFlag = true; //��־λ
		originalPoint = Point(x, y);  //����������µ�ľ������
		processPoint = originalPoint;
	}
	if (event == CV_EVENT_MOUSEMOVE && leftButtonDownFlag)
	{
		imageCopy = image.clone();
		processPoint = Point(x, y);

		if (originalPoint != processPoint)
		{
			//�ڸ��Ƶ�ͼ���ϻ��ƾ���
			rectangle(imageCopy, originalPoint, processPoint, Scalar(255, 0, 0), 2);
		}
		imshow("Friends", imageCopy);
		
	}
	if (event == CV_EVENT_LBUTTONUP)
	{
		leftButtonDownFlag = false;
		targetImage = image(Rect(originalPoint, processPoint)); //��ȡĿ��ͼ��targetImage
		cccc = 1;
	}
}

double compHist(const MatND srcHist,Mat compareImage)
{
	//�ڱȽ�ֱ��ͼʱ����Ѳ�������HSV�ռ��в�����������Ҫ��BGR�ռ�ת��ΪHSV�ռ�
	Mat compareHsvImage;
	cvtColor(compareImage, compareHsvImage, CV_BGR2HSV);
	//����H-Sֱ��ͼ���д���
	//���ȵ�����ֱ��ͼ�Ĳ���
	MatND  compHist;
	//����ԭͼֱ��ͼ�ļ���
	
	//����Ҫ�Ƚϵ�ͼ����ֱ��ͼ�ļ���
	calcHist(&compareHsvImage, 1, channels, Mat(), compHist, 2, histSize, ranges, true, false);
	//ע�⣺������Ҫ������ֱ��ͼ���й�һ������
	
	normalize(compHist, compHist, 0, 1, NORM_MINMAX);
	//�Եõ���ֱ��ͼ�Ա�
	double g_dCompareRecult = compareHist(srcHist, compHist, 3);//3��ʾ���ð��Ͼ����������ֱ��ͼ�ıȽ�
	return g_dCompareRecult;
}
