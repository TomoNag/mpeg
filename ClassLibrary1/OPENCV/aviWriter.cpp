#include "stdafx.h"
#include "aviWriter.h"
//#include <vfw.h>
#include <string>
#include <iostream>

static void icvInitCapture_VFW()
{
	static int isInitialized = 0;
	if (!isInitialized)
	{
		AVIFileInit();
		isInitialized = 1;
	}
}
// philipg.  Made this code capable of writing 8bpp gray scale bitmaps
struct BITMAPINFO_8Bit
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD          bmiColors[256];
};

static BITMAPINFOHEADER icvBitmapHeader(int width, int height, int bpp, int compression = BI_RGB)
{
	BITMAPINFOHEADER bmih;
	memset(&bmih, 0, sizeof(bmih));
	bmih.biSize = sizeof(bmih);
	bmih.biWidth = width;
	bmih.biHeight = height;
	bmih.biBitCount = (WORD)bpp;
	bmih.biCompression = compression;
	bmih.biPlanes = 1;

	return bmih;
}

std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}



bool CAviWriter::open(const std::string &filename, int _fourcc, double _fps, int height, int width, int isColor)
{
	close();

	std::wstring stemp = s2ws(filename);
	LPCWSTR result = stemp.c_str();

	icvInitCapture_VFW();
	if (AVIFileOpen(&avifile, result, OF_CREATE | OF_WRITE, 0) == AVIERR_OK)
	{
		fourcc = _fourcc;
		fps = _fps;
		if (width > 0 && height > 0 &&
			!createStreams(height, width, isColor))
		{
			close();
			return false;
		}
	}
	return true;
}

void CAviWriter::close()
{
	if (uncompressed)
		AVIStreamRelease(uncompressed);
	if (compressed)
		AVIStreamRelease(compressed);
	if (avifile)
		AVIFileRelease(avifile);
	cvReleaseImage(&tempFrame);
	init();
}

void CAviWriter::init()
{
	avifile = 0;
	compressed = uncompressed = 0;
	fps = 0;
	tempFrame = 0;
	pos = 0;
	fourcc = 0;
}

bool CAviWriter::createStreams(int height, int width, bool isColor)
{
	if (!avifile)
		return false;
	AVISTREAMINFO aviinfo;

	BITMAPINFO_8Bit bmih;
	bmih.bmiHeader = icvBitmapHeader(width, height, isColor ? 24 : 8);
	for (int i = 0; i < 256; i++)
	{
		bmih.bmiColors[i].rgbBlue = (BYTE)i;
		bmih.bmiColors[i].rgbGreen = (BYTE)i;
		bmih.bmiColors[i].rgbRed = (BYTE)i;
		bmih.bmiColors[i].rgbReserved = 0;
	}

	memset(&aviinfo, 0, sizeof(aviinfo));
	aviinfo.fccType = streamtypeVIDEO;
	aviinfo.fccHandler = 0;
	// use highest possible accuracy for dwRate/dwScale
	aviinfo.dwScale = (DWORD)((double)0x7FFFFFFF / fps);
	aviinfo.dwRate = cvRound(fps * aviinfo.dwScale);
	aviinfo.rcFrame.top = aviinfo.rcFrame.left = 0;
	aviinfo.rcFrame.right = width;
	aviinfo.rcFrame.bottom = height;

	if (AVIFileCreateStream(avifile, &uncompressed, &aviinfo) == AVIERR_OK)
	{
		AVICOMPRESSOPTIONS copts, *pcopts = &copts;
		copts.fccType = streamtypeVIDEO;
		copts.fccHandler = fourcc != -1 ? fourcc : 0;
		copts.dwKeyFrameEvery = 1;
		copts.dwQuality = 10000;
		copts.dwBytesPerSecond = 0;
		copts.dwFlags = AVICOMPRESSF_VALID;
		copts.lpFormat = &bmih;
		copts.cbFormat = (isColor ? sizeof(BITMAPINFOHEADER) : sizeof(bmih));
		copts.lpParms = 0;
		copts.cbParms = 0;
		copts.dwInterleaveEvery = 0;

		if (fourcc != -1 || AVISaveOptions(0, 0, 1, &uncompressed, &pcopts) == TRUE)
		{
			if (AVIMakeCompressedStream(&compressed, uncompressed, pcopts, 0) == AVIERR_OK &&
				AVIStreamSetFormat(compressed, 0, &bmih, sizeof(bmih)) == AVIERR_OK)
			{
				fps = fps;
				fourcc = (int)copts.fccHandler;
				//frameSize = frameSize;
				tempFrame = cvCreateImage(cv::Size(width, height), 8, (isColor ? 3 : 1));
				//tempFrame = cv::Mat(height, width, (isColor ? CV_8UC3 : CV_8UC1));
				return true;
			}
		}
	}
	return false;
}

CV_INLINE int cvAlign(int size, int align)
{
	assert((align & (align - 1)) == 0 && size < INT_MAX);
	return (size + align - 1) & -align;
}


bool CAviWriter::writeFrame(const IplImage* image)
{
	bool result = false;
	//CV_FUNCNAME( "CvVideoWriter_VFW::writeFrame" );

	//__BEGIN__;

	if (!image)
		return false;

	if (!compressed && !createStreams(image->height, image->width, image->nChannels > 1))
		return false;

	if (image->width != tempFrame->width || image->height != tempFrame->height) {
		std::cout << "image size is different from the currently set frame size" << std::endl;
	}

	if (image->nChannels != tempFrame->nChannels ||
		image->depth != tempFrame->depth ||
		image->origin == 0 ||
		image->widthStep != cvAlign(image->width*image->nChannels*((image->depth & 255) / 8), 4))
	{
		cvConvertImage(image, tempFrame, image->origin == 0 ? CV_CVTIMG_FLIP : 0);
		image = (const IplImage*)tempFrame;
	}

	result = AVIStreamWrite(compressed, pos++, 1, image->imageData,
		image->imageSize, AVIIF_KEYFRAME, 0, 0) == AVIERR_OK;

	//__END__;

	return result;
}

CAviWriter& CAviWriter::operator<<(const cv::Mat& image)
{
	IplImage _img = image;
	writeFrame(&_img);
	return *this;
}