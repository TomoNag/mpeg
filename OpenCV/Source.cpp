
#include "stdafx.h"

#include <string>
#include <iostream>
#include <fstream>
//#include <boost/cast.hpp>
#pragma warning( push )
#pragma warning( disable : 4996 )
#include <opencv2/opencv.hpp>
#pragma warning( pop )
#include "AviWriter.h"

extern "C"
{
#include "dev\include\libavcodec\avcodec.h"
}

#pragma comment(lib, "dev\\lib\\avcodec.lib")

#include "OpenCV.h"


#include <msclr\marshal_cppstd.h>

std::string toChar(System::String^ str)
{
	msclr::interop::marshal_context context;
	std::string standardString = context.marshal_as<std::string>(str);

	return standardString;
	//using namespace System::Runtime::InteropServices;
	//const char* str2 = (const char*)(Marshal::StringToHGlobalAnsi(str)).ToPointer();
	//return str2;

}



void Process();

double alpha; /**< Simple contrast control */
int beta;  /**< Simple brightness control */




void Process(Mat src)
{
	Mat src2, img, image;

	alpha = 0.5;
	beta = 0x80;

	src2 = Mat::zeros(src.rows, src.cols, CV_8UC3);
	
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

	int hue = 0;
	int saturation = 0;
	int value = 0;


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

int OpenCV::VideoEditor::test(System::String^ input, System::String^ output)
{

	
	using namespace cv;

	VideoCapture video;
	
	video.open(toChar(input));
	
	if (!video.isOpened()) {
		printf("Error: Couldn't open the video file.\n");

		std::cout << "press any key to exit...";
		return 0;
	}

	namedWindow("video");
	double fps = video.get(CV_CAP_PROP_FPS);
	int width = static_cast<int>(video.get(CV_CAP_PROP_FRAME_WIDTH));
	int height = static_cast<int>(video.get(CV_CAP_PROP_FRAME_HEIGHT));

	CAviWriter video_out;
	//int fourcc =  CV_FOURCC('X', '2', '6', '4');
	int fourcc = -1;
	//int fourcc = CV_FOURCC('X', '2', '6', '4');
	
	//std::string wstr("C:/test2.avi");
	if (!video_out.open(toChar(output), fourcc, fps, height, width)) {
		printf("Error: Couldn't open the output video file.\n");

		std::cout << "press any key to exit...";
		return 0;
	}

	Mat frame;
	while (true) {
		video >> frame;
		if (frame.empty())
			break;
		imshow("video", frame);

		Process(frame);

		waitKey(1);
		//waitKey(1000 / fps);
		video_out << frame;
	}
	video.release();

	std::cout << "press any key to exit...";
	return 0;
}