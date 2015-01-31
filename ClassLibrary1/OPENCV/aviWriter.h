#pragma  comment(lib,  "vfw32") 

#pragma once
#include <string>
#include <opencv2/opencv.hpp>
#include <tchar.h>
#include <windows.h>
#include <vfw.h>

//#include <vfw.h>

class CAviWriter {
public:
	CAviWriter() { init(); }
	virtual ~CAviWriter() { close(); }
	bool open(const std::string &filename, int fourcc,
		double fps, int height, int width, int isColor = true);
	void close();

	virtual bool writeFrame(const IplImage*);
	virtual CAviWriter& operator << (const cv::Mat& image);
private:
	void init();
	bool createStreams(int height, int width, bool isColor);

	PAVIFILE      avifile;
	PAVISTREAM    compressed;
	PAVISTREAM    uncompressed;
	double        fps;
	IplImage*     tempFrame;
	long          pos;
	int           fourcc;
};