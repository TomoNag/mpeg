
#pragma once
#include "Util.h"
#include <windows.h>
#include <process.h>

enum convtype {COLOR, FLOATTOCHAR, CHARTOFLOAT, LOCALPEAK, INVERT, CLONE, DIFFX, DIFFY, SOLVEPOISSON, POISSON_SUB1, POISSON_SUB2};
enum threshtype { TOMAX, THRESH , THRESH2, HUE_THRESH};
enum circletype {SEPARABILITY, LINESEPARABILITY, COLORDIFFERENCE, DENSITYIN, DENSITYOUT, DENSITYRING, LOCALPEAK2};


int ConvertFuncs(convtype FT, Mat src, Mat *dst, int param1 = 0, float fparam1 = 0, float fparam2 = 0, Mat *mask = NULL);
int CircleFuncs(circletype FT, Mat *src0, Mat *dst, Mat *mask, float radius, float radiusin, float radiusout, float fparam1 = 0, Mat *dstRad = NULL, Mat temps[] = NULL, int repeat = 1, float step = 1);
int CircleFuncsSub(circletype FT, Mat &src, Mat *dst, float radius, float radiusin, float radiusout, Mat *mask = NULL, float fparam1 = 0, Mat *sq = NULL);

void localPeak(Mat src0, Mat *dst, Mat *msk0, float radius, float thresh = 0);
int lineseparability(Mat src, Mat *dst, float radius, float theta360, Mat *mask = NULL, Mat *sq = NULL);
int lineaverage(Mat src, Mat *dst, float radius, float theta360, Mat *mask = NULL);
int separability(Mat src, Mat *dst, float radius, float radiusin, float radiusout, Mat *mask = NULL, float fparam = 0, Mat *sq = NULL);

template <class T>
T Funcs(threshtype FT, T src, T max, T min, T param1 = 0, T param2 = 0, float fparam1 = 0);

template <class T> int ThreshFuncs(threshtype FT, Mat src, Mat srcMax, Mat srcMin, Mat dst, T param1 = 0, T param2 = 0, float fparam1 = 0);
template <class T> int ThreshFuncs(threshtype FT, Mat src, T max, Mat srcMin, Mat dst, T param1 = 0, T param2 = 0, float fparam1 = 0);
template <class T> int ThreshFuncs(threshtype FT, Mat src, Mat srcMax, T min, Mat dst, T param1 = 0, T param2 = 0, float fparam1 = 0);
template <class T> int ThreshFuncs(threshtype FT, Mat src, T max, T min, Mat dst, T param1 = 0, T param2 = 0, float fparam1 = 0);


template <class T> int ThreshFuncs(threshtype FT, Mat *src, Mat *srcMax, Mat *srcMin, T max, T min, Mat *dst, T param1, T param2, float fparam1);
