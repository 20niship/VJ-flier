#pragma once


//#include <WinSock2.h> //windows
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <Windows.h>
#include <windowsx.h>
#include <random>
#include <string>
#include <time.h>     // for clock()
#include <commctrl.h>
#include <stdlib.h>
#include <math.h>
#include <string>


#include "resource.h"
#include "Video.h"
#include "MainWindow.h"
#include "mat_to_bmp.h"
#include "bmp_to_mat.h"
#include "UDP_con.h"


#pragma comment(lib, "ws2_32.lib")

// inet_addr()関数で警告が出る場合は以下で警告を無効化する。
#pragma warning(disable:4996) 

#define BUFFER_MAX 400


#pragma comment(lib, "ComCtl32.lib") //ライブラリのリンク

// size, position settings
#define CNV_MOVIE_WIDTH   432  //4の倍数
#define CNV_MOVIE_HTIGHT  243
#define CNV_SELECT_WIDTH  432  //4の倍数
#define CNV_SELECT_HTIGHT 241
#define CNV_LOGO_WIDTH    216  //4の倍数
#define CNV_LGO_HEIGHT    80

#define CNV_THUMB_WIDTH    64  //4の倍数
#define CNV_THUMB_HEIGHT   40

#define CNV_MOVIE_POS_X   10
#define CNV_MOVIE_POS_Y   36
#define CNV_SELECT_POS_X  10
#define CNV_SELECT_POS_Y  305
#define CNV_LOGO_POS_X    826
#define CNV_LOGO_POS_Y    10

#define CNV_THUMB_CH_POS_X 479
#define CNV_THUMB_CH1_POS_Y 63
#define CNV_THUMB_CH2_POS_Y 186

#define BACKGROUND_COLOR RGB(30,30,30)
#define BACK_EDIT_COLOR  RGB(70,70,70)

//ID (for Widgets)
#define ID_BTN_CONFIG  200
#define ID_BTN_PLAY    201
#define ID_BTN_SAVE    202
#define ID_BTN_SWITCH  203
#define ID_BTN_MUTE    204
#define ID_BTN_REPEAT  205
#define ID_BTN_INSERT  888
#define ID_BTN_RESET_ALL 492
#define ID_BTN_TOGLLE_OUTPUT 493

#define ID_RESET_VOL     250
#define ID_RESET_SPEED   251
#define ID_RESET_CONT    252
#define ID_RESET_BRIGHT  253
#define ID_RESET_RGB     254
#define ID_RESET_MIX     255
#define ID_RESET_MIX_VOL 256

#define ID_TRACK_VOL   299
#define ID_TRACK_CONT  298
#define ID_TRACK_BRI   223
#define ID_TRACK_INTE  206
#define ID_TRACK_SPEED 207
#define ID_TRACK_R     208
#define ID_TRACK_G     209
#define ID_TRACK_B     210
#define ID_TRACK_MIX   555
#define ID_TRACK_MIX_V 585  //mix volume

#define ID_COMBO_MIX_TYPE 231
#define ID_COMBO_IN_TYPE  232
#define ID_COMBO_OUT_DISPLAY_NUM 235
#define ID_COMBO_NORMAL_MOVIE_SIZE 237
#define ID_RADIO_SOUND_CH1 301
#define ID_RADIO_SOUND_CH2 302
#define ID_COMBO_UDP       303

#define ID_STATUS_BAR  2001
#define KEY_0 48
#define VOLUME_DEFAULT 6
#define SETUP_WINDOW_NAME "setup"

#define TIMER_SCREEN 1
#define TIMER_MOVIE_LOAD 2
#define TIMER_FOR_FPS  3

#define TIMER_SERVER_SETUP 4
#define TIMER_CLIENT_SETUP 5

#define UDP_CON_PORT 50007

#define UDP_CMD_SPEED_UP 0xC0
#define UDP_CMD_SPEED_DOWN 0xC1
#define UDP_CMD_VOLUME_UP 0xC2
#define UDP_CMD_VOLUME_DOWN 0xC3
#define UDP_CMD_STOP_PLAY 0xC4
#define UDP_CMD_ESCAPE 0xC5


BOOL CALLBACK ClientDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ServerDlgProc(HWND, UINT, WPARAM, LPARAM);


class SubWindow {
public:
	VideoController myVideo;
	//MainWindow mainWin;

	bool setup(HINSTANCE, HINSTANCE, LPSTR, int);
	bool MainLoop();
	void setWindowName(LPCSTR window_name);
	void setAppName(LPCSTR app_name);
	void setWindowSize(int, int);
	void setWindowPos(int, int);

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	//###################   以降OpenCV関連   #########################

	void ExitSubWindow();
	void ChangeSpeedSub(int);
	void ChangeVolumeSub(int);
	void ChangeContrastSub(int);
	void ChangeBrightNessSub(int);

	void AddLogText(std::string);
	void SetStatusbarText(std::string);

private:
	static LRESULT CALLBACK MessageRouter(HWND, UINT, WPARAM, LPARAM);

	std::map<int, int> KEYNUM_TO_SCENE = {
		{ 113, 0 }, //Q
		{ 119, 1 }, //W
		{ 101, 2 },  //E
		{ 114, 3 },  //R
		{ 116, 4 },  //T
		{ 121, 5 },  //Y
		{ 97, 6 },  //A
		{ 115, 7 },  //S
		{ 100, 8 },  //D
		{ 102, 9 },  //F
		{ 103, 10 },  //G
		{ 104, 11 },  //H
		{ 122, 12 },  //Z
		{ 120, 13 },  //X
		{ 99, 14 },  //C
		{ 118, 15 },  //V
		{ 98, 16 },  //B
		{ 110, 17 },  //N
	};
	std::string NUM_TO_SCENE[18] = { "q", "w", "e", "r", "t", "y", "a", "s", "d", "f", "g", "h", "z", "x", "c", "v", "b", "n" };

	void CreateWindows(HWND);
	void setupWidowStart();
	void setupWinInfo(cv::String);
	void setupWindowDestroy();

	void setSpeed(int);
	void setBright(int);
	void setVolume(int);
	void setContrast(int);
	void setR_color(int);
	void setG_color(int);
	void setB_color(int);
	void setMix(int);
	void setMix_vol(int);
	void TogglePlayPause();
	void ToggleAudio();
	void TogglePlayChannel();
	void ToggleRepeatMode();
	void ToggleEditMoviesMode();
	void ShowChannelThumb();
	//bool SetNewMovie(int, int, std::string);

	void resetSubWindow();
	void updateSubWindow();
	void ChangeScene();
	void reWriteBitmaps(HWND);
	void ShowMovieWindow();
	void ShowSelectWindow();
	void ShowLogoWindow();
	void DrawProgressBar(HDC,int,int, int);
	void ShowFPS();

	int doKeyAction(WPARAM);
	int doCommandAction(WPARAM, HWND);
	inline void doUDPAction(uint8_t);


	std::string LogText = "log......";
	HINSTANCE _hCurInst;
	HINSTANCE _hPrevInst;
	LPSTR _lpsCmdLine;
	int _nCmdShow;
	HINSTANCE _hInst;

	WNDCLASSEX _wc;
	HWND _hWnd;
	HWND hBgn_insert, hBtn_SwitchCh,hBtn_repeat, hBtn_output,
		hBtn_play, hBtn_mute,
		hBtn_r_vol, hBtn_r_speed, hBtn_r_cont, hBtn_r_bright, Btn_r_RGB, Btn_r_mix, Btn_r_mix_vol,  //各種設定項目リセット
		hEdit_log, hEdit_UDPname,     //log出力をここに表示
		hEdit_ch1, hEdit_ch2, //各動画のチャンネル表示
		hEdt_p_vol, hEdt_p_speed, hEdt_p_cont, hEdt_p_bright, hEdt_p_R, hEdt_p_G, hEdt_p_B, hEdt_mix_vol, hEdt_mix,
		hTrack_vol,hTrack_cont,hTrack_bright,hTrack_mix, //ch1とch2の混ざり具合（アルファ）の設定
		hTrack_vol_mix, //ch1とch2の音の混ざり具合（アルファ）の設定
		hTrack_R, hTrack_G, hTrack_B,//各色の強度設定
		hTrack_speed,     //speed調整
		hRadio_sound_ch1, hRadio_sound_ch2, //サウンドのON/OFF
		hGrp_general,hGrp_advanced,hGrp_ch1,hGrp_ch2,hGrp_mix,hGrp_UDP, //グループボックス
		hStatusBar, 
		hComb_MixType,hCombInputType,hBombOutDisplayNum, hCombMovieNomalSize, hComb_UDPstatus,
		hDlgUDP;

	LPCSTR _window_name = "window_name";
	LPCSTR _app_name = "_app_name";
	int _wm_width = CW_USEDEFAULT;
	int _wm_height = CW_USEDEFAULT;
	int _wm_pos_x = CW_USEDEFAULT;
	int _wm_pos_y = CW_USEDEFAULT;
	bool edit_movie = false;

	const HBRUSH hBrush_BackWindow = CreateSolidBrush(BACKGROUND_COLOR);
	const HBRUSH hBrush_BackEdit  = CreateSolidBrush(BACK_EDIT_COLOR);

	//###################   以降OpenCV関連   #########################
	HDC hdc_movie, hdc_select, hdc_ch1, hdc_ch2; //hdc_logo
	cvtMat2HDC Cnv_movie, Cnv_select, Cnv_ch1, Cnv_ch2; //Cnv_logo

	cv::Mat img_movie, img_select, img_logo, logo_img, img_no_img, CH1_no_img, CH2_no_img, bmp_cam_thumb;
	
	cv::Mat img_select_current,  //選択している枠なし
		img_select_current2, //枠あり
		img_current_frame, current_ch1_thumb, current_ch2_thumb;   //

	int percentage[2] = { 0,0 };
	int _wait_time = 0;
	int _PRrate= 0;
	int _all_frame_num[2] = { 1,1 };
	int _all_time[2] = { 1, 1 };
	int playing_ch = 0;
	int CurFPS = 30;
	int PreviousFPS = 30;

	//###################   以降　bitmap   #########################
	HBITMAP bmp_speed, bmp_volume, bmp_bright, bmp_cont, bmp_reset, bmp_reset2, bmp_movie, bmp_switch, bmp_play, bmp_stop, bmp_R, bmp_G, bmp_B, 
		bmp_sound_on, bmp_sound_off, bmp_repeat, bmp_swap, bmp_insert, bmp_no_insert, bmp_reset_all, bmp_out_on, bmp_out_off;

	void setupImages();

	HDC hMdc_bmp_speed, hMdc_bmp_volume, hMdc_bmp_bright, hMdc_bmp_cont, hMdc_bmp_movie, hMdc_bmp_R, hMdc_bmp_G, hMdc_bmp_B;
	
	HFONT hFont = CreateFont(
		15, 0, 0, 0, FW_BOLD,
		FALSE, FALSE, FALSE,
		SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		VARIABLE_PITCH | FF_ROMAN, NULL);

	HFONT hFont_small = CreateFont(
		13, 0, 0, 0, FW_BOLD,
		FALSE, FALSE, FALSE,
		SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		VARIABLE_PITCH | FF_ROMAN, NULL);

	LPCTSTR MixType[5] = {"ch1 only", "ch2_only", "Screen(Add)","Subtraction", "multiple"};
	LPCTSTR InputType[5] = { "Movie", "CAM1", "CAM2", "CAM3" ,"CAM4"};
	LPCTSTR OutDiaplayNum[5] = {"main (PC)", "diaplay 1", "diaplay 2","diaplay 3","diaplay 4"};
	LPCTSTR MovieDefaultSize[5] = { "(1920, 1080)", "(1080, 720)", "(720, 480)", "2K(2560, 1440)", "4K(4096, 2160)" };
	LPCTSTR UDPstatus[3] = { "none", "server", "client"};

	bool isDual = false;
};










class miniUDP {
private:
	enum Status {
		status_none = 0,
		status_client = 1,
		status_server = 2
	};

	short _cur_status;
	bool setup_finished;
	int sock;
	struct sockaddr_in addr;
	int _port;
	WSAData wsaData;


	/* ----------------   CLIENT   --------------*/
	std::string _server_ip;
	uint8_t self_id;
	struct timeval tv;
	struct sockaddr_in addr_server;


	/* ----------------   SERVER   --------------*/
	struct sockaddr_in client_addr_list[10];
	short client_num;
	short last_client_num;

	// setupの時のみ使用
	int sock_temp;
	struct sockaddr_in addr_temp;


public:
	miniUDP();
	inline void setStatus(short status);
	inline short getStatus();
	inline void setup(int, std::string);
	inline void listten_bind();
	inline void start_main_process();


	/*-----------    CLIENT     --------------*/
	inline uint8_t receive_byte();
	inline uint8_t getSelfID();


	/*-----------    SERVER     --------------*/
	inline void send_all(uint8_t);
	inline short getClientNum();
	inline void close();
	std::string getClientInfo();
};

