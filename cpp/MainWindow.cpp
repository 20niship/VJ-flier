#include "MainWindow.h"


/*  ///////////////////////////////////////////////////////
//           �E�B���h�E�֘A�̏����pclass                 //
//                                                       //
/////////////////////////////////////////////////////////*/

static LRESULT CALLBACK WndProc2(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK WndProc2(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	static HDC hdc;
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}


void MainWindow::updateWindow(cv::Mat frame) {
	if (!window_exist) return;
	Cnv1.Convert(bhdc, frame);
	hdc = GetDC(_hWnd);
	char szStr[] = "test";

	BitBlt(hdc, 0, 0, win_width, win_height, bhdc, 0, 0, SRCCOPY);

	SetBkColor(hdc, RGB(0, 0, 0));  //�����̔w�i�F��ݒ�
	SetTextColor(hdc, RGB(255, 255, 255));  //�����̐F��ݒ�
	TextOut(hdc, 10, 10, (LPCTSTR)szStr, (int)strlen(szStr));
	ReleaseDC(_hWnd, hdc);
}


bool MainWindow::CreateMainWindow(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow, HWND hWndParnt) {
	ATOM atom;

	_hCurInst = hCurInst;
	_hPrevInst = hPrevInst;
	_lpsCmdLine = lpsCmdLine;
	_nCmdShow = nCmdShow;

	_wc.cbSize = sizeof(WNDCLASSEX);
	_wc.style = CS_HREDRAW | CS_VREDRAW;
	_wc.lpfnWndProc = WndProc2;	//�v���V�[�W����
	_wc.cbClsExtra = 0;
	_wc.cbWndExtra = 0;
	_wc.hInstance = hCurInst;//�C���X�^���X
	_wc.hIcon = NULL;
	_wc.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
	_wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	_wc.lpszMenuName = NULL;	 // ���j���[��
	_wc.lpszClassName = (LPCSTR)"screen_app";
	_wc.hIconSm = NULL;

	if ((atom = RegisterClassEx(&_wc)) == 0) {
		MessageBox(NULL, "Failed to Create Window", "ERROR1-1", MB_OK);
		return false;
	}

	_hWnd = CreateWindowEx(NULL, MAKEINTATOM(atom), "screen",
		WS_OVERLAPPEDWINDOW, //�E�B���h�E�̎��
		posX, posY, win_width, win_height,
		hWndParnt, //�e�E�B���h�E�̃n���h���A�e�����Ƃ���NULL
		NULL, //���j���[�n���h���A�N���X���j���[���g���Ƃ���NULL
		hCurInst,
		this);


	if (!_hWnd) {
		MessageBox(NULL, "Failed to Create Window", "ERROR1-2", MB_OK);
		return false;
	}

	DEVMODE devMode;

	devMode.dmSize = sizeof(DEVMODE);
	devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
	devMode.dmPelsWidth = 1920;
	devMode.dmPelsHeight = 1080;

	ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);

	//SetMenu(_hWnd, NULL);	//���j���[���B��
	//SetWindowLong(_hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);//�E�B���h�E�̃X�^�C����ύX
	//MoveWindow(_hWnd, GetSystemMetrics(SM_XVIRTUALSCREEN),
	//	GetSystemMetrics(SM_YVIRTUALSCREEN),
	//	GetSystemMetrics(SM_CXVIRTUALSCREEN),
	//	GetSystemMetrics(SM_CYVIRTUALSCREEN), TRUE);


	ShowWindow(_hWnd, _nCmdShow);
	UpdateWindow(_hWnd);

	Cnv1.hinfo = { sizeof(BITMAPINFOHEADER), win_width, win_height, 1, 24, BI_RGB,0, 0, 0, 0, 0,{ NULL,NULL,NULL } };
	bhdc = Cnv1.DoubleBuffer_Create(_hWnd, win_width, win_height);

	window_exist = true;
	return true;
}


void MainWindow::setWindowPos(int pos_X, int pos_Y) {
	posX = pos_X;
	posY = pos_Y;
	//if (window_exist) { cv::moveWindow(MAIN_WINDOW_NAME, posX, posY); }
}

void MainWindow::ExitSystem() {
	DeleteDC(bhdc);
	//PostQuitMessage(0);
	DestroyWindow(_hWnd);
	window_exist = false;
}


/*
void MainWindow::updateWindow(cv::Mat frame) {
	if (!window_exist) return;

	//int key = cv::waitKey(1); // �\���̂��߂�1ms�҂�
	//if (key == 27) {
	//	// esc or enter�L�[�ŏI��
	//	window_exist = false;
	//	ExitSystem();
	//}
}


void MainWindow::CreateMainWindow() {
	cv::namedWindow(MAIN_WINDOW_NAME, cv::WINDOW_NORMAL);
	cv::moveWindow(MAIN_WINDOW_NAME, 1925, 0);
	cv::setWindowProperty(MAIN_WINDOW_NAME, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
	window_exist = true;
}


void MainWindow::setWindowPos(int posX, int posY) {
	pos_x = posX;
	pos_y = posY;
	if (window_exist) { cv::moveWindow(MAIN_WINDOW_NAME, posX, posY); }
}

void MainWindow::ExitSystem() {
	cv::destroyWindow(MAIN_WINDOW_NAME);
	cv::destroyAllWindows();
	window_exist = false;
}

*/