#pragma once

#include <windows.h>
# include <opencv2/opencv.hpp>
# include <opencv2/highgui.hpp>
# include <opencv2/videoio.hpp>
# include <stdio.h>
# include <iostream>

class cvtMat2HDC {
private:
	HBITMAP hbmp;
	HDC hdc2;
public:
	BITMAPINFO hinfo;
	bool Convert(HDC, const cv::Mat);
	HDC DoubleBuffer_Create(HWND, int, int);
};


