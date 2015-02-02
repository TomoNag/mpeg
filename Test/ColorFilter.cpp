#include "OPENCV.h"

/// Global variables
Mat src, src2, img, image;

int elem1 = 255;
int elem2 = 255;
int elem3 = 255;

int const max_elem = 500;


/** Function Headers */

void Hue(int, void*);
void Saturation(int, void*);
void Value(int, void*);
void Processing();

double alpha; /**< Simple contrast control */
int beta;  /**< Simple brightness control */


/** @function main */
int filtermain(int argc, char** argv)
{
	/// Load an image
	namedWindow("image");

	createTrackbar("Hue", "image", &elem1, max_elem, Hue);
	createTrackbar("Saturation", "image", &elem2, max_elem, Saturation);
	createTrackbar("Value", "image", &elem3, max_elem, Value);

	src = imread("C:\\Users\\a\\Pictures\\h_large_3BJs_27dd00062c631986.jpg");
	src2 = Mat::zeros(src.rows, src.cols, CV_8UC3);

	Processing();
	waitKey(0);
	return 0;
}

void Hue(int, void *)
{
	Processing();
}

void Saturation(int, void *)
{
	Processing();
}

void Value(int, void *)
{
	Processing();
}


void Processing()
{
	alpha = 0.5;
	beta = 0x80;

	
	


	for (int y = 0; y < src.rows; y++)
	{
		for (int x = 0; x < src.cols; x++)
		{
			for (int c = 0; c < 3; c++)
			{
				src2.at<Vec3b>(y, x)[c] =
					saturate_cast<uchar>(alpha*(src.at<Vec3b>(y, x)[c]) + beta);
			}
		}
	}

	cvtColor(src2, img, CV_RGB2HSV);

	int hue = elem1 - 255;
	int saturation = elem2 - 255;
	int value = elem3 - 255;

	
	/// Do the operation new_image(i,j) = alpha*image(i,j) + beta
	

	for (int y = 0; y<img.cols; y++)
	{
		for (int x = 0; x<img.rows; x++)
		{
			int cur1 = img.at<Vec3b>(Point(y, x))[0];
			int cur2 = img.at<Vec3b>(Point(y, x))[1];
			int cur3 = img.at<Vec3b>(Point(y, x))[2];
			cur1 += hue;
			cur2 += saturation;
			cur3 += value;

			if (cur1 < 0) cur1 = 0; else if (cur1 > 255) cur1 = 255;
			if (cur2 < 0) cur2 = 0; else if (cur2 > 255) cur2 = 255;
			if (cur3 < 0) cur3 = 0; else if (cur3 > 255) cur3 = 255;

			img.at<Vec3b>(Point(y, x))[0] = cur1;
			img.at<Vec3b>(Point(y, x))[1] = cur2;
			img.at<Vec3b>(Point(y, x))[2] = cur3;

		
		}
	}

	cvtColor(img, image, CV_HSV2RGB);
	imshow("image", image);

}