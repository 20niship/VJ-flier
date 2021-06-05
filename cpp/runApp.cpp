#include <Windows.h>
#include <windowsx.h>
# include <stdio.h>
# include <iostream>
#include <io.h>
#include <fcntl.h>
#include <cstdio>

# include <opencv2/opencv.hpp>
# include <opencv2/highgui.hpp>
# include <opencv2/videoio.hpp>

#include "resource.h"
#include "SubWindow.h"

bool isDual = false;


int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst,
	LPSTR lpsCmdLine, int nCmdShow) {

	SubWindow Subwin;
	
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);

	std::cout << "Starting Process......";

	Subwin.setWindowSize(1100,640);
	Subwin.setWindowPos(30, 30);
	Subwin.setWindowName("SUB WINDOW");
	bool success = Subwin.setup(hCurInst, hPrevInst, lpsCmdLine, nCmdShow);

	if (!success) {
		std::cout << "[ ERROR 1-3 ] -> can't create sub window";
		std::cout << "[ ERROR 1-3 ] -> finished main process.....";
		MessageBox(NULL, "ERROR", "ERROR", MB_OK);
	}else{
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	PlaySound(NULL, NULL, 0);

	//std::cout << "finishing main process.....";
	Subwin.ExitSubWindow();
}


