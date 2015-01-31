#include "stdafx.h"
#include "Filters.h"

float localPeaksub(Mat src, int y0, int x0, Mat element)
{
	Rect rec(x0 - element.cols/2, y0 - element.rows/2, element.cols, element.rows);
	if(IsLegal(src, rec) == false)
	{
		return src.at<float>(y0, x0);
	}


	Mat roi = src(rec).mul(element);
	
	Point maxloc = Point(0, 0);
	float max = roi.at<float>(0,0); //getMax(roi);


	for(int y=0; y<roi.rows; y++)
	{
		float *p = roi.ptr<float>(y);
		for(int x=0; x<roi.cols; x++)
		{		
			if(p[x] > max)
			{
				max = p[x];
				maxloc = Point(y,x);
			}
		}
	}

	maxloc += Point(rec.x, rec.y);


	if((maxloc.x == x0 && maxloc.y == y0))
	{
		return max;
	}
	else
	{
		return 0;
	}
}

void localPeak(Mat src0, Mat *dst, Mat *msk0, float radius, float thresh){
	//アルゴリズムを削除型に修正した方がよい?
	Mat src;
	if(src0.type() != CV_32F)
	{
		src0.convertTo(src, CV_32F);
	}
	else
	{
		src = src0;
	}
	*dst = Mat::zeros(src.rows, src.cols, CV_32F);

	Mat msk;
	if(msk0 == NULL)
	{
		msk = Mat::ones(src.rows, src.cols, CV_32F);
	}
	else
	{
		msk = *msk0;
	}

	Mat element = matCircle(radius, 0, false);

	for(int y=0; y<src.rows; y++)
	{
		float *p = src.ptr<float>(y);
		float *q = (*dst).ptr<float>(y);
		float *r = msk.ptr<float>(y);

		for(int x=0; x<src.cols; x++)
		{			
			if( x < radius - 1 || x + radius - 1 >= src.cols || y < radius - 1|| y + radius - 1 >= src.rows|| *r < 0.5 || *p < thresh)
			{
				*q = 0.0;
			}
			else
			{
				*q = localPeaksub(src, y, x, element);

			}

			p++;
			q++;
			r++;

		}
	}
	return;
}

void diffx(Mat src0, Mat *dst, int step){

	Mat src;
	if(src0.type() != CV_32F)
	{
		src0.convertTo(src, CV_32F);
	}
	else
	{
		src = src0;
	}

	*dst = Mat::zeros(src.rows, src.cols, CV_32F);

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
		float *p = src.ptr<float>(y) + minx;
		float *q = (*dst).ptr<float>(y) + minx;
		for(int x=minx; x<maxx; x++)
		{			
			*q = *(p + step) - *p;
			p++;
			q++;
		}
		
	}
	return;
}

void diffy(Mat src0, Mat *dst, int step){

	Mat src;
	if(src0.type() != CV_32F)
	{
		src0.convertTo(src, CV_32F);
	}
	else
	{
		src = src0;
	}

	*dst = Mat::zeros(src.rows, src.cols, CV_32F);

	

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
		float *p = src.ptr<float>(y);
		float *q = (*dst).ptr<float>(y);
		for(int x=0; x<src.cols; x++)
		{			
			*q = *(p + dist) - *p;
			p++;
			q++;
		}
		
	}
	return;
}

void solvePoisson(Mat L, Mat *dst, int bgcolor)
{

	*dst = Mat::ones(L.rows, L.cols, CV_32F) * bgcolor;

	int h = L.rows;
	int w = L.cols;
	int ITERATION = 300;

	for (int loop=0; loop< ITERATION; loop++) {
		int changed = 0;
		for (int y=1; y<h-1; y++) {
			float *p = (*dst).ptr<float>(y) + 1;
			float *q = L.ptr<float>(y) + 1;
			for (int x=1; x<w-1; x++) {	
				float oldval = *p;
				float value = 0;
				value += *(p+1);
				value += *(p-1);
				value += *(p+w);
				value += *(p-w);

				float newval = (value - *q) / 4;

				*p = newval;
				if(abs(oldval - newval) > 0.5)
				{
					changed++;
				}
				p++;
				q++;
			}


		}
		if(changed == 0)
		{
			break;
		}
	}

}

int poissonSub1(Mat src, Mat *dst, int blurrad)
{

	Mat _Mblurred;
	Mat _Mdifference;
	cv::medianBlur(src, _Mblurred, blurrad * 2 + 1);
	_Mdifference = abs(src - _Mblurred);
	_Mdifference.convertTo(_Mdifference, CV_32F);
	cv::blur(_Mdifference, *dst, Size(3,3));

	return 0;
}
int ConvertFuncs(convtype FT, Mat src, Mat *dst, int param1, float fparam1, float fparam2, Mat *mask)
{
	if(dst == NULL)
	{
		return -1;
	}

	switch(FT)
	{	
	case COLOR:	
		cvtColor(src, *dst, param1);
		break;
	case FLOATTOCHAR:
		if(fparam1 == 0)
		{
			*dst = src;
		}
		else{
			*dst = src * fparam1;
		}
		(*dst).convertTo(*dst, CV_8U);
		break;
    case CHARTOFLOAT:
		src.convertTo(*dst, CV_32F);
		if(fparam1 != 0)
		{
			*dst = *dst * fparam1;
		}
		break;
	case LOCALPEAK:
		localPeak(src, dst, mask, fparam1, fparam2);
		break;
	case CLONE:
		*dst = src.clone();
		break;
	case INVERT:
		if(param1 != 0)
		{
			*dst = 0xFF - src;
		}
		else
		{
			*dst = src.clone();
		}
		break;
	case DIFFX:
		diffx(src, dst, param1);
		break;		
	case DIFFY:
		diffy(src, dst, param1);
		break;
    case SOLVEPOISSON:
		solvePoisson(src, dst, param1);
		break;
	case POISSON_SUB1:
		poissonSub1(src, dst, param1);
		break;
	default:
		return -1;
	}
	return 0;
}


int lineseparability(Mat src, Mat *dst, float radius, float theta360, Mat *mask, Mat *sq)
{
	Mat SQ;
	if(sq == NULL)
	{
		SQ = src.mul(src);
		sq = &SQ;
	}
	
	Mat R1 = matHalfCircleAA(radius, theta360, 0.5);
	Mat R2 = matHalfCircleAA(radius, theta360 + 180, 0.5);

	

	float sizeR1 = getSum(R1);
	float sizeR2 = getSum(R2);

	Mat uiTotalR1, uiTotalR2, uiTotalPi1, uiTotalPi2;
	convolution(src, &uiTotalR1, R1, mask);
	convolution(src, &uiTotalR2, R2, mask);  	
	convolution(*sq, &uiTotalPi1, R1, mask);
	convolution(*sq, &uiTotalPi2, R2, mask);  

	Mat uiTotalPi = uiTotalPi1 + uiTotalPi2;

	Mat dlP1 = (uiTotalR1) / sizeR1;
	Mat dlP2 = (uiTotalR2) / sizeR2;	

	Mat dlPm = ((uiTotalR1 + uiTotalR2)) / (sizeR1 + sizeR2);

	Mat dlSigmaB = ((dlP1 - dlPm).mul(dlP1 - dlPm) * sizeR1) + ((dlP2 - dlPm).mul(dlP2 - dlPm) * sizeR2);
	Mat dlSigmaT = (uiTotalPi) - dlPm.mul(dlPm) * (sizeR1 + sizeR2);

	Mat dlMu = dlSigmaB / dlSigmaT;


	Mat diff = abs(dlP1 - dlP2);
	float thresh = getAvg(diff);

	dlMu += diff / thresh;
	dlMu *= 0.5;

	//ThreshFuncs<float>(THRESH, diff, dlMu, 0, dlMu, thresh * 1.5);

	
	//IMSHOW("M", dlMu * 0xFF);
	
	//dlMu *= 1.0 - (1.0/radius);
	//dlMu = (dlMu / 0.17);

	//if(fparam > 0)
	//{
	//	//内側が黒くないときは判定しない。
	//	ThreshFuncs<float>(THRESH, dlP1 - dlP2, dlMu, 0, dlMu, fparam);
	//}
	//if(fparam < 0)
	//{
	//	//内側が白くないときは判定しない。
	//	ThreshFuncs<float>(THRESH, dlP2 - dlP1, dlMu, 0, dlMu, fparam);
	//}

	*dst = dlMu;

	return 0;
}

int lineaverage(Mat src, Mat *dst, float radius, float theta360, Mat *mask)
{

	
	Mat R1 = matRectangleAA(radius, theta360, 0.2, true);

	float sizeR1 = getSum(R1);

	Mat uiTotalR1;
	convolution(src, &uiTotalR1, R1, mask);

	*dst = uiTotalR1;

	return 0;
}


int separability(Mat src, Mat *dst, float radius, float radiusin, float radiusout, Mat *mask, float fparam, Mat *sq)
{
	Mat SQ;
	if(sq == NULL)
	{
		SQ = src.mul(src);
		sq = &SQ;
	}
	
	Mat R1 = matCircleAA(radiusout, radius);
	Mat R2 = matCircleAA(radius, radiusin);

	float sizeR1 = getSum(R1);
	float sizeR2 = getSum(R2);

	Mat uiTotalR1, uiTotalR2, uiTotalPi1, uiTotalPi2;
	convolution(src, &uiTotalR1, R1, mask);
	convolution(src, &uiTotalR2, R2, mask);  	
	convolution(*sq, &uiTotalPi1, R1, mask);
	convolution(*sq, &uiTotalPi2, R2, mask);  

	Mat uiTotalPi = uiTotalPi1 + uiTotalPi2;

	Mat dlP1 = (uiTotalR1) / sizeR1;
	Mat dlP2 = (uiTotalR2) / sizeR2;	

	Mat dlPm = ((uiTotalR1 + uiTotalR2)) / (sizeR1 + sizeR2);

	Mat dlSigmaB = ((dlP1 - dlPm).mul(dlP1 - dlPm) * sizeR1) + ((dlP2 - dlPm).mul(dlP2 - dlPm) * sizeR2);
	Mat dlSigmaT = (uiTotalPi) - dlPm.mul(dlPm) * (sizeR1 + sizeR2);

	Mat dlMu = dlSigmaB / dlSigmaT;

	//dlMu *= 1.0 - (1.0/radius);
	//dlMu = (dlMu / 0.17);

	if(fparam > 0)
	{
		//内側が黒くないときは判定しない。
		ThreshFuncs<float>(THRESH, dlP1 - dlP2, dlMu, 0, dlMu, fparam);
	}
	if(fparam < 0)
	{
		//内側が白くないときは判定しない。
		ThreshFuncs<float>(THRESH, dlP2 - dlP1, dlMu, 0, dlMu, fparam);
	}

	*dst = dlMu;
	return 0;
}

int colordiff(Mat src, Mat *dst, float radius, float radiusin, float radiusout, Mat *mask = NULL)
{
	Mat R1 = matCircle(radiusout, radius);
	Mat R2 = matCircle(radius, radiusin);

	float sizeR1 = getSum(R1);
	float sizeR2 = getSum(R2);

	Mat uiTotalR1, uiTotalR2;
	convolution(src, &uiTotalR1, R1, mask);
	convolution(src, &uiTotalR2, R2, mask);  	


	Mat dlP1 = (uiTotalR1) / sizeR1;
	Mat dlP2 = (uiTotalR2) / sizeR2;	

	*dst = dlP1 - dlP2;
	return 0;
}

int density(Mat src, Mat *dst, float radius, float radiusin = 0, Mat *mask = NULL)
{	

	Mat R1 = matCircle(radius, radiusin, true);
	convolution(src, dst, R1, mask);
	return 0;
}

int CircleFuncsSub(circletype FT, Mat &src, Mat *dst, float radius, float radiusin, float radiusout, Mat *mask, float fparam1, Mat *sq)
{
	if(dst == NULL)
	{
		return -1;
	}


	switch(FT)
	{
	case LOCALPEAK2:
		localPeak(src, dst, mask, radius * 1.33, fparam1);
		break;
	case SEPARABILITY:	
		separability(src, dst, radius, radiusin, radiusout, mask, fparam1, sq);
		break;
	case LINESEPARABILITY:	
		lineseparability(src, dst, radius, fparam1, mask, sq);
		break;
	case COLORDIFFERENCE:	
		colordiff(src, dst, radius, radiusin, radiusout, mask);
		break;
	case DENSITYIN:
		density(src, dst, radiusin, 0, mask);
		break;
	case DENSITYOUT:
		density(src, dst, radiusout, 0, mask);
		break;
	case DENSITYRING:
		density(src, dst, radius, radiusin, mask);
		break;
	

	default:
		return -1;
	}
	return 0;
}



int CircleFuncs(circletype FT, Mat *src0, Mat *dst, Mat *mask, float radius, float radiusin, float radiusout, float fparam1, Mat *dstRad, Mat temps[], int repeat, float step)
{
	Mat src;

	switch(src0->type())
	{
	case CV_8U:
		src0->convertTo(src, CV_32F);
		break;

	case CV_32F:
		src = *src0;
		break;
	default:
		return -1;
	}

	
	*dst = Mat::zeros(src.size(), CV_32F);
	if(dstRad != NULL)
	{
		*dstRad = Mat::zeros(src.size(), CV_32F) * radius;
	}
	

	if(radiusin == radiusout)
	{
		radiusin = radius - 2;
		radiusout = radius + 2;
	}

	if(repeat >= 1)
	{

		for(int i = 0; i < repeat; i++){

			if(FT == LOCALPEAK2)
			{
				CircleFuncsSub(FT, *(src0 + i), &temps[i], i * step + radius, i * step + radiusin, i * step + radiusout, mask, fparam1);
			}
			else
			{
				CircleFuncsSub(FT, src, &temps[i], i * step + radius, i * step + radiusin, i * step + radiusout, mask, fparam1);
			}


			if(dstRad == NULL)
			{
				compare(*dst, temps[i]);
			}
			else
			{			
				compare(*dst, temps[i], *dstRad, i * step + radius);
				if(FT == LOCALPEAK2)
				{
				}
			}
		}
	}
	/*else
	{ 
		
		CircleFuncsSub(FT, src, dst, radius, radiusin, radiusout, mask, fparam1);
	}*/

	return 0;
}



template <class T>
T Funcs(threshtype FT, T src, T max, T min, T param1, T param2, float fparam1)
{
	switch(FT)
	{
	case TOMAX:	
		return max;
	

		break;
	case THRESH:
		if(src >= param1)
		{
			return max;
		}
		else
		{
			return min;
		}

		break;

	case THRESH2:
		if(src >= param1 || src <= param2)
		{
			return max;
		}
		else
		{
			return min;
		}

		break;

	case HUE_THRESH:

		if( abs(((int)src - (int)param1 + 180 * 2 ) % 180) < param2 )
		{
			return max;
		}
		else
		{
			return min;
		}
		break;

	default:
		return 0;
	}

		
}

template <class T>
int ThreshFuncs(threshtype FT, Mat src, Mat srcMax, Mat srcMin, Mat dst, T param1, T param2, float fparam1){

	for(int y=0; y<src.rows; y++)
	{
		T *p = src.ptr<T>(y);
		T *max = srcMax.ptr<T>(y);
		T *min = srcMin.ptr<T>(y);
		T *q = dst.ptr<T>(y);

		for(int x=0; x<src.cols; x++)
		{
			*q = Funcs<T>(FT, *p, *max, *min, param1, param2);
			
			 p++;
			 max++;
			 min++;
			 q++;
		}
	}
	return 0;
}

template <class T>
int ThreshFuncs(threshtype FT, Mat src, T max, Mat srcMin, Mat dst, T param1, T param2, float fparam1){

	for(int y=0; y<src.rows; y++)
	{
		T *p = src.ptr<T>(y);
		T *min = srcMin.ptr<T>(y);
		T *q = dst.ptr<T>(y);

		for(int x=0; x<src.cols; x++)
		{
			*q = Funcs<T>(FT, *p, max, *min, param1, param2);
			
			 p++;
			 min++;
			 q++;
		}
	}
	return 0;
}

template <class T>
int ThreshFuncs(threshtype FT, Mat src, Mat srcMax, T min, Mat dst, T param1, T param2, float fparam1){

	for(int y=0; y<src.rows; y++)
	{
		T *p = src.ptr<T>(y);
		T *max = srcMax.ptr<T>(y);
		T *q = dst.ptr<T>(y);

		for(int x=0; x<src.cols; x++)
		{
			*q = Funcs<T>(FT, *p, *max, min, param1, param2);
			
			 p++;
			 max++;
			 q++;
		}
	}
	return 0;
}

template <class T>
int ThreshFuncs(threshtype FT, Mat src, T max, T min, Mat dst, T param1, T param2, float fparam1){

	for(int y=0; y<src.rows; y++)
	{
		T *p = src.ptr<T>(y);
		T *q = dst.ptr<T>(y);

		for(int x=0; x<src.cols; x++)
		{
			*q = Funcs<T>(FT, *p, max, min, param1, param2);			
			 p++;
			 q++;
		}
	}
	return 0;
}


template <class T> 
int ThreshFuncs(threshtype FT, Mat *src, Mat *srcMax, Mat *srcMin, T max, T min, Mat *dst, T param1, T param2, float fparam1)
{
	

	if(srcMin == NULL)
	{
		if(srcMax == NULL){
			ThreshFuncs<T>(FT, *src, max, min, *dst, param1, param2, fparam1);
		}
		else
		{
			ThreshFuncs<T>(FT, *src, *srcMax, min, *dst, param1, param2, fparam1);
		}
	}
	else{
		if(srcMax == NULL){
			ThreshFuncs<T>(FT, *src, max, *srcMin, *dst, param1, param2, fparam1);
		}
		else
		{
			ThreshFuncs<T>(FT, *src, *srcMax, *srcMin, *dst, param1, param2, fparam1);
		}
	}
	return 0;
}

template <class T>
int ThreshFuncs()
{
	threshtype FT = (threshtype)0;
	Mat src, dst;
	T max = 0;
	T min = 0;
	Mat srcMax, srcMin;
	T param1 = 0;
	T param2 = 0;
	float fparam1 = 0;

	ThreshFuncs<T>(FT, src, max, min, dst, param1, param2, fparam1);
	ThreshFuncs<T>(FT, src, srcMax, min, dst, param1, param2, fparam1);
	ThreshFuncs<T>(FT, src, max, srcMin, dst, param1, param2, fparam1);
	ThreshFuncs<T>(FT, src, srcMax, srcMin, dst, param1, param2, fparam1);

	ThreshFuncs<T>(FT, &src, &srcMax, &srcMin, max, min, &dst, param1, param2, fparam1);
	return 0;
}

int ThreshFuncs()
{
	ThreshFuncs<unsigned char>();
	ThreshFuncs<float>();
	ThreshFuncs<double>();
	return 0;
}
