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

//int _tmain(int argc, _TCHAR* argv[])
//{
//	_tprintf(_T("Trying avcodec_register_all... "));
//	avcodec_register_all();
//	_tprintf(_T("Done.\n"));
//	return 0;
//}

int test()
{
	cv::VideoCapture cap(0);
	// 様々な設定...
	cv::Size cap_size(640, 480);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, cap_size.width);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, cap_size.height);
	// カメラがオープンできたかの確認
	if (!cap.isOpened()) return -1;

	// ビデオライタ
	int fps = 15;
	cv::VideoWriter writer("capture.avi", CV_FOURCC('X', 'V', 'I', 'D'), fps, cap_size);

	cv::namedWindow("Capture", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::Mat frame;
	while (1) {
		cap >> frame;  // キャプチャ
					   // 様々な処理
					   // ...
		writer << frame;
		cv::imshow("Capture", frame);
		if (cv::waitKey(30) >= 0) break;
	}
}

int main(int argc, char* argv[])
{


	using namespace cv;

	VideoCapture video;

	video.open("D:\\MUSIC\\ビデオ８\\britney spears - overprotected - lyrics.mp4");
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

	std::string wstr("C:/test2.avi");
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