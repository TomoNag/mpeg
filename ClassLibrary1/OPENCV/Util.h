#pragma once
#include "OPENCV.h"


#define MAXCOUNT 256
#define IMSHOW(name, mat) if(SHOWIMG){;\
						  imshow(name, mat);\
						  waitKey(30);\
						  }
#define SHOWIMG true

void connectedcomponent(Mat src, int l1, int l2);
cv::flann::GenericIndex< cvflann::L2<int> > trimaptree(Mat orig, Mat msk);
cv::flann::GenericIndex< cvflann::L2<int> > gethaircolor(Mat src, Mat face, Mat bg, Mat hair, Mat solidface);
Mat HueMat(Mat src);
void closeholes(Mat *src);
Mat BAYSIAN(Mat img, Mat tri, Mat solidface);

bool IsUniform(Mat src, float thresh = 0x10);
bool IsNoisy(Mat gray);
bool IsLegal(Mat src, Point p);
bool IsLegal(Mat src, Rect r);
Rect patchRect(Size patchsize, Point patchpos);
Mat patchMat(Mat img, Size patchsize, Point patchpos);

cv::Scalar numToColor(unsigned short num);
Vec3b numToColor2(unsigned short num);
Mat labelToImg(Mat label0);

void rotate(Mat src, Mat* dst, float angle, float scale);
void translate(Mat src, Mat* dst, Point2d p);

Mat matCircle(float radOut, float radIn, bool normalize = false, int size = 0, float backcolor = 0, float forecolor = 1);
Mat matCircleU(float radOut, float radIn, bool normalize = false);
Mat matCircleAA(float radOut, float radIn, bool normalize = false, int size = 0, float backcolor = 0, float forecolor = 1);
Mat matHalfCircleAA(float radOut, float theta360, float widthratio = 1.0, bool normalize = false, int size = 0, float backcolor = 0, float forecolor = 1);
Mat matRectangleAA(float radOut, float theta360, float widthratio = 1.0, bool normalize = false, int size = 0, float backcolor = 0, float forecolor = 1);

float getSum(Mat &orig);
float getAvg(Mat &orig);
float getMax(Mat &orig);
float getMin(Mat &orig);

void convolution(Mat src, Mat *dst, Mat element, Mat *msk = NULL);

void addcolor(cv::Mat &imageIn, cv::Mat &imageAdd, int thresh, int colornum = 0);
void addcolor2(cv::Mat &imageIn, cv::Mat &imageAdd, Mat &radius, int thresh, int colornum = 0);
void matToDataF(Mat &imageAdd, Mat &radius, float thresh, float mult, int *size, float **X, float **Y, float **Data, float **R);
void addcolor3(cv::Mat &imageIn, float mult, int size, float *X, float *Y, float *R);
void addcolor4(cv::Mat &imageIn, int size, int *X, int *Y, float *R);

Mat resize(Mat src, int maxsize);
Mat connect(Mat mats[], int numH, int numV);

void compare(Mat inMatA, Mat inMatB); // A = Max(A, B);
void compare(Mat inMatA, Mat inMatB, Mat inMatC, float num); // A = Max(A, B), if(B > A) C = num;
void compareMin(Mat inMatA, Mat inMatB);
void compareMin(Mat inMatA, Mat inMatB, Mat inMatC, float num);

string intToString(int number);
int dist( Point p , Point q);
int dist( int x, int y, int z = 0);
int dist( Vec3b p ,  Vec3b q);

int L1dist( Point p , Point q);
int L1dist( int x, int y, int z = 0);
int L1dist( Vec3b p ,  Vec3b q);

double matdistL1F(Mat src, Mat src2);
double matdistF(Mat src, Mat src2);


Mat correlationLP(Mat src0, Point pos);
Vec3f getL(Mat img, int x, int y);
float getLU(Mat img, int x, int y);

void solvePoisson(Mat L, Mat *init, Mat msk);

template <class T>
void diffx2(Mat src, Mat *dst, int step);
template <class T>
void diffy2(Mat src, Mat *dst, int step);



void fixcircle(Mat &src, Mat &sep, Mat &rad, float thresh, int dist, float MINRAD, float MAXRAD, float mult);
void fixlocalpeak(Mat src, Mat rad, float thresh);

void analyze(vector<Point> points, Mat *out= NULL, double threshC = 0.8, double threshR = 4.0, Mat *matA= NULL, Mat *matL= NULL, Mat *matC= NULL, Mat *matR= NULL);
void regionsToData(vector<vector<Point>> regions, float threshC, float threshR, int *num, float **X, float **Y, float **A, float **L, float **C, float **R);

bool finddivide(vector<Point> points, vector<vector<Point>> *approxs, Mat *out);
vector<vector<Point>> analyzer(Mat src, int threshColor, int threshCanny, bool divide, Mat *out);

Mat correlation(Mat src0, Point pos);

Point randompoint(int width, int height);
Mat hsv(Mat src, int i);


namespace Util
{
	

	int hist(Mat *matA,  int histSize = 25, float max = 0x100 , float min = 0, bool ignorezero = false, Mat *histOut = NULL, float* threshout = NULL, float* aveout = NULL, float *devout = NULL, float *absaveout = NULL, int *countout = NULL, float percentage = 0);
	int findBG(Mat src, Mat *dst, int *avgcolor, int blurrad = 0);

};
