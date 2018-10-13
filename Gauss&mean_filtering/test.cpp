#include<iostream>

using namespace std;

//�õ�ͼ��Ļ���ͼ
void  integral_image(const  uchar *src, int width, int height, int sstride, int *pint, int istride) {
	int *prow = new int[width];
	memset(prow, 0, sizeof(int)*width);
	for (int yi = 0; yi < height; ++yi, src += sstride, pint += istride) {
		prow[0] += src[0];  
		pint[0] = prow[0];//for the first pixel
		for (int xi = 1; xi < width; ++xi) 
		{ 
			prow[xi] += src[xi]; 
			pint[xi] = pint[xi - 1] + prow[xi]; 
		}
	}
	delete[]prow;
}


void GetGrayIntegralImage(unsigned char *Src, int *Integral, int Width, int Height, int Stride)
{
	memset(Integral, 0, (Width + 1) * sizeof(int));                    //    ��һ�ж�Ϊ0
	for (int Y = 0; Y < Height; Y++)
	{
		unsigned char *LinePS = Src + Y * Stride;
		int *LinePL = Integral + Y * (Width + 1) + 1;              //��һ��λ��
		int *LinePD = Integral + (Y + 1) * (Width + 1) + 1;            //    ��ǰλ�ã�ע��ÿ�еĵ�һ�е�ֵ��Ϊ0
		LinePD[-1] = 0;                                                //    ��һ�е�ֵΪ0
		for (int X = 0, Sum = 0; X < Width; X++)
		{
			Sum += LinePS[X];                                        //    �з����ۼ�
			LinePD[X] = LinePL[X] + Sum;                            //    ���»���ͼ
		}
	}
}

int main() {

	int *p;
	p = new int[25];
	for (int i = 0; i <5; i++,p+=5) {
		for (int j = 0; j < 5; j++) {
			p[j] = i*5;
			cout << p[j] << endl;
		}
		
	}
	system("pause");
	return 0;
}