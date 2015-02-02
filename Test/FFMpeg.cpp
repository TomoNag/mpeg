// FFMpeg + OpenCV demo  
#include <stdio.h>  
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  

#ifdef __cplusplus  
extern "C" {
#endif  

#include "dev/include/libavformat/avformat.h"  
#include "dev/include/libavcodec/avcodec.h"  
#include "dev/include/libswscale/swscale.h"
#ifdef __cplusplus  
}
#endif  




#pragma comment(lib, "dev\\lib\\avcodec.lib")  
#pragma comment(lib, "dev\\lib\\avformat.lib")  
#pragma comment(lib, "dev\\lib\\avutil.lib")    
#pragma comment(lib ,"dev\\lib\\swscale.lib")  
#pragma comment(lib ,"dev\\lib\\swresample.lib")  

static void CopyDate(AVFrame *pFrame, int width, int height, int time);
static void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame);

int ffmpegmain(int argc, const char * argv[])
{
	AVFormatContext *pFormatCtx = NULL;
	int             i, videoStream;
	AVCodecContext  *pCodecCtx;
	AVCodec         *pCodec;
	AVFrame         *pFrame;
	AVFrame         *pFrameRGB;
	AVPacket        packet;
	int             frameFinished;
	int             numBytes;
	uint8_t         *buffer;

	// Register all formats and codecs  
	av_register_all();

	// Open video file  
	if (avformat_open_input(&pFormatCtx, "D:\\MUSIC\\ビデオ８\\britney spears - overprotected - lyrics.mp4", NULL, NULL) != 0)
		// if(avformat_open_input(NULL, argv[1], NULL, NULL)!=0)  
		return -1; // Couldn't open file  

				   // Retrieve stream information  
	if (avformat_find_stream_info(pFormatCtx, NULL)<0)
		return -1; // Couldn't find stream information  

				   // Dump information about file onto standard error  
	av_dump_format(pFormatCtx, 0, "D:\\MUSIC\\ビデオ８\\britney spears - overprotected - lyrics.mp4", false);

	// Find the first video stream  
	videoStream = -1;
	for (i = 0; i<pFormatCtx->nb_streams; i++)
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoStream = i;
			break;
		}
	if (videoStream == -1)
		return -1; // Didn't find a video stream  

				   // Get a pointer to the codec context for the video stream  
	pCodecCtx = pFormatCtx->streams[videoStream]->codec;

	// Find the decoder for the video stream  
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL)
		return -1; // Codec not found  

				   // Open codec  
	if (avcodec_open2(pCodecCtx, pCodec, 0)<0)
		return -1; // Could not open codec  

				   // Hack to correct wrong frame rates that seem to be generated by some codecs  
	if (pCodecCtx->time_base.num>1000 && pCodecCtx->time_base.den == 1)
		pCodecCtx->time_base.den = 1000;

	// Allocate video frame  
	pFrame = av_frame_alloc();

	// Allocate an AVFrame structure  
	pFrameRGB = av_frame_alloc();
	if (pFrameRGB == NULL)
		return -1;

	// Determine required buffer size and allocate buffer  
	numBytes = avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width,
		pCodecCtx->height);

	//buffer=malloc(numBytes);  
	buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

	// Assign appropriate parts of buffer to image planes in pFrameRGB  
	avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24,
		pCodecCtx->width, pCodecCtx->height);

	// Read frames and save first five frames to disk  
	i = 0;
	long prepts = 0;
	while (av_read_frame(pFormatCtx, &packet) >= 0)
	{
		// Is this a packet from the video stream?  
		if (packet.stream_index == videoStream)
		{
			// Decode video frame  
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

			// Did we get a video frame?  
			if (frameFinished)
			{
				static struct SwsContext *img_convert_ctx;

#if 0  
				// Older removed code  
				// Convert the image from its native format to RGB swscale  
				img_convert((AVPicture *)pFrameRGB, PIX_FMT_RGB24,
					(AVPicture*)pFrame, pCodecCtx->pix_fmt, pCodecCtx->width,
					pCodecCtx->height);

				// function template, for reference  
				int sws_scale(struct SwsContext *context, uint8_t* src[], int srcStride[], int srcSliceY,
					int srcSliceH, uint8_t* dst[], int dstStride[]);
#endif  
				// Convert the image into YUV format that SDL uses  
				if (img_convert_ctx == NULL) {
					int w = pCodecCtx->width;
					int h = pCodecCtx->height;

					img_convert_ctx = sws_getContext(w, h,
						pCodecCtx->pix_fmt,
						w, h, PIX_FMT_RGB24, SWS_BICUBIC,
						NULL, NULL, NULL);
					if (img_convert_ctx == NULL) {
						fprintf(stderr, "Cannot initialize the conversion context!\n");
						exit(1);
					}
				}
				int ret = sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0,
					pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
#if 0   
				// this use to be true, as of 1/2009, but apparently it is no longer true in 3/2009  
				if (ret) {
					fprintf(stderr, "SWS_Scale failed [%d]!\n", ret);
					exit(-1);
				}
#endif  
				// Save the frame to disk  
				if (i++ <= 5)
					SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, i);

				CopyDate(pFrameRGB, pCodecCtx->width, pCodecCtx->height, packet.pts - prepts);
				prepts = packet.pts;
			}
		}

		// Free the packet that was allocated by av_read_frame  
		av_free_packet(&packet);
	}

	// Free the RGB image  
	//free(buffer);  
	av_free(buffer);
	av_free(pFrameRGB);

	// Free the YUV frame  
	av_free(pFrame);

	// Close the codec  
	avcodec_close(pCodecCtx);

	// Close the video file  
	avformat_close_input(&pFormatCtx);

	system("Pause");

	return 0;
}

static void CopyDate(AVFrame *pFrame, int width, int height, int time)
{
	if (time <= 0) time = 1;

	int     nChannels;
	int     stepWidth;
	uchar*  pData;
	cv::Mat frameImage(cv::Size(width, height), CV_8UC3, cv::Scalar(0));
	stepWidth = frameImage.step;
	nChannels = frameImage.channels();
	pData = frameImage.data;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			pData[i*stepWidth + j*nChannels + 0] = pFrame->data[0][i*pFrame->linesize[0] + j*nChannels + 2];
			pData[i*stepWidth + j*nChannels + 1] = pFrame->data[0][i*pFrame->linesize[0] + j*nChannels + 1];
			pData[i*stepWidth + j*nChannels + 2] = pFrame->data[0][i*pFrame->linesize[0] + j*nChannels + 0];
		}
	}

	cv::namedWindow("Video", cv::WINDOW_NORMAL);
	cv::imshow("Video", frameImage);
	cv::waitKey(time);
}

static void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame)
{
	FILE *pFile;
	char szFilename[32];
	int  y;

	// Open file  
	sprintf(szFilename, "frame%d.ppm", iFrame);
	pFile = fopen(szFilename, "wb");
	if (pFile == NULL)
		return;

	// Write header  
	fprintf(pFile, "P6\n%d %d\n255\n", width, height);

	// Write pixel data  
	for (y = 0; y<height; y++)
		fwrite(pFrame->data[0] + y*pFrame->linesize[0], 1, width * 3, pFile);

	// Close file  
	fclose(pFile);
}