
#pragma once
#include "opencv/cv.h"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/video/tracking.hpp>
#include <iostream>
//#include "labeling.h"

#define _USE_MATH_DEFINES
#define NOMINMAX

#include <math.h>
#include <algorithm> 

 #ifdef _DEBUG
    //Debugモードの場合
	#pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_ml246d.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_core246d.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_imgproc246d.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_highgui246d.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_objdetect246d.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_contrib246d.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_features2d246d.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_flann246d.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_gpu246d.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_haartraining_engined.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_legacy246d.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_ts246d.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_video246d.lib")

#pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_photo246d.lib")

#pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_calib3d246d.lib")
#pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_nonfree246d.lib")
#pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_stitching246d.lib")
#pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_superres246d.lib")
#pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_videostab246d.lib")

//#pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_ffmpeg246d.lib")

#else
    //Releaseモードの場合
	#pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_ml246.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_core246.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_imgproc246.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_highgui246.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_objdetect246.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_contrib246.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_features2d246.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_flann246.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_gpu246.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_haartraining_engined.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_legacy246.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_ts246.lib")
    #pragma comment(lib,"D:\\opencv\\build\\x86\\vc10\\lib\\opencv_video246.lib")
#endif

using namespace std;
using namespace cv;

