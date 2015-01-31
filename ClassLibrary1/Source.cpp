#include "stdafx.h"



//#include <boost/cast.hpp>
#pragma warning( push )
#pragma warning( disable : 4996 )
#include <opencv2/opencv.hpp>
#pragma warning( pop )
#include "OPENCV\AviWriter.h"

extern "C"
{
#include "dev\include\libavcodec\avcodec.h"
}

#pragma comment(lib, "dev\\lib\\avcodec.lib")

#include "ClassLibrary1.h"

const char* toChar(String^ str)
{
	using namespace System::Runtime::InteropServices;
	const char* str2 = (const char*)(Marshal::StringToHGlobalAnsi(str)).ToPointer();
	return str2;

}


int myvideo(const char *input, const char *output)
{
	ofstream myfile;
	myfile.open("example.txt");
	myfile << "Writing this to a file.\n";
	myfile << input;
	myfile << "\n";
	myfile << output;
	myfile.close();


	using namespace cv;

	VideoCapture video;

	const char * input2 = "D:\\MUSIC\\ƒrƒfƒI‚W\\britney spears -overprotected - lyrics.mp4";
	const char * output2 = "C:\\test3.avi";

	video.open("");


	//video.open("C:\\test.avi");

	if (!video.isOpened()) {
		printf("Error: Couldn't open the video file.\n");

		std::cout << "press any key to exit...";
		return 0;
	}

	namedWindow("video");
	double fps = video.get(CV_CAP_PROP_FPS);
	int width = static_cast<int>(video.get(CV_CAP_PROP_FRAME_WIDTH));
	int height = static_cast<int>(video.get(CV_CAP_PROP_FRAME_HEIGHT));


	//int width = boost::numeric_cast<int, double>(video.get(CV_CAP_PROP_FRAME_WIDTH));
	//int height = boost::numeric_cast<int, double>(video.get(CV_CAP_PROP_FRAME_HEIGHT));

	CAviWriter video_out;
	int fourcc = CV_FOURCC('X', '2', '6', '4');

	std::string wstr(output2);
	if (!video_out.open(wstr, fourcc, fps, height, width)) {
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
		waitKey(1000 / fps);
		video_out << frame;
	}
	video.release();

	std::cout << "press any key to exit...";
	return 0;
}


int OpenCVdll::Class1::video(String^ input, String^ output)
{
	return myvideo(toChar(input), toChar(output));

}