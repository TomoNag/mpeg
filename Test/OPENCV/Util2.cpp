#include "Util.h"

Mat resize(Mat src, int maxsize)
{
	int width = src.cols;
	int height = src.rows;

	if(width >= height)
	{
		width = maxsize;
		height = height * maxsize / src.cols;
	}
	else
	{
		width = width * maxsize / src.rows;
		height = maxsize;
	}

	Mat dst;
	resize(src, dst, Size(width, height));
	return dst;
}

Mat connect(Mat mats[], int numH, int numV) {

	int img_numV = numV;
	int img_numH = numH;
	vector<Mat> src_img;
	
	int width = 0;
	int height = 0;

	for (int i = 0; i < img_numV * img_numH; i++) {
		src_img.push_back(mats[i]);
		height = max(mats[i].rows, height);
		width =  max(mats[i].cols, width);
	}

	int total_height = height * img_numV;
	int total_width = width * img_numH;
	Mat combined_img(Size(total_width, total_height), CV_8UC3);

	vector<Mat>::iterator it = src_img.begin(), it_end = src_img.end();
	Rect roi_rect;
	int cnt = 0;
	for (; it != it_end; ++it) {
		roi_rect.width = it->cols;
		roi_rect.height = it->rows;

		Mat roi(combined_img, roi_rect);
		it->copyTo(roi);
		roi_rect.y += it->rows; 
		cnt++;
		if(cnt == img_numV){
			roi_rect.x += it->cols;
			roi_rect.y = 0;
			cnt = 0;
		}
	}


	return  combined_img;
}

double sigmoid(double x)
{
	return 1.0 / ( 1.0 + exp(-x));
}

Point center(vector<Point> points)
{
	double totalx = 0;
	double totaly = 0;
	for (int i = 0; i < points.size(); i++)
	{
		totalx += points[i].x;
		totaly += points[i].y;
	}

	return Point(totalx/ points.size(), totaly/ points.size());

}

Point center(Mat src)
{
	int x= src.cols / 2;
	int y= src.rows / 2;

	return Point(x, y);
}

Point randompoint(int width, int height)
{

	int x= rand() % width;
	int y= rand() % height;

	return Point(x, y);

}

Mat hsv(Mat src, int i)
{
	Mat temp;
	cv::cvtColor(src, temp, CV_BGR2HSV);
	vector<Mat> hsv;
	split(temp, hsv);

	return hsv[i];
}
Mat blurredcircle(int matsize, int backcolor, int forecolor, int radius, int blurrad)
{

	Mat src = matCircle(radius, 0, false, matsize, backcolor, forecolor);

	src.convertTo(src, CV_8U);

	if(blurrad > 0)
	{
		blur(src, src, Size(blurrad, blurrad));
	}

	return src;
}

Mat labelToImg(Mat label0)
{
	Mat label;
	label0.convertTo(label, CV_32S);
	Mat img = Mat::zeros(label.size(), CV_8UC3);
	for (int y = 0; y < label.rows; y++)
	{
		int* p = label.ptr<int>(y);
		Vec3b* q = img.ptr<Vec3b>(y);
		for (int x = 0; x < label.cols; x++)
		{
			if(p[x] > 0)
			{
				q[x] = numToColor2(p[x] - 1);
			}
		}
	}

	return img;
}

double matdistU(Mat src, Mat src2)
{
	double total = 0;

	if(src.type() == CV_8U)
	{

		for (int y = 0; y < src.rows; y++)
		{
			unsigned char* p = src.ptr<unsigned char>(y);
			unsigned char* q = src2.ptr<unsigned char>(y);
			for (int x = 0; x < src.cols; x++)
			{

				total += ((int)p[x] - q[x]) * ((int)p[x] - q[x]);//abs(p[x] - q[x]);

			}
		}
	}
	else if(src.type() == CV_8UC3)
	{
		for (int y = 0; y < src.rows; y++)
		{
			Vec3b* p = src.ptr<Vec3b>(y);
			Vec3b* q = src2.ptr<Vec3b>(y);
			for (int x = 0; x < src.cols; x++)
			{

				total += dist(p[x], q[x]);//abs(p[x] - q[x]);

			}
		}
	}
	return total;
}


double matdistF(Mat src, Mat src2)
{
	double total = 0;
	for (int y = 0; y < src.rows; y++)
	{
		float* p = src.ptr<float>(y);
		float* q = src2.ptr<float>(y);
		for (int x = 0; x < src.cols; x++)
		{

			total += (p[x] - q[x]) * (p[x] - q[x]);//abs(p[x] - q[x]);

		}
	}
	return total;
}

double matdistL1F(Mat src, Mat src2)
{
	double total = 0;
	for (int y = 0; y < src.rows; y++)
	{
		float* p = src.ptr<float>(y);
		float* q = src2.ptr<float>(y);
		for (int x = 0; x < src.cols; x++)
		{

			total += abs(p[x] - q[x]);

		}
	}
	return total;
}

Rect patchRect(Size patchsize, Point patchpos)
{
	int w = patchsize.width;
	int h = patchsize.height;

	Rect rect(patchpos.x - w/2, patchpos.y - h/2, w, h);
	return rect;
}

Mat patchMat(Mat img, Size patchsize, Point patchpos)
{
	return img(patchRect(patchsize, patchpos));
}

double mathcing(Mat src0, Size patchsize, Point patchpos, Point searchpos)
{
	int w = patchsize.width;
	int h = patchsize.height;

	Rect rect(patchpos.x - w/2, patchpos.y - h/2, w, h);
	Rect rect2(searchpos.x - w/2, searchpos.y - h/2, w, h);

	if(!IsLegal(src0, rect2) || !IsLegal(src0, rect))
	{
		return 0;
	}

	Mat patch = src0(rect);
	Mat patch2 = src0(rect2);
	

	double dist = matdistU(patch, patch2);


	double var = 0x20 * 0x20 * patchsize.width * patchsize.height;
	
	double score = exp(- dist/ var);

	//score = min(score, 1.0);
	
	return score;
}

void mathcing(Mat src0, Size patchsize, Point patchpos, Size searchsize, Point searchpos, Point *foundpos, double *maxval)
{
	int w = patchsize.width;
	int h = patchsize.height;

	Rect rect(patchpos.x - w/2, patchpos.y - h/2, w, h);
	Rect trim(searchpos.x - w/2, searchpos.y - h/2, searchsize.width + w, searchsize.height + h);

	if(!IsLegal(src0, trim)||!IsLegal(src0, rect))
	{
		*foundpos = Point(0, 0);
		*maxval = 0;
		return;
	}

	Mat patch = src0(rect).clone();
	//Mat mskpatch = msk0(rect).clone();
	Mat src = src0(trim);
	//Mat msk = msk0(trim);

	double mincost = -1;
	Point minpos = Point(0, 0);
	for(int y = h/2; y < src.rows - h/2; y++){
		Vec3b* p = src.ptr<Vec3b>(y);
		//unsigned char* r = msk.ptr<unsigned char>(y);
		for(int x = w/2; x < src.cols - w/2; x++){
			Point pos2 = Point(x, y);
			Rect rect2(x - w/2, y - h/2, w, h);

			double cost = matdistU(patch, src(rect2));
			if(mincost < 0 || cost < mincost)
			{
				mincost = cost;
				minpos = pos2;
			}
		}
	}

	*foundpos = minpos + Point(trim.x, trim.y);

	double var = 0x20 * 0x20 * patchsize.width * patchsize.height;
	*maxval = exp(- mincost/ var);

	return;
}

Mat correlation(Mat src0, Point pos)
{
	int w = 5;
	int h = w;
	int W = 35;
	int H = W;

	Mat hist = Mat::zeros(Size(W/w, H/h), CV_32F);
	Rect rect(pos.x - W/2, pos.y - H/2, W, H);
	if(!IsLegal(src0, rect))
	{
		return hist;
	}

	Size patchsize = Size(w, h);
	Size searchsize = Size(w, h);

	for(int y = 0; y < hist.rows; y++){
		float* p = hist.ptr<float>(y);
		for(int x = 0; x < hist.cols; x++){
			Point foundpos;
			double maxval = 0;
			Point searchpos = pos + Point(w * (x - hist.cols/2 - 1), h * (y - hist.rows/2 - 1));
			mathcing(src0, patchsize, pos, searchsize, searchpos, &foundpos, &maxval);
			if(maxval < 1)
			{
				p[x] = maxval;
			}
		}
	}

	return hist;

}

Mat labelLP(int radius, int divRad, int divArg)
{
	int size = radius * 2 + 1;
	float logradius = log((float)radius);

	
	Point center = cv::Point(radius + 1, radius + 1);
		
	Mat out = Mat::zeros(Size(size, size), CV_8U);

	


	for(int i = divRad; i >= 0; i--)
	{
		int offset = 6;
		int radOut = exp(logradius * (i + offset)/(divRad + offset));
		cv::circle(out, center, radOut, i * divArg, -1, 4);
	}

	for(int y = 0; y < out.rows; y++){
		unsigned char* p = out.ptr<unsigned char>(y);
		for(int x = 0; x < out.cols; x++){
			float dx = center.x - x;
			float dy = center.y - y;

			float arg = atan2(dy, dx);
			int argindex = floor((arg + M_PI) * divArg / (M_PI * 2));

			if(argindex >= divArg)
			{
				argindex = divArg -1;
			}

			if(p[x] != 0)
			{
				p[x] = p[x] + argindex - divArg + 1;
			}

		}
	}

	return out;
}


Mat correlationLP(Mat src0, Point pos)
{
	int radius = 50;
	int divRad = 4;
	int divArg = 20;
	int size = radius * 2 + 1;


	Mat labelMat = labelLP(radius, divRad, divArg);

	int w = 5;
	int h = w;
	int margin = w/2;

	int W = size + margin * 2;
	int H = W;

	Mat hist = Mat::zeros(Size(divArg, divRad), CV_32F);
	Rect rect(pos.x - W/2, pos.y - H/2, W, H);
	if(!IsLegal(src0, rect))
	{
		return hist;
	}

	Size patchsize = Size(w, h);
	Size searchsize = Size(w, h);

	Mat src = src0(rect);
	Point center = pos - cv::Point(rect.x, rect.y);

	for(int y = margin; y < src.rows - margin; y++){
		for(int x = margin; x < src.cols - margin; x++){
			int label= labelMat.at<unsigned char>(y - margin, x - margin);
			if(label == 0)
			{
				continue;
			}

			label -= 1;
			int indexR = label/divArg;
			int indexArg = label%divArg;

			float score = mathcing(src, Size(w, h), center, Point(x, y));

			Point p = Point(indexArg, indexR);
			if(score > hist.at<float>(indexR, indexArg))
			{
				hist.at<float>(indexR, indexArg) = score;
			}
			
		}
	}

	cv::normalize(hist, hist, hist.cols * hist.rows / 2, 0, NORM_L1, CV_32F);
	

	return hist;

}


void toUchar(int *num)
{
	if(*num > 0xFF)
	{
		*num = 0xFF;
	}
	if(*num < 0)
	{
		*num = 0;
	}
}

void averageColor(Mat src, float radius, int* forecolor, int* backcolor)
{
	int dia = src.cols;

	float D = radius;

	float count1 = 0;
	float total1 = 0;

	float count2 = 0;
	float total2 = 0;

	int center = (dia - 1)/2;

	for (int y = 0; y < src.rows; y++)
	{
		for (int x = 0; x < src.cols; x++)
		{

			float d = (center - x) * (center - x) + (center - y) * (center - y);
			d = sqrt(d);

			if(D > d){
				float val = min(1.0f, D - d);

				if(val == 1.0)
				{
					total1 += val * src.at<unsigned char>(y, x);
					count1 += val;	
				}
			}
			else
			{
				float val = min(1.0f, d - D);
				if(val == 1.0)
				{
					total2 += val * src.at<unsigned char>(y, x);
					count2 += val;	
				}
			}

		}
	}

	if(count1 == 0)
	{
		*forecolor = 0x80;
	}
	else
	{
		*forecolor = total1/count1;
	}
	if(count1 == 0)
	{
		*backcolor = 0x80;
	}
	else
	{
		*backcolor = total2/count2;
	}

}

void findcircle(Mat src, Mat rad, Mat sep, int x, int y, int dist, float MINRAD, float MAXRAD, int offset = 0)
{
	int step = 1;
	int repeat = 7;

	float radiusmin = rad.at<float>(y, x) - (repeat/2) * step;
	radiusmin = max(MINRAD, radiusmin);


	float radiusmax =  radiusmin + step * (repeat - 1);
	radiusmax = min(MAXRAD, radiusmax);

	repeat = max(repeat, (int)((radiusmax - radiusmin)/step));

	int size = radiusmax * 2.5;

	Rect roi = Rect(x - size/2, y-size/2, size, size);

	if(roi.x < 0 || roi.y < 0 || roi.x + roi.width > src.cols || roi.y + roi.height > src.rows)
	{
		rad.at<float>(y, x) = 0;
		sep.at<float>(y, x) = 0;
		return;
	}

	Mat src2 = src(roi);
	Mat found;


	double minscore = -1;
	int minindex = 0;
	int mindx = 0;
	int mindy = 0;

	float radius = rad.at<float>(y, x);

	int forecolor;
	int backcolor;
	averageColor(src2, radius, &forecolor, &backcolor);

	int colorstep = (backcolor - forecolor)/ 12;

	if(abs(colorstep) < 5)
	{
		if(colorstep < 0)
		{
			colorstep = -5;
		}
		else
		{
			colorstep = 5;
		}
	}

	backcolor += colorstep * 3;
	forecolor -= colorstep * 3;

	toUchar(&forecolor);
	toUchar(&backcolor);

	for(int i = 0; i < repeat; i++)
	{
		for(int j = 0; j < 5; j++)
		{
			for(int k = 0; k < 6; k++)
			{
				for(int l = 0; l < 6; l++)
				{
					Mat temp = blurredcircle(size, backcolor + k * colorstep, forecolor - l * colorstep, radiusmin + i * step, 3 * j);
					for(int dx = -dist + offset; dx <= dist + offset; dx++)
					{
						for(int dy = -dist + offset; dy <= dist + offset; dy++)
						{
							if(abs(dx) + abs(dy) > dist)
							{
								continue;
							}

							Rect roi2 = roi;
							roi2.x += dx;
							roi2.y += dy;
							if(roi2.x < 0 || roi2.y < 0 || roi2.x + roi2.width > src.cols || roi2.y + roi2.height > src.rows)
							{
								continue;
							}
							Mat src3 = src(roi2);

							double score = matdistU(src3, temp);
							if(minscore > score || minscore == -1)
							{
								found = temp.clone();
								minscore = score;
								minindex = i;
								mindx = dx;
								mindy = dy;
							}
						}
					}
				}
			}
		}

	}

	sep.at<float>(y + mindy, x + mindx) = sep.at<float>(y, x);
	rad.at<float>(y + mindy, x + mindx) = radiusmin + minindex * step;

	if(mindy != 0 || mindx !=0)
	{
		sep.at<float>(y, x) = 0;
		rad.at<float>(y, x) = 0;
	}

}


void fixcircle(Mat &src, Mat &sep, Mat &rad, float thresh, int dist, float MINRAD, float MAXRAD, float mult)
{
	int offset = min(dist/2, (int)((mult + 1)/2));


	for (int y = 0; y < src.rows; y++) {
		for (int x = 0; x < src.cols ; x++) {
			float num = sep.at<float>(y, x);
			if(num >= thresh){
				findcircle(src, rad, sep, x, y, dist, MINRAD, MAXRAD, offset);
			}
			else
			{
				sep.at<float>(y, x) = 0;
				rad.at<float>(y, x) = 0;
			}
		}
	}

	return;
}

void deletion(Mat src, Mat rad, int x0, int y0)
{

	float val = src.at<float>(y0, x0);
	float radius = rad.at<float>(y0, x0) * 1.5;
	if(radius < 1)
	{
		return;
	}

	Mat element = matCircleU(radius, 0, false);

	Rect rec(x0 - element.cols/2, y0 - element.rows/2, element.cols, element.rows);
	if(!IsLegal(src, rec))
	{
		return;
	}
	Mat roi = src(rec);
	Mat radroi = rad(rec);

	for(int y=0; y<roi.rows; y++)
	{
		float *p = roi.ptr<float>(y);
		float *q = radroi.ptr<float>(y);
		unsigned char *r = element.ptr<unsigned char>(y);
		for(int x=0; x<roi.cols; x++)
		{		
			if(p[x] != 0 && p[x] <= val)
			{
				if(x != element.cols/2 || y != element.rows/2)
				{
					p[x] = 0;
					q[x] = 0;
				}
			}
		}
	}


}





void fixlocalpeak(Mat src, Mat rad, float thresh)
{
	for (int y = 0; y < src.rows; y++) {
		for (int x = 0; x < src.cols ; x++) {
			float val = src.at<float>(y, x);

			//if(val >= thresh){
			deletion(src, rad, x, y);
			//}
		}
	}

	return;
}


void regionsToData(vector<vector<Point>> regions, float threshC, float threshR, int *num, float **X, float **Y, float **A, float **L, float **C, float **R)
{
	int size = regions.size();

	*X = new float[size];	
	*Y = new float[size];	

	*A = new float[size];	
	*L = new float[size];
	*C = new float[size];	
	*R = new float[size];

	int k = 0;
	for( int i = 0; i< size; i++ )
	{
		float a = contourArea(regions[i]);
		float l = arcLength(Mat(regions[i]), true);
		float c = 4 * M_PI * a/ (l * l);
		float r = sqrt(a/M_PI);

		if(c >= threshC && r >= threshR)
		{
			Moments mu = moments( regions[i], false );
			Point2f mc = Point2f( mu.m10/mu.m00 , mu.m01/mu.m00 );

			(*A)[k] = a;
			(*L)[k] = l;
			(*C)[k] = c;
			(*R)[k] = r;
			(*X)[k] = mc.x;
			(*Y)[k] = mc.y;
			k++;
		}
	}

	*num = k;
	return;
}



void analyze(vector<Point> points, Mat *out, double threshC, double threshR, Mat *matA, Mat *matL, Mat *matC, Mat *matR)
{
	double A = contourArea(points);
	double L = arcLength(Mat(points), true);
	double C = 4 * M_PI * A/ (L * L);
	double R = sqrt(A/M_PI);

	Moments mu = moments( points, false );
	Point2f mc = Point2f( mu.m10/mu.m00 , mu.m01/mu.m00 );

	


	if(C >= threshC && R >= threshR)
	{
		
		if(matA != NULL)
			(*matA).at<float>(mc) = A;
		if(matL != NULL)
			(*matL).at<float>(mc) = L;
		if(matC != NULL)
			(*matC).at<float>(mc) = C;
		if(matR != NULL)
			(*matR).at<float>(mc) = R;

		if(out != NULL)
		{
			string text = intToString((int)(R * 100));
			//string text = intToString((int)A);
			int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
			double fontScale = 0.5;
			int thickness = 1;


			int baseline=0;
			Size textSize = getTextSize(text, fontFace,
				fontScale, thickness, &baseline);
			baseline += thickness;

			// テキストをセンタリングします．
			Point textOrg = points[0];

			// 続いてテキスト自体を描画します．
			putText(*out, text, textOrg, fontFace, fontScale,
				Scalar::all(0), thickness, 8);

			circle( *out, mc, 1,  Scalar(0, 0xFF, 0xFF), -1, CV_AA, 0 );
		}

	}


}


	


bool finddivide(vector<Point> points, vector<vector<Point>> *approxs, Mat *out)
{
	int arcthresh = 30;
	int areathresh = 170;

	double minarclength = 300;

	int num = points.size();
	if(num < 4)
	{
		return false;
	}

	double ** d = new double*[num];
	double *a = new double[num];


	for( int i=0; i<num; i++ ) {
		d[i] = new double[i];
		for( int j=0; j<i; j++ ) {
			d[i][j] = sqrt((double)dist(points[i], points[j]));
		}

	}

	a[0] = 0;
	for( int i=0; i<num - 1; i++ ) {
		a[i+1] = a[i] + d[i+1][i];
	}
	int test = a[1];
	int test2 = a[num - 1];

	double totallength = a[num - 1] + sqrt((double)dist(points[0], points[num-1]));



	double minratio = 1; 
	int minj = 0;
	int mini = 0;

	for( int i=0; i<num; i++ ) {
		for( int j=0; j<i - 1; j++ ) {
			double arclength = a[i] - a[j];
			double length = d[i][j];
			arclength = min(arclength, totallength - arclength);
			if(arclength < arcthresh)
			{
				continue;
			}
			arclength = min(arclength, minarclength);
			if(minratio > length/(arclength))
			{
				if(1 == pointPolygonTest((Mat)points, (Point2f)(points[i] + points[j])* 0.5, false))
				{

					if(false)
					{
						vector<Point> points1;
						for( int i2=0; i2<= j; i2++ )
						{
							points1.push_back(points[i2]);
						}
						for( int i2=i; i2<num; i2++ ) {
							points1.push_back(points[i2]);
						}
						vector<Point> points2;
						for( int i2=j; i2<=i; i2++ ) {
							points2.push_back(points[i2]);
						}

						double A1 = contourArea(points1);
						double A2 = contourArea(points2);

						if(A1 > areathresh && A2 > areathresh)
						{
							minratio = length/(arclength);
							mini = i;
							minj = j;
						}
					}
					else
					{
						minratio = length/(arclength);
						mini = i;
						minj = j;
					}
				}
			}

		}
	}

	bool found = false;
	if(minratio < 0.25)
	{
		found = true;

		if(out != NULL)
		line(*out, points[mini], points[minj], Scalar(0,0xFF,0), 2);

		vector<Point> points1;

		for( int i=0; i<= minj; i++ )
		{
			points1.push_back(points[i]);
		}
		for( int i=mini; i<num; i++ ) {
			points1.push_back(points[i]);
		}

		vector<Point> points2;
		for( int i=minj; i<=mini; i++ ) {
			points2.push_back(points[i]);
		}

		if(isContourConvex(Mat(points1)) == false)
		{
			finddivide( points1, approxs, out);
		}
		else
		{
			approxs->push_back(points1);
			//analyze(points1, out);
		}

		if(isContourConvex(Mat(points2)) == false)
		{
			finddivide( points2, approxs, out);
		}
		else
		{
			approxs->push_back(points2);
			//analyze(points2, out);
		}

	}
	else
	{
		approxs->push_back(points);
		//analyze(points, out);
	}

	for( int i=0; i<num; i++ ) {
		delete[] d[i];
	}
	delete[] d;
	delete a;

	return found;
}


void checkconvex(vector<Point> points, Mat out)
{
	int i;
	int orientation = 0;



	Point cur_pt;
	Point prev_pt;
	vector<Point>::iterator it = points.begin();  // イテレータのインスタンス化
	prev_pt = *it;
	it++; 				
	cur_pt = *it;
	it++;
	int dx0 = cur_pt.x - prev_pt.x;
	int dy0 = cur_pt.y - prev_pt.y;
	int dxdy0, dydx0;
	int dx, dy;

	while( it != points.end() )  // 末尾要素まで
	{		 				
		prev_pt = cur_pt;
		cur_pt = *it;

		dx = cur_pt.x - prev_pt.x;
		dy = cur_pt.y - prev_pt.y;
		dxdy0 = dx * dy0;
		dydx0 = dy * dx0;

		/* find orientation */
		/*orient = -dy0 * dx + dx0 * dy;
		orientation |= (orient > 0) ? 1 : 2;
		*/
		orientation |= (dydx0 > dxdy0) ? 1 : 2;

		if( orientation == 3 )
		{
			out.at<unsigned char>(prev_pt.y, prev_pt.x) = 0xFF;
			orientation = (dydx0 > dxdy0) ? 2 : 1;
		}
		else
		{
			out.at<unsigned char>(prev_pt.y, prev_pt.x) = 0x80;
		}

		dx0 = dx;
		dy0 = dy;

		it++;

	}




}


vector<vector<Point>> analyzer(Mat src, int threshColor, int threshCanny, bool divide, Mat *out)
{
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	Mat gray;
	cv::cvtColor(src, gray, CV_BGR2GRAY);

	float num = 1.0;
	cv::Mat element = matCircleU(2, 0);
	cv::Mat element2 = matCircleU((int)(num * 2), 0);


	cv::threshold(gray, gray, threshColor, 255, cv::THRESH_BINARY);
	Canny(gray, canny_output, threshCanny, threshCanny*2, 3, false);

	cv::dilate(canny_output,canny_output, element, cv::Point(-1,-1), 1);
	cv::erode(canny_output,canny_output, element, cv::Point(-1,-1), 1);

	findContours( canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );


	Mat drawing =  src.clone();
	Rect rect(0, 0, drawing.cols, drawing.rows);

	vector<vector<Point>> approxs;

	for( int i = 0; i< contours.size(); i++ )
	{

		double len = 1;
		vector<Point> approx;
		approxPolyDP(Mat(contours[i]), approx, len, true);

		//approx = contours[i];

		const Point* p = &approx[0];
		int n = (int)approx.size();
		if(n < 3)
		{
			continue;
		}

		/*if(divide == false)
		{
			approxs.push_back(approx);
			continue;
		}*/

		if(isContourConvex(Mat(approx)) || arcLength(Mat(approx), true) < len * 6 || divide == false)
		{
			approxs.push_back(approx);

			if(out != NULL)
			polylines(drawing, &p, &n, 1, true, Scalar(0xC0, 0xFF, 0), 1, CV_AA);
			
		}
		else
		{

			if(finddivide(approx, &approxs, &drawing))
			{
				if(out != NULL)
				{
					for( int i = 0; i < n; i++ )
					{

						circle( drawing, approx[i], 2,  Scalar(0, 0, 255), -1, CV_AA, 0 );
					}
					polylines(drawing, &p, &n, 1, true, Scalar(255, 0x40, 0), 1, CV_AA);
				}
			}
			else
			{
				if(out != NULL)
				polylines(drawing, &p, &n, 1, true, Scalar(0xC0, 0xFF, 0), 1, CV_AA);
			}



		}


	}

	if(out != NULL)
	*out = drawing;

	return approxs;
}