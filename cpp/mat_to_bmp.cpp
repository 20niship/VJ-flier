#include "mat_to_bmp.h"

bool cvtMat2HDC::Convert(HDC hdc, const cv::Mat image) {
	//https://gist.github.com/AhiyaHiya/6e455a3a6c846766f1017044131bfab7
	//�ɂ��ƁA�摜�̍�����4�̔{���łȂ���΂Ȃ�Ȃ��B
	//4�̔{���łȂ��ƁA�摜��45�x�䂪�񂾌`�ŕ\�������B
	//���A�\�[�X�R�[�h�́@http://chichimotsu.hateblo.jp/entry/20121130/1354265478

	cv::Mat _image;
	cv::flip(image, _image, 0);	// �㉺���]

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



HDC cvtMat2HDC::DoubleBuffer_Create(HWND hwnd, int xSize, int ySize)	// �����F(��ʂ̃n���h��,����ʂ�X���T�C�Y,����ʂ�Y���T�C�Y)
{
	HDC hdc;
	static HDC bhdc;
	static HBITMAP hBackBMP;
	hdc = GetDC(hwnd);
	bhdc = CreateCompatibleDC(hdc);
	hBackBMP = CreateCompatibleBitmap(hdc, xSize, ySize);	// ����ʂ�BITMAP�Ƃ��č쐬(�쐬����T�C�Y�͖{���\����������ʂ̃T�C�Y)
	SelectObject(bhdc, hBackBMP);			// ��������̂�\��t����
	PatBlt(bhdc, 0, 0, xSize, ySize, BLACKNESS);		// ���œh��Ԃ�
	DeleteObject(hBackBMP);				// �؂肽���͕̂Ԃ�
	ReleaseDC(hwnd, hdc);				// �؂肽���͕̂Ԃ�
	return bhdc;
}
