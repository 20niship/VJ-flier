#include "mat_to_bmp.h"

bool cvtMat2HDC::Convert(HDC hdc, const cv::Mat image) {
	//https://gist.github.com/AhiyaHiya/6e455a3a6c846766f1017044131bfab7
	//によると、画像の高さが4の倍数でなければならない。
	//4の倍数でないと、画像が45度ゆがんだ形で表示される。
	//尚、ソースコードは　http://chichimotsu.hateblo.jp/entry/20121130/1354265478

	cv::Mat _image;
	cv::flip(image, _image, 0);	// 上下反転

	const int x = _image.cols;
	const int y = _image.rows;
	hinfo.bmiHeader.biWidth = x;
	hinfo.bmiHeader.biHeight = y;
	hinfo.bmiHeader.biBitCount = _image.channels() * 8;

	hbmp = CreateCompatibleBitmap(hdc, x, y);
	SetDIBits(hdc, hbmp, 0, y, _image.data, &hinfo, DIB_RGB_COLORS);
	hdc2 = CreateCompatibleDC(hdc);
	SelectObject(hdc2, hbmp);
	BitBlt(hdc, 0, 0, x, y, hdc2, 0, 0, SRCCOPY);
	DeleteDC(hdc2);
	DeleteObject(hbmp);

	return true;
};



HDC cvtMat2HDC::DoubleBuffer_Create(HWND hwnd, int xSize, int ySize)	// 引数：(画面のハンドル,裏画面のX軸サイズ,裏画面のY軸サイズ)
{
	HDC hdc;
	static HDC bhdc;
	static HBITMAP hBackBMP;
	hdc = GetDC(hwnd);
	bhdc = CreateCompatibleDC(hdc);
	hBackBMP = CreateCompatibleBitmap(hdc, xSize, ySize);	// 裏画面をBITMAPとして作成(作成するサイズは本来表示したい画面のサイズ)
	SelectObject(bhdc, hBackBMP);			// 作ったものを貼り付ける
	PatBlt(bhdc, 0, 0, xSize, ySize, BLACKNESS);		// 黒で塗りつぶす
	DeleteObject(hBackBMP);				// 借りたものは返す
	ReleaseDC(hwnd, hdc);				// 借りたものは返す
	return bhdc;
}
