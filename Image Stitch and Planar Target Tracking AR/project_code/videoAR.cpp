#include <opencv2/opencv.hpp>
#include <iostream>
#include<opencv2/xfeatures2d.hpp>
#include<opencv2/features2d/features2d.hpp>
#include<cmath>

using namespace cv;
using namespace std;

const char* videoPath = "video_pro.mp4";
const char* videoPath02 = "video03.mp4";
Mat image999;
Mat imageCopy999; //���ƾ��ο�ʱ��������ԭͼ��ͼ��
bool leftButtonDownFlag = false; //�����������Ƶ��ͣ���ŵı�־λ
int cccc = 0;//�ж�Ŀ�������
//��Ҫ�õ�һ���������򣬸��ݸ�������������������homograph�任
Point originalPoint; //���ο����
Point processPoint; //���ο��յ�

struct userdata {
	Mat im;
	vector<Point2f> points;
};

void mouseHandler(int event, int x, int y, int flags, void* data_ptr)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		userdata *data = ((userdata *)data_ptr);
		circle(data->im, Point(x, y), 3, Scalar(0, 255, 255), 5, CV_AA);
		imshow("Image", data->im);
		if (data->points.size() < 4)
		{
			data->points.push_back(Point2f(x, y));
		}
	}

}

Mat getmyHomographMat(Mat &lastPart, Mat &curPart) {
	//�Ҷ�ͼת��  
	Mat image1, image2;
	cvtColor(lastPart, image1, CV_RGB2GRAY);
	cvtColor(curPart, image2, CV_RGB2GRAY);

	//��ȡ������
	Ptr<xfeatures2d::SURF> detector = xfeatures2d::SURF::create(2000);

	vector<KeyPoint> keyPoint1, keyPoint2;
	Mat imageDesc1, imageDesc2;
	detector->detectAndCompute(image1, Mat(), keyPoint1, imageDesc1);
	detector->detectAndCompute(image2, Mat(), keyPoint2, imageDesc2);

	FlannBasedMatcher matcher;
	vector<vector<DMatch> > matchePoints;
	vector<DMatch> GoodMatchePoints;

	set< pair<int, int>>  matches;

	vector<Mat> train_desc(1, imageDesc1);
	matcher.add(train_desc);
	matcher.train();

	matcher.knnMatch(imageDesc2, matchePoints, 2);
	//cout << "ȫ��ƥ�����Ŀ��" << matchePoints.size() << endl;

	// Lowe's algorithm,��ȡ����ƥ���
	for (int i = 0; i < matchePoints.size(); i++)
	{
		if (matchePoints[i][0].distance < 0.5 * matchePoints[i][1].distance)
		{
			GoodMatchePoints.push_back(matchePoints[i][0]);
			matches.insert(make_pair(matchePoints[i][0].queryIdx, matchePoints[i][0].trainIdx));
		}
	}
	//cout << "1->2 good ƥ�����Ŀ��" << GoodMatchePoints.size() << endl;

	//��ѡ�Ƿ���ж���������ƥ��ɸѡ
#if 1

	FlannBasedMatcher matcher2;
	matchePoints.clear();
	vector<Mat> train_desc2(1, imageDesc2);
	matcher2.add(train_desc2);
	matcher2.train();

	matcher2.knnMatch(imageDesc1, matchePoints, 2);
	// Lowe's algorithm,��ȡ����ƥ���
	for (int i = 0; i < matchePoints.size(); i++)
	{
		if (matchePoints[i][0].distance < 0.5 * matchePoints[i][1].distance)
		{
			if (matches.find(make_pair(matchePoints[i][0].trainIdx, matchePoints[i][0].queryIdx)) == matches.end())
			{
				GoodMatchePoints.push_back(DMatch(matchePoints[i][0].trainIdx, matchePoints[i][0].queryIdx, matchePoints[i][0].distance));
			}

		}
	}
	cout << "1->2 & 2->1 good ƥ�����Ŀ��" << GoodMatchePoints.size() << endl;
#endif

	vector<Point2f> imagePoints1, imagePoints2;

	for (int i = 0; i < GoodMatchePoints.size(); i++)
	{
		imagePoints2.push_back(keyPoint2[GoodMatchePoints[i].queryIdx].pt);
		imagePoints1.push_back(keyPoint1[GoodMatchePoints[i].trainIdx].pt);
	}
	Mat homo = findHomography(imagePoints1, imagePoints2,CV_RANSAC);
	return homo;
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
		imageCopy999 = image999.clone();
		processPoint = Point(x, y);

		if (originalPoint != processPoint)
		{
			//�ڸ��Ƶ�ͼ���ϻ��ƾ���
			rectangle(imageCopy999, originalPoint, processPoint, Scalar(0, 0, 255), 2);
		}
		
		imshow("Friends", imageCopy999);

	}
	if (event == CV_EVENT_LBUTTONUP)
	{
		leftButtonDownFlag = false;
		Mat targetImage999 = image999(Rect(originalPoint, processPoint)); //��ͼ����ʾj
		imshow("zsds", targetImage999);
		waitKey(0);
		cccc = 1;
	}
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
			video >> image999;
		}
		if (!image999.data || waitKey(pauseTime) == 27)  //ͼ��Ϊ�ջ�Esc�������˳�����
		{
			break;
		}
		//��������²���ԭʼ��Ƶͼ����ˢ�¾���
		//1. �������յ�
		//2. ���������δ̧��
		if (originalPoint != processPoint && !leftButtonDownFlag)
		{
			rectangle(image999, originalPoint, processPoint, Scalar(255, 0, 0), 2);
		}
		//rectangle(image999, originalPoint, processPoint, Scalar(255, 0, 0), 2);
		imshow("Friends", image999);
		if (cccc == 1) {
			destroyWindow("Friends");
			break;
		}
	}
	video.release();
}

int main() {

	getTarget();
	//waitKey(0);
	cout << "���ο���㣺" << originalPoint << endl;
	cout << "���ο��յ㣺" << processPoint << endl;
	destroyAllWindows();

	cout << "start!!!" << endl;
	VideoCapture video(videoPath);
	VideoCapture myvideo(videoPath02);

	if ((!video.isOpened())||(!myvideo.isOpened()))
	{
		cout << "video open error!" << endl;
		return -1;
	}
	double fps = video.get(CV_CAP_PROP_FPS); //��ȡ��Ƶ֡��
	cout << "video fps:" << fps << endl;
	double pauseTime = 1000 / fps; //���������м���
	//long num_frame = static_cast<long>(video.get(CV_CAP_PROP_FRAME_COUNT));
	//int update_num = num_frame / 20;
	int WY = static_cast<int>(video.get(CV_CAP_PROP_FRAME_WIDTH));
	int HY = static_cast<int>(video.get(CV_CAP_PROP_FRAME_HEIGHT));
	cv::Size S(WY, HY);

	Mat FstImg, shImg, vimg;
	video >> FstImg;
	vimg = FstImg.clone();
	// Read in the image.
	Mat inImg;
	myvideo >> inImg;
	Size size = inImg.size();
	
	// Create a vector of points.
	vector<Point2f> pts_src;
	pts_src.push_back(Point2f(0, 0));
	pts_src.push_back(Point2f(size.width - 1, 0));
	pts_src.push_back(Point2f(size.width - 1, size.height - 1));
	pts_src.push_back(Point2f(0, size.height - 1));

	// Set data for mouse handler
	Mat im_temp = FstImg.clone();
	userdata data;
	data.im = im_temp;

	//show the image
	imshow("Image", im_temp);

	cout << "Click on four corners of a billboard and then press ENTER" << endl;
	//set the callback function for any mouse event
	setMouseCallback("Image", mouseHandler, &data);
	waitKey(0);

	cout << "size data point :"<<data.points.size() << endl;

	// Calculate Homography between source and destination points
	Mat h = findHomography(pts_src, data.points);

	// Warp source image
	warpPerspective(inImg, im_temp, h, im_temp.size());

	// Extract four points from mouse data
	Point pts_dst[4];
	for (int i = 0; i < 4; i++)
	{
		pts_dst[i] = data.points[i];
	}
	// Black out polygonal area in destination image.
	fillConvexPoly(vimg, pts_dst, 4, Scalar(0), CV_AA);

	// Add warped source image to destination image.
	shImg = vimg + im_temp;
	Mat curImg;

	vector<Point2f> CURponits;
	//Mat lastPart = FstImg(Rect(originalPoint, processPoint));//�����ٶ�

	VideoWriter writerr;
	writerr.open("video_out_final.mp4", CV_FOURCC('D', 'I', 'V', 'X'), fps, S, true);
	//imshow("result", shImg);
	writerr.write(shImg);
	int count_frame = 1;
	//Mat lastPart = FstImg(Rect(originalPoint, processPoint));//�����ٶ�
	while (true) {
		video >> curImg; myvideo >> inImg;		
		if (!inImg.data||!curImg.data || waitKey(pauseTime) == 27)  //ͼ��Ϊ�ջ�Esc�������˳�����
		{
			break;
		}
		im_temp = curImg.clone();
		vimg = curImg.clone();		
		//Mat curPart = curImg(Rect(originalPoint, processPoint));
		//h = getmyHomographMat(lastPart, curPart);
		h = getmyHomographMat(FstImg, curImg);
		//cout << "�任����Ϊ��\n" << h << endl << endl; //���ӳ�����		
		perspectiveTransform(data.points,CURponits, h);
		//cout << "size data point :" << data.points.size() << endl;
		Mat HH = findHomography(pts_src, CURponits);
		warpPerspective(inImg, inImg, HH, curImg.size());
		for (int i = 0; i < 4; i++)
		{
			pts_dst[i] = CURponits[i];
		}		
		fillConvexPoly(vimg, pts_dst, 4, Scalar(0), CV_AA);
		shImg = vimg + inImg;
		//��ȥ���¶Ա�ͼƬ�����ÿ������һ�εĵ�����ϱ任���ᵼ�������ۻ�
		//imshow("result", shImg);
		count_frame++;
		if (count_frame % 10 == 0) {
			FstImg = im_temp;
			//lastPart = im_temp(Rect(originalPoint, processPoint));//�����ٶ�
			data.points = CURponits;
		}
		writerr.write(shImg);
		//shImg.release();		
	}
	//waitKey(0);
	writerr.release();
	video.release();
	myvideo.release();
	return 0;
}



int main()
{
	//VideoCapture capture;
	//capture.open("video_pro.mp4");//VideoCapture��ķ���
	//long num_frame = static_cast<long>(capture.get(CV_CAP_PROP_FRAME_COUNT));
	//cout << num_frame << endl;
	//capture.release();
	// Read in the image.
	Mat im_src = imread("target.jpg");
	Size size = im_src.size();

	// Create a vector of points.
	vector<Point2f> pts_src;
	pts_src.push_back(Point2f(0, 0));
	pts_src.push_back(Point2f(size.width - 1, 0));
	pts_src.push_back(Point2f(size.width - 1, size.height - 1));
	pts_src.push_back(Point2f(0, size.height - 1));

	// Destination image
	Mat im_dst = imread("raw.png");

	// Set data for mouse handler
	Mat im_temp = im_dst.clone();
	userdata data;
	data.im = im_temp;

	//show the image
	imshow("Image", im_temp);

	cout << "Click on four corners of a billboard and then press ENTER" << endl;
	//set the callback function for any mouse event
	setMouseCallback("Image", mouseHandler, &data);
	waitKey(0);

	// Calculate Homography between source and destination points
	Mat h = findHomography(pts_src, data.points);

	// Warp source image
	warpPerspective(im_src, im_temp, h, im_temp.size());

	// Extract four points from mouse data
	Point pts_dst[4];
	for (int i = 0; i < 4; i++)
	{
		pts_dst[i] = data.points[i];
	}
	// Black out polygonal area in destination image.
	fillConvexPoly(im_dst, pts_dst, 4, Scalar(0), CV_AA);

	// Add warped source image to destination image.
	im_dst = im_dst + im_temp;

	// Display image.
	imshow("Image", im_dst);
	waitKey(0);
	return 0;
}

int flipVideo() {
	cout << "start!!!" << endl;
	VideoCapture video(videoPath);
	if (!video.isOpened())
	{
		cout << "video open error!" << endl;
		return -1;
	}
	double fps = video.get(CV_CAP_PROP_FPS); //��ȡ��Ƶ֡��
	cout << "video fps:" << fps << endl;

	double pauseTime = 1000 / fps; //���������м���
	int w = static_cast<int>(video.get(CV_CAP_PROP_FRAME_WIDTH));
	int h = static_cast<int>(video.get(CV_CAP_PROP_FRAME_HEIGHT));
	cv::Size S(w, h);

	VideoWriter write;
	write.open("video_pro.mp4", CV_FOURCC('D', 'I', 'V', 'X'), fps, S, true);
	Mat image;
	while (true)
	{
		//ȡ��Ƶ��һ֡
		video >> image;
		if (!image.data || waitKey(pauseTime) == 27)  //ͼ��Ϊ�ջ�Esc�������˳�����
		{
			break;
		}
		flip(image, image, -1);
		write.write(image);
	}
	video.release();
	write.release();
	/*
	Mat firstimg;
	video.read(firstimg);
	flip(firstimg, firstimg, -1);
	imshow("first", firstimg);
	waitKey(0);*/
	return 0;
}
