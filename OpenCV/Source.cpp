
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
		waitKey(1);
		//waitKey(1000 / fps);
		video_out << frame;
	}
	video.release();

	std::cout << "press any key to exit...";
	return 0;
}