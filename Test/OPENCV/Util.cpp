#include "Util.h"
#include "Filters.h"

//void main()
//{
//	Mat src2 = imread("VAL/PIC (35).jpg", 1);
//	src2 = resize(src2, 512);
//
//	return;
//	//edgedetection(src2);
//}

bool IsUniform(Mat src, float thresh)
{
	Mat m2, m3;
	cv::reduce(src, m2, 1, CV_REDUCE_AVG); 
	cv::reduce(m2, m3, 0, CV_REDUCE_AVG);

	Vec3b ave = m3.at<Vec3b>(0);

	double total = 0;
	int count = 0;
	for(int y = 0; y < src.rows; y++){
		Vec3b* p = src.ptr<Vec3b>(y);
		for(int x = 0; x < src.cols; x++){
			
			total+= dist(ave, p[x]);
			count++;
		}
	}

	if(total/count < thresh * thresh)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool IsNoisy(Mat gray)
{
	Mat laplacian_img;
	Mat tmp_img;
	Laplacian(gray, tmp_img, CV_32F);
	convertScaleAbs(tmp_img, laplacian_img, 1, 0);
	Mat edge = laplacian_img.clone();
	ThreshFuncs<unsigned char>(THRESH, edge, 0xFF, 0, edge, 0x20);
	Mat element = matCircleU(3, 0);
	cv::dilate(edge,edge, element, cv::Point(-1,-1), 1);
	float noise = getAvg(edge);

	if(noise > 0xFF * 0.05)
	{
		return true;
	}
	return false;
}


bool IsLegal(Mat src, Point p)
{
	if(p.x >= 0 && p.y >= 0 && p.x < src.cols && p.y < src.rows)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool IsLegal(Mat src, Rect r)
{
	if(r.x >= 0 && r.y >= 0 && r.x + r.width < src.cols && r.y + r.height < src.rows)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void closeholes(Mat *src)
{
	Mat canny_output;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	int thresh = 0x40;

	cv::Mat element = matCircle(2, 0);
	element.convertTo(element, CV_8U);

	Canny( *src, canny_output, thresh, thresh*2, 3, false);

	cv::dilate(canny_output,canny_output, element, cv::Point(-1,-1), 1);
	cv::erode(canny_output,canny_output, element, cv::Point(-1,-1), 1);

	findContours( canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	//Mat drawing;
	//cvtColor(src, drawing, CV_GRAY2BGR);


	for( int i = 0; i< contours.size(); i++ )
	{
		const Point* p = &contours[i][0];
		int n = (int)contours[i].size();
		fillPoly(*src, &p, &n, 1, 0xFF, 4);
		//polylines(drawing, &p, &n, 1, true, Scalar(0xC0, 0xFF, 0), 1, 0);
	}
}

void connectedcomponent(Mat src, int l1, int l2)
{
	typedef unsigned char T;

	if(l1 == l2)
	{
		return;
	}

	int w = src.cols;
	int h = src.rows;

	while(true)
	{
		int count = 0;
		for(int y = 0; y <h - 1; y++){
			for(int x = 0; x < w - 1; x++){
				int a = src.at<T>(y, x);
				int b = src.at<T>(y, x +1);
				int c = src.at<T>(y + 1, x);
				if(a == l1 && b == l2)
				{
					src.at<T>(y, x +1) = src.at<T>(y, x);
					count++;
				}

				if(a == l1 && c == l2)
				{
					src.at<T>(y + 1,  x) = src.at<T>(y, x);
					count++;
				}

			}
		}

		for(int y = h - 1; y > 0; y--){
			for(int x = w - 1; x > 0; x--){
				int a = src.at<T>(y, x);
				int b = src.at<T>(y, x - 1);
				int c = src.at<T>(y - 1, x);
				if(a == l1 && b == l2)
				{
					src.at<T>(y, x - 1) = src.at<T>(y, x);
					count++;
				}

				if(a == l1 && c == l2)
				{
					src.at<T>(y - 1,  x) = src.at<T>(y, x);
					count++;
				}

			}
		}

		if(count == 0)
		{
			break;
		}
	}
}

string intToString(int number)
{
  stringstream ss;
  ss << number;
  return ss.str();
}

int dist(int x , int y, int z)
{
	return x * x + y * y + z * z;
}



int dist( Point p , Point q)
{
	int dx = p.x - q.x;
	int dy = p.y - q.y;

	return dist(dx, dy);
}


int dist( Vec3b p ,  Vec3b q)
{
	int dx = p[0] - q[0];
	int dy = p[1] - q[1];
	int dz = p[2] - q[2];
	return dist(dx, dy, dz);
}

int L1dist(int x , int y, int z)
{
	return abs(x) + abs(y) + abs(z);
}


int L1dist( Point p , Point q)
{
	int dx = p.x - q.x;
	int dy = p.y - q.y;

	return L1dist(dx, dy);
}


int L1dist( Vec3b p ,  Vec3b q)
{
	int dx = p[0] - q[0];
	int dy = p[1] - q[1];
	int dz = p[2] - q[2];
	return L1dist(dx, dy, dz);
}



template <class T>
void diffx2(Mat src, Mat *dst, int step){


	*dst = Mat::zeros(src.size(), src.type());

	int minx = 0;
	int maxx = src.cols;
	if(step > 0)
	{
		maxx -= step;
	}
	else
	{
		minx -= step;
	}

	for(int y=0; y<src.rows; y++)
	{
		T *p = src.ptr<T>(y) + minx;
		T *q = (*dst).ptr<T>(y) + minx;
		for(int x=minx; x<maxx; x++)
		{			
			*q = *(p + step) - *p;
			p++;
			q++;
		}
		
	}
	return;
}

template <class T>
void diffy2(Mat src, Mat *dst, int step){


	*dst = Mat::zeros(src.size(), src.type());

	int miny = 0;
	int maxy = src.rows;
	if(step > 0)
	{
		maxy -= step;
	}
	else
	{
		miny -= step;
	}

	int w = src.cols;
	int dist = w * step;
	for(int y=miny; y<maxy; y++)
	{
		T *p = src.ptr<T>(y);
		T *q = (*dst).ptr<T>(y);
		for(int x=0; x<src.cols; x++)
		{			
			*q = *(p + dist) - *p;
			p++;
			q++;
		}
		
	}
	return;
}

int diffs(Mat src, Mat *dst, int step)
{
	diffx2<float>(src, dst, step);
	diffx2<Vec3f>(src, dst, step);

	diffy2<float>(src, dst, step);
	diffy2<Vec3f>(src, dst, step);

	return 0;
}

Vec3f getL(Mat img, int x, int y)
{
	if(x < 1 || y < 1 || x >= img.cols - 1 || y >= img.rows - 1)
	{
		return Vec3f(0, 0, 0);
	}

	Vec3f color = img.at<Vec3b>(y, x);

	color *= -4;
	color += img.at<Vec3b>(y, x+1);
	color += img.at<Vec3b>(y, x-1);
	color += img.at<Vec3b>(y+1, x);
	color += img.at<Vec3b>(y-1, x);

	return color;
}

float getLU(Mat img, int x, int y)
{
	if(x < 1 || y < 1 || x >= img.cols - 1 || y >= img.rows - 1)
	{
		return 0;
	}

	float color = img.at<unsigned char>(y, x);

	color *= -4;
	color += img.at<unsigned char>(y, x+1);
	color += img.at<unsigned char>(y, x-1);
	color += img.at<unsigned char>(y+1, x);
	color += img.at<unsigned char>(y-1, x);

	return color;
}


double abs(Vec3f color)
{
	return abs(color[0]) + abs(color[1]) + abs(color[2]);
}


void solvePoisson(Mat L, Mat *init, Mat msk)
{
	typedef float T;

	Mat mskU;
	msk.convertTo(mskU, CV_8U);

	int h = L.rows;
	int w = L.cols;

	int MAX_ITERATION = 300;

	for (int loop=0; loop<MAX_ITERATION; loop++) {
		int changed = 0;
		for (int y=1; y<h-1; y++) {
			for (int x=1; x<w-1; x++) {			
				uchar c = mskU.at<unsigned char>(y, x);
				if (c >0x80) {
					int destIndex = w * y + x;
					//printf("check (%d,%d)\n", x, y);
					T value = 0;
					// right
					int count = 0;
					if (x < w - 1) {
						count++;
						value += (*init).at<T>(y, x+1);

					}
					// left
					if (x > 0) {
						count++;
						value += (*init).at<T>(y, x-1);
					}
					// top
					if (y > 0) {
						count++;
						value += (*init).at<T>(y-1, x);
					}
					// bottom
					if (y < h - 1) {
						count++;
						value += (*init).at<T>(y+1, x);
					}

					T newval = (value - L.at<T>(y, x)) * (1.0 / count);
					//Vec3f newval = value/count - L.at<T>(y, x);
					T oldval = (*init).at<T>(y, x);				
					(*init).at<T>(y, x) = oldval + 1.9 * (newval - oldval);
					if(abs(oldval - newval) > 0.01)
					{
						changed++;
					}
				}
			}
		}
		if(changed == 0)
		{
			break;
		}
	}

}




unsigned char getBit(unsigned short a, unsigned char pos)
{
	return 0x0001 & (a >> pos);
}

Vec3b numToColor2(unsigned short num)
{
	unsigned short a = num/3;
	unsigned short b = num%3;

	unsigned char x = 0;
	x = x| (getBit(a,0) << 7);
	x = x| (getBit(a,2) << 6);
	x = x| (getBit(a,4) << 5);
	x = x| (getBit(a,6) << 4);
	x = x| (getBit(a,8) << 3);
	x = x| (getBit(a,10) << 2);
	x = x| (getBit(a,12) << 1);

	unsigned char y = 0;
	y = y| (getBit(a,1) << 7);
	y = y| (getBit(a,3) << 6);
	y = y| (getBit(a,5) << 5);
	y = y| (getBit(a,7) << 4);
	y = y| (getBit(a,9) << 3);
	y = y| (getBit(a,11) << 2);
	y = y| (getBit(a,13) << 1);


	unsigned char z = 0xFF;

	if(b == 0){
	return Vec3b(z, x, y);
	}
	else if(b == 1)
	{
		return Vec3b(y, z, x);
	}
	else if(b == 2)
	{
		return Vec3b(x, y, z);
	}

	return Vec3b(0, 0, 0);

}

cv::Scalar numToColor(unsigned short num)
{
	return (Scalar)numToColor2(num);

}

unsigned short colorToNum(Vec3b color)
{
	unsigned char c = color[0];
	unsigned char d = color[1];
	unsigned char e = color[2];


    unsigned short a = 0;
	unsigned short b = 0;

	unsigned char x = 0;
	unsigned char y = 0;
	unsigned char z = 0;

	if(color[0] == 0xFF){
		b = 0;
		z = color[0];
		x = color[1];
		y = color[2];
	}
	else if(color[1] == 0xFF)
	{
		b = 1;
		z = color[1];
		x = color[2];
		y = color[0];
	}
	else if(color[2] == 0xFF)
	{
		b = 2;
		z = color[2];
		x = color[0];
		y = color[1];
	}

	a = a| (getBit(x,7) << 0);
	a = a| (getBit(x,6) << 2);
	a = a| (getBit(x,5) << 4);
	a = a| (getBit(x,4) << 6);
	a = a| (getBit(x,3) << 8);
	a = a| (getBit(x,2) << 10);
	a = a| (getBit(x,1) << 12);

	a = a| (getBit(y,7) << 1);
	a = a| (getBit(y,6) << 3);
	a = a| (getBit(y,5) << 5);
	a = a| (getBit(y,4) << 7);
	a = a| (getBit(y,3) << 9);
	a = a| (getBit(y,2) << 11);
	a = a| (getBit(y,1) << 13);


	return a * 3 + b;

}

void rotate(Mat src, Mat* dst, float angle, float scale)
{	
	cv::Point2f center(src.cols*0.5, src.rows*0.5);
	const cv::Mat affine_matrix = cv::getRotationMatrix2D( center, angle, scale );
	std::cout << "affine_matrix=\n" << affine_matrix << std::endl;
	cv::warpAffine(src, *dst, affine_matrix, src.size(), 1, 1);//1:境界値を外まで伸ばす
}

void translate(Mat src, Mat* dst, Point2d p)
{	
	cv::Mat affine_matrix =  (cv::Mat_<double>(2,3) << 1, 0, p.x, 0, 1, p.y);
	cv::warpAffine(src, *dst, affine_matrix, src.size(), 1, BORDER_REFLECT);// 3);//3:端まで来ると逆側に回る。
}

//
//Mat matCircleAA(float radOut, float radIn, bool normalize)
//{
//	int dia = (int)(radOut * 2 - 1);
//	float D = radOut * radOut;
//	float D2 = radIn * radIn;
//
//	int count = 0;
//	cv::Mat src = cv::Mat::zeros(dia, dia ,CV_32F);
//
//	cv::circle(src, cv::Point(src.cols/2, src.rows/2), radOut, 1.0, -1, CV_AA);
//	cv::circle(src, cv::Point(src.cols/2, src.rows/2), radIn, 0.0, -1, CV_AA);
//	
//
//	return src;
//}

Mat matHalfCircleAA(float radOut, float theta360, float widthratio, bool normalize, int size, float backcolor, float forecolor)
{
	float theta = theta360 * (2 * M_PI) / 360;

	int dia = size;
	if(dia == 0)
	{
		dia = (int)(ceil(radOut) * 2 + 1);
	}

	float D = radOut + 0.5;

	float count = 0;
	cv::Mat src = cv::Mat::zeros(dia, dia ,CV_32F);
	int center = (dia - 1)/2;

	float Y = widthratio * D;
	for (int y = 0; y < src.rows; y++)
	{
		for (int x = 0; x < src.cols; x++)
		{

			float d = (center - x) * (center - x) + (center - y) * (center - y);
			float temp = - (center - x) * sin(theta) + (center - y) * cos(theta);

			d = sqrt(d);
			//temp = temp/d;
			
			if(D > d && temp > 0 && Y > temp){

				float val = min(min(1.0f, D - d), Y - temp);
				
				src.at<float>(y,x) = val;
				count+= val;
			
			}

		}
	}

	if(normalize == true && count > 0)
	{
		src = src/count;
	}

	if(normalize == false)
	{
		src = src * (forecolor - backcolor) + backcolor;
	}
	
	/*imshow("SRC", src);
	waitKey(0);*/

	return src;
}

Mat matRectangleAA(float radOut, float theta360, float widthratio, bool normalize, int size, float backcolor, float forecolor)
{
	float theta = theta360 * (2 * M_PI) / 360;

	int dia = size;
	if(dia == 0)
	{
		dia = (int)(ceil(radOut) * 2 + 1);
	}

	float D = radOut + 0.5;

	float count = 0;
	cv::Mat src = cv::Mat::zeros(dia, dia ,CV_32F);
	int center = (dia - 1)/2;

	float Y = widthratio * D;
	for (int y = 0; y < src.rows; y++)
	{
		for (int x = 0; x < src.cols; x++)
		{

			float temp = - (center - x) * sin(theta) + (center - y) * cos(theta);

			
			if(Y >abs(temp)){

				float val = min(1.0f, Y - abs(temp));
				
				src.at<float>(y,x) = val;
				count+= val;
			
			}

		}
	}

	if(normalize == true && count > 0)
	{
		src = src/count;
	}

	if(normalize == false)
	{
		src = src * (forecolor - backcolor) + backcolor;
	}
	
	return src;
}


Mat matCircleAA(float radOut, float radIn, bool normalize, int size, float backcolor, float forecolor)
{
	int dia = size;
	if(dia == 0)
	{
		dia = (int)(ceil(radOut) * 2 + 1);
	}

	float D = radOut + 0.5;
	float D2 = radIn - 0.5;

	float count = 0;
	cv::Mat src = cv::Mat::zeros(dia, dia ,CV_32F);
	int center = (dia - 1)/2;

	for (int y = 0; y < src.rows; y++)
	{
		for (int x = 0; x < src.cols; x++)
		{

			float d = (center - x) * (center - x) + (center - y) * (center - y);
			d = sqrt(d);

			if(D > d && d >= D2){
				float val = min(min(1.0f, D - d), d - D2);
				
				src.at<float>(y,x) = val;
				count+= val;
			
			}

		}
	}

	if(normalize == true && count > 0)
	{
		src = src/count;
	}

	if(normalize == false)
	{
		src = src * (forecolor - backcolor) + backcolor;
	}
	
	return src;
}

//境界の値をbilinearで計算する処理を追加するべき？
Mat matCircle(float radOut, float radIn, bool normalize, int size, float backcolor, float forecolor)
{
	int dia = size;
	if(dia == 0)
	{
		dia = (int)(ceil(radOut) * 2 - 1);
	}

	float D = radOut * radOut;
	float D2 = radIn * radIn;

	int count = 0;
	cv::Mat src = cv::Mat::zeros(dia, dia ,CV_32F);
	int center = (dia - 1)/2;

	for (int y = 0; y < src.rows; y++)
	{
		for (int x = 0; x < src.cols; x++)
		{

			float d = (center - x) * (center - x) + (center - y) * (center - y);			
				if(D > d && d >= D2){
					src.at<float>(y, x) = 1;
					count++;
				}
				
			}
		}

	if(normalize == true)
	{
		src = src/count;
	}

	if(normalize == true && count > 0)
	{
		src = src/count;
	}

	if(normalize == false)
	{
		src = src * (forecolor - backcolor) + backcolor;
	}

	return src;
}



Mat matCircleU(float radOut, float radIn, bool normalize)
{
	Mat src = matCircle(radOut,radIn,normalize);
	/*int dia = (int)(radOut * 2 - 1);
	float D = radOut * radOut;
	float D2 = radIn * radIn;

	int count = 0;
	cv::Mat src = cv::Mat::zeros(dia, dia ,CV_32F);
	for (int i = 0; i < src.cols; i++)
		{
			for (int j = 0; j < src.rows; j++)
			{
				float d = (radOut - 1 - i) * (radOut - 1 - i) + (radOut - 1 - j) * (radOut - 1 - j);				
				if(D > d && d >= D2){
					src.at<float>(j,i) = 1;
					count++;
				}
				
			}
		}

	if(normalize == true)
	{
		src = src/count;
	}*/

	src.convertTo(src, CV_8U);
	return src;
}
float getSum(Mat &orig)
{
	Mat src;
	orig.convertTo(src, CV_32F);
	cv::Mat m2, m3;
	cv::reduce(src, m2, 1, CV_REDUCE_SUM);
	cv::reduce(m2, m3, 0, CV_REDUCE_SUM);
	return m3.at<float>(0);
}

float getAvg(Mat &orig)
{
	Mat src;
	orig.convertTo(src, CV_32F);
	cv::Mat m2, m3;
	cv::reduce(src, m2, 1, CV_REDUCE_AVG);
	cv::reduce(m2, m3, 0, CV_REDUCE_AVG);
	return m3.at<float>(0);
}

float getMax(Mat &orig)
{
	Mat src;
	orig.convertTo(src, CV_32F);
	cv::Mat m2, m3;
	cv::reduce(src, m2, 1, CV_REDUCE_MAX);
	cv::reduce(m2, m3, 0, CV_REDUCE_MAX);
	return m3.at<float>(0);
}

float getMin(Mat &orig)
{
	Mat src;
	orig.convertTo(src, CV_32F);
	cv::Mat m2, m3;
	cv::reduce(src, m2, 1, CV_REDUCE_MIN);
	cv::reduce(m2, m3, 0, CV_REDUCE_MIN);
	return m3.at<float>(0);
}

template<typename _Tp> static void
filter2DX_(const Mat& src, Mat& dst, Mat& msk, const vector<int>& ofsvec, const vector<double>& coeffvec)
{
    const int* ofs = &ofsvec[0];
    const double* coeff = &coeffvec[0];
    int width = dst.cols*dst.channels(), ncoeffs = (int)ofsvec.size();
    
    for( int y = 0; y < dst.rows; y++ )
    {
        const _Tp* sptr = src.ptr<_Tp>(y);
        double* dptr = dst.ptr<double>(y);
        unsigned char* mptr = msk.ptr<unsigned char>(y);

        for( int x = 0; x < width; x++ )
        {
			if(mptr[x] < 0x80)
			{
				dptr[x] = 0;
				continue;
			}
            double s = 0;
            for( int i = 0; i < ncoeffs; i++ )
                s += sptr[x + ofs[i]]*coeff[i];
            dptr[x] = s;
        }
    }
}
    
template<typename _Tp> static void
filter2DX_(const Mat& src, Mat& dst, Mat& msk, const vector<int>& ofsvec, const vector<double>& coeffvec, const vector<int>& ofsvecdiff, const vector<double>& coeffvecdiff)
{
    const int* ofs = &ofsvec[0];
    const double* coeff = &coeffvec[0];
	const int* ofsdiff = &ofsvecdiff[0];
    const double* coeffdiff = &coeffvecdiff[0];

	int width = dst.cols*dst.channels();
	int ncoeffs = (int)ofsvec.size();
	int ncoeffsdiff = (int)ofsvecdiff.size();
	
    
    for( int y = 0; y < dst.rows; y++ )
    {
        const _Tp* sptr = src.ptr<_Tp>(y);
        double* dptr = dst.ptr<double>(y);
        unsigned char* mptr = msk.ptr<unsigned char>(y);

		bool flag = true;
		double s = 0;
        for(int x = 0; x < width; x++ )
        {
			if(mptr[x] < 0x80)
			{
				dptr[x] = 0;
				flag = true;
				continue;
			}

			if(flag == true)
			{
				flag= false;

				s = 0;
				for( int i = 0; i < ncoeffs; i++ )
					s += sptr[x + ofs[i]]*coeff[i];
				dptr[x] = s;

			}
			else
			{
				for( int i = 0; i < ncoeffsdiff; i++ )
					s += sptr[x + ofsdiff[i]]*coeffdiff[i];
				dptr[x] = s;
			}
        }
    }
}

void filter2DX(const Mat& _src, Mat& dst, Mat& msk, int ddepth, const Mat& kernel,
              Point anchor, double delta, int borderType)
{
    Mat src, _dst;
    Scalar borderValue = 0;
    CV_Assert( kernel.type() == CV_32F || kernel.type() == CV_64F );
    if( anchor == Point(-1,-1) )
        anchor = Point(kernel.cols/2, kernel.rows/2);

    copyMakeBorder(_src, src, anchor.y, kernel.rows - anchor.y - 1,
                   anchor.x, kernel.cols - anchor.x - 1,
                   borderType, borderValue);
    _dst.create( _src.size(), CV_MAKETYPE(CV_64F, src.channels()) );
    
    vector<int> ofs;
    vector<double> coeff; //(kernel.rows*kernel.cols);

	vector<int> ofsdiff;
    vector<double> coeffdiff; //(kernel.rows*kernel.cols);

	Mat kernel64;
	kernel.convertTo(kernel64, CV_64F);

	int step = (int)(src.step/src.elemSize1()), cn = src.channels();
	for( int y = 0; y < kernel.rows; y++ )
	{
		double* ptr = kernel64.ptr<double>(y);
		for( int x = 0; x < kernel.cols; x++ )
		{
			if(*ptr != 0)
			{
				ofs.push_back(y*step + x*cn);
				coeff.push_back(*ptr);
			}
			ptr++;
			
		}
	}

	for( int y = 0; y < kernel.rows; y++ )
	{
		double* ptr = kernel64.ptr<double>(y);
		for( int x = -1; x < kernel.cols; x++ )
		{
			double diff = 0;
			if(x != -1)
			{
				diff += (*(ptr + x));
			}

			if(x != kernel.cols -1)
			{
				diff += (-*(ptr + x + 1));
			}

			if(diff != 0)
			{
				coeffdiff.push_back(diff);
				ofsdiff.push_back(y*step + x*cn);
			}
		}

	}

   
	if((int)ofsdiff.size() >= (int)ofs.size())
	{
		switch( src.depth() )
		{
		case CV_8U:
			filter2DX_<uchar>(src, _dst, msk, ofs, coeff);
			break;
		case CV_8S:
			filter2DX_<schar>(src, _dst, msk, ofs, coeff);
			break;
		case CV_16U:
			filter2DX_<ushort>(src, _dst, msk, ofs, coeff);
			break;
		case CV_16S:
			filter2DX_<short>(src, _dst, msk, ofs, coeff);
			break;
		case CV_32S:
			filter2DX_<int>(src, _dst, msk, ofs, coeff);
			break;
		case CV_32F:
			filter2DX_<float>(src, _dst, msk, ofs, coeff);
			break;
		case CV_64F:
			filter2DX_<double>(src, _dst, msk, ofs, coeff);
			break;
		default:
			CV_Assert(0);
		}
	}
	else
	{

		switch( src.depth() )
		{
		case CV_8U:
			filter2DX_<uchar>(src, _dst, msk, ofs, coeff, ofsdiff, coeffdiff);
			break;
		case CV_8S:
			filter2DX_<schar>(src, _dst, msk, ofs, coeff, ofsdiff, coeffdiff);
			break;
		case CV_16U:
			filter2DX_<ushort>(src, _dst, msk, ofs, coeff, ofsdiff, coeffdiff);
			break;
		case CV_16S:
			filter2DX_<short>(src, _dst, msk, ofs, coeff, ofsdiff, coeffdiff);
			break;
		case CV_32S:
			filter2DX_<int>(src, _dst, msk, ofs, coeff, ofsdiff, coeffdiff);
			break;
		case CV_32F:
			filter2DX_<float>(src, _dst, msk, ofs, coeff, ofsdiff, coeffdiff);
			break;
		case CV_64F:
			filter2DX_<double>(src, _dst, msk, ofs, coeff, ofsdiff, coeffdiff);
			break;
		default:
			CV_Assert(0);
		}

	}
	_dst.convertTo(dst, _src.type());
	return;
	

}

void convolution(Mat src, Mat *dst, Mat element, Mat *msk)
{
	Point anchor = Point( -1, -1 );
	double delta = 0;
	int ddepth = -1;
	//filter2D(src, *dst, ddepth , element, anchor, delta, BORDER_DEFAULT);
	if(msk == NULL)
	{
		filter2D(src, *dst, ddepth , element, anchor, delta, BORDER_DEFAULT);
	}
	else
	{
		filter2DX(src, *dst, *msk, ddepth , element, anchor, delta, BORDER_DEFAULT);
	}
}

//bool notexist(Mat matA)
//{
//	if(matA.empty())
//	{
//		//return true;
//	}
//	if(NULL == matA.data)
//	{
//		return true;
//	}
//
//	return false;
//}

bool toSingleChannel(Mat *inMat, Mat *outMat)
{
	if((*inMat).data == NULL)
	{
		return false;
	}

	//Mat temp;
	if((*inMat).channels() ==  1)
	{
		*outMat = *inMat;
	}
	else if((*inMat).type() == CV_8UC3)
	{
		cvtColor(*inMat, *outMat, CV_BGR2GRAY);
	}
	else
	{
		return false;
	}

	return true;

}

int Util::hist(Mat *matA, int histSize, float max, float min, bool ignorezero, Mat *histOut, float *threshout, float *aveout, float *devout, float *absaveout, int *countout, float percentage)
{
	Mat src;
	if(toSingleChannel(matA, &src) == false)
	{
		return -1;
	}
	
	src.convertTo(src, CV_32F);


	int size = 0;
	for (int y = 0; y < src.rows; y++)
	{
		float *p = src.ptr<float>(y);
		for (int x = 0; x < src.cols; x++)
		{
			if(p[x] != 0 || ignorezero == false)
			{
				size++;
			}
		}
	}

	if(size == 0)
	{
		size = 1;
	}

	Mat samples = Mat::zeros(Size(1, size), CV_32F);
	int k = 0;
	for (int y = 0; y < src.rows; y++)
	{
		float *p = src.ptr<float>(y);
		for (int x = 0; x < src.cols; x++)
		{
			if(p[x] != 0 || ignorezero == false)
			{
				samples.at<float>(k) = p[x];
				k++;
			}
		}
	}

	CvScalar mean = cvScalarAll(0); 
	CvScalar std_dev= cvScalarAll(0); 
	CvScalar mean2 = cvScalarAll(0); 
	CvScalar std_dev2= cvScalarAll(0); 

	if(size > 0)
	{
		IplImage iplImage = samples;
		cvAvgSdv(&iplImage, &mean, &std_dev); 

		Mat abssamples = abs(samples);
		IplImage iplImage2 = abssamples;
		cvAvgSdv(&iplImage2, &mean2, &std_dev2); 
	}
	

	if(aveout != NULL)
	{
		*aveout = mean.val[0];
		cout  << "ave: "<< mean.val[0] << endl;
	}
	if(devout != NULL)
	{
		*devout = std_dev.val[0];
		cout  << "dev: "<< std_dev.val[0] << endl;
	}
	if(countout != NULL)
	{
		*countout = size;
		cout  << "count: "<< size << endl;
	}

	if(absaveout != NULL)
	{
		*absaveout = mean2.val[0];
		cout  << "absAve: "<< mean2.val[0] << endl;
	}

	

  /// Set the ranges ( for B,G,R) )
  float range[] = { min, max } ;
  const float* histRange = { range };

  bool uniform = true; bool accumulate = false;

  Mat histM;

  /// Compute the histograms:
  calcHist( &samples, 1, 0, Mat(), histM, 1, &histSize, &histRange, uniform, accumulate );

  

 
  int threshindex = 0;

  if(threshout != NULL)
  {
	  int totalcount = 0;
	  // int samplesize = std::max((int)sqrt((float)size)/10, 30);
	    int samplesize = size * percentage;//std::max((int)((float)size * percentage), 30);

		if(samplesize == 0 && percentage > 0)
		{
			samplesize = 1;
		}

	  *threshout = 0;
	  for( int i = histSize - 1; i >= 0; i--)
	  {
		  totalcount+=cvRound((histM).at<float>(i));
		 
		  if(totalcount >= samplesize)
		  {
			   cout  << "size: " << samplesize << endl;
			  cout  << "index: " << i << endl;
			  float thresh = min + ((max - min) * i)/histSize;
			  cout  << "thresh: " << thresh << endl;
			  *threshout = thresh;
			  threshindex = i;
			  break;
		  }
	  }
  }

  cout  << "" << endl;

  //見やすくするための調整
  double minVal, maxVal;
  //cv::Point minLoc, maxLoc;
  cv::minMaxLoc(histM, &minVal, &maxVal, NULL, NULL); //&minLoc, &maxLoc);

  histM.type();

  if(maxVal > (size/ sqrt((double)histSize)) * 4)
  {
	  //log(histM/((double)histSize) + 1, histM);
  }

  int hist_w = 512; int hist_h = 100;
  int bin_w = cvRound( (double) hist_w/histSize );

  Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

  /// Normalize the result to [ 0, histImage.rows ]
  normalize(histM, histM, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

  ///描画
  for( int i = 0; i <= histSize; i++ )
  {
	  Scalar color = Scalar(0, 0, 255);

	  if(i <= threshindex)
	  {
		  color = Scalar(255, 0, 0);
	  }

	  if(i == 0)
	  {
	  line( histImage, Point( bin_w*(histSize-1), hist_h - cvRound((histM).at<float>(histSize-1)) ) ,
		  Point( bin_w*(histSize), hist_h - 0 ),
		  color, 2, 8, 0  );
	  }
	  else if(i == histSize)
	  {

	  line( histImage, Point( bin_w*(histSize), hist_h - 0 ) ,
		  Point( hist_w, hist_h - 0 ),
		  color, 2, 8, 0  );
	  }
	  else
	  {
	  line( histImage, Point( bin_w*(i-1), hist_h - cvRound((histM).at<float>(i-1)) ) ,
		  Point( bin_w*(i), hist_h - cvRound((histM).at<float>(i)) ),
		  color, 2, 8, 0  );
	  }
  }
 

  for( int i = 0; i < histSize; i = i+ (histSize/10) )
  {
      line( histImage, Point( bin_w*(i+1), 0) ,
                       Point( bin_w*(i+1), hist_h),
                       Scalar( 0, 255, 0), 1, 8, 0  );
  }

 if(histOut != NULL)
 {
	 *histOut = histImage;
 }

  return 0;
}

int Util::findBG(Mat src, Mat *dst, int *avgcolor, int blurrad){

	if(blurrad == 0)
	{
		blurrad = min(src.cols, src.rows)/10 + 1;
	}
		  Mat result, temp, bg;
		 /* cv::Mat m2, m3;
		  cv::reduce(src, m2, 1, CV_REDUCE_AVG);
		  cv::reduce(m2, m3, 0, CV_REDUCE_AVG);*/

		  float avg = getAvg(src); //m3.at<uchar>(0);

		  blur(src, temp, Size(blurrad,blurrad));
		  bg = src.clone();

		  for(int i = 0; i < bg.rows; i++ )
		  {
			  for(int j = 0; j < bg.cols; j++ )
			  {
				  int a = src.at<uchar>(i,j);
				  int b = temp.at<uchar>(i,j);
				  if(abs(a-b) > 0x10){
					  bg.at<uchar>(i,j) = (unsigned char)avg;
				  }
			  }
		  }

		  blur(bg, temp, Size(blurrad,blurrad));

		  for(int i = 0; i < bg.rows; i++ )
		  {
			  for(int j = 0; j < bg.cols; j++ )
			  {
				  int a = src.at<uchar>(i,j);
				  int b = temp.at<uchar>(i,j);
				  if(abs(a-b) > 0x10){
					  bg.at<uchar>(i,j) = b;
				  }
			  }
		  }

		  float avg2 = getAvg(bg);
		  *avgcolor = (int)avg2;

		  if(dst != NULL)
		  {
			  blur(bg, bg, Size(blurrad/2,blurrad/2));
			  result = avg + (src - bg);
			  *dst = result;
		  }

		 
		 
		  return 0;

  }



cv::Vec3b IntTocolor(int hueint){

	
   float hue = ((float)hueint * 330)/0xFF;

   float saturation = 1.0f;
   float value = 1.0f;

   int r = 0;
   int g = 0;
   int b = 0;
   int region;
   float fraction;
   int min,max,up,down;

   //入力データのチェックと補正
   while(hue>360.0f || hue<0.0f){
      if(hue>=360.0f) hue-=360.0f;
      else if(hue<0.0f) hue+=360.0f;
   }
   if(saturation>1.0f) saturation=1.0f;
   else if(saturation<0.0f) saturation=0.0f;
   if(value>1.0f) value=1.0f;
   else if(value<0.0f) value=0.0f;

   max=(int)(value*255);

   if(saturation==0.0f){
      r=max;
      g=max;
      b=max;
   }
   else{
      region=(int)(hue/60.0f);
      fraction=hue/60.0f-region;
      min=(int)(max*(1.0f-saturation));
      up= min+(int)(fraction*max*saturation);
      down=max-(int)(fraction*max*saturation);

      switch(region){
         case 0:r= max; g= up; b= min; break;     //赤→黄
         case 1:r=down; g= max; b= min; break;    //黄→緑
         case 2:r= min; g= max; b= up; break;     //緑→シアン
         case 3:r= min; g=down; b= max; break;    //シアン→青 
         case 4:r= up; g= min; b= max; break;     //青→マゼンタ
         case 5:r= max; g= min; b=down; break;    //マゼンタ→赤
      }
   }

   return cv::Vec3b(r,g,b);
   
}


void addcolor(cv::Mat &imageIn, cv::Mat &imageAdd, int thresh, int colornum)
{


	cv::Vec3b color;
	cv::Vec3b colors[256];
	for(int i = 0; i < 256; i++){
		colors[i] = IntTocolor(i);;
	}



	for (int y = 1; y < imageIn.rows -1 ; y++) {
		for (int x = 1; x < imageIn.cols -1 ; x++) {
			int num = imageAdd.at<uchar>(y, x);

			if(colornum == 0){
				color = colors[num];
			}
			else{
				color = colors[colornum];
			}

			if(num >= thresh){
				/* imageIn.at<cv::Vec3b>(y,x-1) = color;
				imageIn.at<cv::Vec3b>(y+1,x-1) = color;
				imageIn.at<cv::Vec3b>(y-1,x-1) = color;*/
				//imageIn.at<cv::Vec3b>(y-1,x) = color;
				//imageIn.at<cv::Vec3b>(y-1,x+1) = color;


				imageIn.at<cv::Vec3b>(y,x) = color;
				imageIn.at<cv::Vec3b>(y+1,x) = color;			  
				imageIn.at<cv::Vec3b>(y,x+1) = color;
				imageIn.at<cv::Vec3b>(y+1,x+1) = color;

			}
		}
	}
}


void addcolor2(cv::Mat &imageIn, cv::Mat &imageAdd, Mat &radius, int thresh, int colornum)
{

	cv::Vec3b color;
	cv::Vec3b colors[256];
	for(int i = 0; i < 256; i++){
		colors[i] = IntTocolor(i);;
	}

	for (int y = 1; y < imageIn.rows -1 ; y++) {
		for (int x = 1; x < imageIn.cols -1 ; x++) {
			int num = imageAdd.at<uchar>(y, x);
			int num2 = (radius.at<uchar>(y, x) * 0x20) % 0x100;
			if(colornum == 0){
				color = colors[num2];
			}
			else{
				color = colors[colornum];
			}

			if(num >= thresh){

				int rad = radius.at<uchar>(y, x);
				cv::circle(imageIn, cv::Point(x, y), rad, (Scalar)color, 1, CV_AA);

			}
		}
	}
}


void matToDataF(Mat &imageAdd, Mat &radius, float thresh, float mult, int *size, float **X, float **Y, float **Data, float **R)
{

	int count = 0;



	for (int y = 0; y < imageAdd.rows; y++) {
		for (int x = 0; x < imageAdd.cols ; x++) {
			float num = imageAdd.at<float>(y, x);
			if(num >= thresh){
				count++;
			}
		}
	}



	*X = new float[count];	
	*Y = new float[count];	
	*Data = new float[count];	
	*R = new float[count];	


	count = 0;
	for (int y = 0; y < imageAdd.rows; y++) {
		for (int x = 0; x < imageAdd.cols ; x++) {
			float num = imageAdd.at<float>(y, x);
			float num2 = radius.at<float>(y, x);
			if(num >= thresh){
				(*X)[count] = (float)x / mult;
				(*Y)[count] = (float)y / mult;
				(*Data)[count] = num;
				(*R)[count] = num2 / mult;
				count++;

				if(count >= MAXCOUNT)
				{   
					*size = count;
					return;
				}
			}
		}
	}

	*size = count;
	return;
}

void addcolor3(cv::Mat &imageIn, float mult, int size, float *X, float *Y, float *R)
{

	cv::Vec3b color;
	cv::Vec3b colors[256];
	for(int i = 0; i < 256; i++){
		colors[i] = IntTocolor(i);
	}

	

	for (int i = 0; i < size; i++) {
			float x = X[i];
			float y = Y[i];
			float rad = R[i];
			int num2 = (int)(rad * mult * 0x20) % 0x100;

				color = colors[num2];
				cv::circle(imageIn, cv::Point(x * mult , y * mult), (int)(rad * mult), (Scalar)color, max(1.0f, (mult + 1)/2), CV_AA);

		}

}

void addcolor4(cv::Mat &imageIn, int size, int *X, int *Y, float *R)
{

	cv::Vec3b color;
	cv::Vec3b colors[256];
	for(int i = 0; i < 256; i++){
		colors[i] = IntTocolor(i);;
	}

	

	for (int i = size-1; i >= 0; i--) {
			int x = X[i];
			int y = Y[i];
			float rad = R[i];
			int num2 = (int)(i * 17) % 0x100;

				color = colors[num2];
				cv::circle(imageIn, cv::Point(x, y), (int)rad, (Scalar)color, 1, CV_AA);
				 
				
				

		}

}

void compare(Mat inMatA, Mat inMatB){
	
	for(int y=0; y<inMatA.rows; y++)
	{
		float *p = inMatA.ptr<float>(y);
		float *q = inMatB.ptr<float>(y);

		for(int x=0; x<inMatA.cols; x++)
		{			

			if(abs(*q) > abs(*p))
			{
				*p = *q;
			}

			p++;
			q++;
		}
	}

}

void compare(Mat inMatA, Mat inMatB, Mat inMatC, float num){
	
	for(int y=0; y<inMatA.rows; y++)
	{
		float *p = inMatA.ptr<float>(y);
		float *q = inMatB.ptr<float>(y);
		float *r = inMatC.ptr<float>(y);

		for(int x=0; x<inMatA.cols; x++)
		{			

			if(abs(*q) > abs(*p))
			{
				*p = *q;
				*r = num;
			}

			p++;
			q++;
			r++;

		}
	}

}

void compareMin(Mat inMatA, Mat inMatB){
	
	for(int y=0; y<inMatA.rows; y++)
	{
		float *p = inMatA.ptr<float>(y);
		float *q = inMatB.ptr<float>(y);

		for(int x=0; x<inMatA.cols; x++)
		{			

			if(abs(*q) < abs(*p) || (*p == 0 && *q != 0))
			{
				*p = *q;
			}

			p++;
			q++;
		}
	}

}

void compareMin(Mat inMatA, Mat inMatB, Mat inMatC, float num){
	
	for(int y=0; y<inMatA.rows; y++)
	{
		float *p = inMatA.ptr<float>(y);
		float *q = inMatB.ptr<float>(y);
		float *r = inMatC.ptr<float>(y);

		for(int x=0; x<inMatA.cols; x++)
		{			

			if(abs(*q) < abs(*p) || (*p == 0 && *q != 0))
			{
				*p = *q;
				*r = num;
			}

			p++;
			q++;
			r++;

		}
	}

}