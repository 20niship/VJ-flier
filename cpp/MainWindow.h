#pragma once

#include <windows.h>
#include <cmath>
#include <opencv2/opencv.hpp>
#pragma comment(lib,"Winmm.lib")

#include "mat_to_bmp.h"

#define MAIN_WINDOW_NAME "screen"

class MainWindow {
public:
	void updateWindow(cv::Mat);
	bool CreateMainWindow(HINSTANCE, HINSTANCE, LPSTR, int, HWND);
	void setWindowPos(int, int);
	void ExitSystem();
	bool get_enabled() { return window_exist; }

private:
	cv::Mat frame;
	bool window_exist = false;
	int pos_x = 10;
	int pos_y = 10;
	int width = 100;
	int height = 100;

	char szStr[50];
	cvtMat2HDC Cnv1;
	HDC hdc;
	HINSTANCE _hCurInst;
	HINSTANCE _hPrevInst;
	LPSTR _lpsCmdLine;
	int _nCmdShow;
	
	int win_width = 1920;
	int win_height = 1080;

	int posX = 1990;
	int posY = 0;


	WNDCLASSEX _wc;
	HWND _hWnd;

	HDC bhdc;
};