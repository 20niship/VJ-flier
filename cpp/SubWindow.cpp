#include "SubWindow.h"
#define MY_FONT cv::FONT_HERSHEY_SIMPLEX



miniUDP::miniUDP() { setup_finished = false; }
inline void miniUDP::setStatus(short status) { _cur_status = status;  if (status == status_none) { setup_finished = false; } }
inline short  miniUDP::getStatus() { return _cur_status; }

inline void  miniUDP::setup(int port, std::string server_ip) {
	std::cout << "setup() UDP ->";
	setup_finished = false;
	std::random_device rnd;     // 非決定的な乱数生成器を生成
	std::uniform_int_distribution<> rand100(200, 250);        // [0, 99] 範囲の一様乱数
	std::mt19937 mt(rnd());     //  メルセンヌ・ツイスタの32ビット版、引数は初期シード値
	self_id = rand100(mt);
	u_long val = 1;

	switch (_cur_status) {
	case status_client:
		/* ----------------   CLIENT   --------------*/
		WSAStartup(MAKEWORD(2, 0), &wsaData);   //MAKEWORD(2, 0)はwinsockのバージョン2.0ってこと
		sock = socket(AF_INET, SOCK_DGRAM, 0);  //AF_INETはIPv4、SOCK_DGRAMはUDP通信、0は？

		tv.tv_sec = 0;
		tv.tv_usec = 10;


		_server_ip = server_ip;
		_port = port;

		addr_server.sin_family = AF_INET;  //IPv4
		addr_server.sin_port = htons(_port);   //通信ポート番号設定
		addr_server.sin_addr.S_un.S_addr = inet_addr(_server_ip.c_str()); // 送信アドレスを127.0.0.1に設定

		val = 1;
		ioctlsocket(sock_temp, FIONBIO, &val);// ここで、ノンブロッキングに設定しています

		if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv)) < 0) { perror("Error"); }
		break;


	case status_server:
		/* ----------------   SERVER   --------------*/
		client_num = 0;
		_port = port;

		tv.tv_sec = 0;
		tv.tv_usec = 1;


		//setup用の設定
		WSAStartup(MAKEWORD(2, 0), &wsaData);   //MAKEWORD(2, 0)はwinsockのバージョン2.0ってこと
		sock_temp = socket(AF_INET, SOCK_DGRAM, 0);
		addr_temp.sin_family = AF_INET;
		addr_temp.sin_port = htons(_port);
		addr_temp.sin_addr.s_addr = INADDR_ANY;
		bind(sock_temp, (struct sockaddr *)&addr_temp, sizeof(addr_temp));

		ioctlsocket(sock_temp, FIONBIO, &val);// ここで、ノンブロッキングに設定しています

		if (setsockopt(sock_temp, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv)) < 0) {
			perror("Error");
		}
		break;
	}

	std::cout << "done!\n";

}

inline void  miniUDP::listten_bind() {
	char data;
	switch (_cur_status) {
	case status_client:
		//データを送信
		data = self_id;
		sendto(sock, &data, 1, 0, (struct sockaddr *)&addr_server, sizeof(addr_server));
		break;

	case status_server:
		char senderstr[16];
		int addrlen = sizeof(addr_temp);
		recvfrom(sock_temp, &data, 1, 0, (struct sockaddr *)&addr_temp, &addrlen);
		uint8_t u_data = (uint8_t)data;

		printf("recv data: %d, address=%d\n", u_data, ntohs(addr_temp.sin_addr.S_un.S_addr));

		if (u_data > 200 && ntohs(addr_temp.sin_addr.S_un.S_addr) != 0) {
			for (int i = 0; i < client_num; i++) {
				if (client_addr_list[i].sin_addr.S_un.S_addr == addr_temp.sin_addr.S_un.S_addr) { return; }
			}


			printf("new client ->%d\n", addr_temp.sin_addr.S_un.S_addr);
			// アドレス等格納
			client_addr_list[client_num].sin_family = AF_INET;  //IPv4
			client_addr_list[client_num].sin_port = htons(_port);   //通信ポート番号設定
			client_addr_list[client_num].sin_addr.S_un.S_addr = addr_temp.sin_addr.S_un.S_addr; // 送信アドレスを127.0.0.1に設定
			client_num++;
		}
	}
}

inline void  miniUDP::start_main_process() {
	u_long val = 1;
	std::cout << "start_main_process() ....";

	switch (_cur_status) {
	case status_client:
		// setupの終了処理
		closesocket(sock);
		WSACleanup();

		//start!!
		WSAStartup(MAKEWORD(2, 0), &wsaData);   //MAKEWORD(2, 0)はwinsockのバージョン2.0ってこと
		sock = socket(AF_INET, SOCK_DGRAM, 0);  //AF_INETはIPv4、SOCK_DGRAMはUDP通信、0は？
		addr.sin_family = AF_INET;  //IPv4
		addr.sin_port = htons(_port);   //通信ポート番号設定
		addr.sin_addr.S_un.S_addr = INADDR_ANY; // INADDR_ANYはすべてのアドレスからのパケットを受信する
		bind(sock, (struct sockaddr *)&addr, sizeof(addr));

		ioctlsocket(sock, FIONBIO, &val);// ここで、ノンブロッキングに設定しています。
		break;

	case status_server:
		//setupの後処理
		closesocket(sock_temp);
		WSACleanup();
		//start!!
		WSAStartup(MAKEWORD(2, 0), &wsaData);   //MAKEWORD(2, 0)はwinsockのバージョン2.0ってこと
		sock = socket(AF_INET, SOCK_DGRAM, 0);  //AF_INETはIPv4、SOCK_DGRAMはUDP通信、0は？
		ioctlsocket(sock, FIONBIO, &val);// ここで、ノンブロッキングに設定しています。
		break;
	}
	setup_finished = true;
	std::cout << "done!\n";
}



/*-----------    CLIENT     --------------*/
inline uint8_t  miniUDP::receive_byte() {
	if (_cur_status != status_client || !setup_finished) return 0xFF;

	std::cout << "receive_byte()....";
	char buf = 0xFF;
	if (recv(sock, &buf, 1, 0) < 0) return 0xFF;
	std::cout << std::to_string(buf) << "done!\n";
	return (uint8_t)buf;
}

inline uint8_t  miniUDP::getSelfID() { return self_id; }



/*-----------    SERVER     --------------*/
inline void  miniUDP::send_all(uint8_t data_) {
	if (_cur_status != status_server) return;

	char data = data_;
	for (short i = 0; i < client_num; i++) {
		sendto(sock, &data, 1, 0, (struct sockaddr *)&client_addr_list[i], sizeof(client_addr_list[i]));
	}
}

inline short  miniUDP::getClientNum() { return client_num; }
inline void  miniUDP::close() { closesocket(sock); WSACleanup(); setup_finished = false; }

std::string miniUDP::getClientInfo() {
	if (last_client_num == client_num) return "";

	std::string info = std::string("\n\n---------\nnumber of clients : ");
	info = info + std::to_string(client_num) + "\nclient list : \n"; 
	for (int i = 0; i < client_num; i++) {
		info += (std::string("NO.") + std::to_string(i) + std::string(" = ") + std::to_string(client_addr_list[i].sin_addr.S_un.S_addr) + std::string("\n"));
	}
	info += "----------------------------\n\n";
	return info;
}




/* ////////////////////////////////////////////////////////////////////////////
///																			////
///						ここからメイン処理　　								////
///																			////
///																			////
////////////////////////////////////////////////////////////////////////////  */




static char UDP_address_text[16];
static 	miniUDP UDPobj;


/*  ########################################################
###  機能：1.Opencvを用いてSubWindow（操作ウィンドウ）の画像処理を行う
###　　　　2.VideoControllerクラスを読み込み、Video処理を行う。
###　
############################################################# */

bool SubWindow::setup(HINSTANCE hCurInst, HINSTANCE hPrevInst,LPSTR lpsCmdLine, int nCmdShow) {
	ATOM atom;

	_hCurInst = hCurInst;
	_hPrevInst = hPrevInst;
	_lpsCmdLine = lpsCmdLine;
	_nCmdShow = nCmdShow;
	_hInst = _hCurInst;
	
	setupImages();

	setupWidowStart();

	setupWinInfo("Loading Movies...");
	myVideo.LoadMovies();

	setupWinInfo("getting thumbnails...");
	
	myVideo.setThumbs(img_no_img, bmp_cam_thumb);

	setupWindowDestroy();

	//################### cv::Mat初期設定   #########################


	img_select_current = img_select.clone();
	img_select_current2 = img_select_current.clone();
	img_current_frame = img_movie.clone();

	current_ch1_thumb = CH1_no_img.clone();
	current_ch2_thumb = CH2_no_img.clone();

	_wc.cbSize = sizeof(WNDCLASSEX);
	_wc.style = CS_HREDRAW | CS_VREDRAW;
	_wc.lpfnWndProc = MessageRouter;	//プロシージャ名
	_wc.cbClsExtra = 0;
	_wc.cbWndExtra = 0;
	_wc.hInstance = hCurInst;//インスタンス
	_wc.hIcon = LoadIcon(_hInst, MAKEINTRESOURCE(ID_ICON));
	_wc.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
	_wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	_wc.lpszMenuName = MAKEINTRESOURCE(MYMENU);	 // メニュー名
	_wc.lpszClassName = (LPCSTR)_app_name;
	_wc.hIconSm = LoadIcon(_hInst, MAKEINTRESOURCE(ID_ICON_SMALL));

	
	if ((atom = RegisterClassEx(&_wc)) == 0) {
		MessageBox(NULL, "Failed to Create Window", "ERROR1-1", MB_OK);
		//std::cout << "[ ERROR 1-1 ]  -> Failed to Create Window\n";
		return false;
	}

	_hWnd = CreateWindowEx(NULL, MAKEINTATOM(atom), _window_name,
		WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE, //最小化はOK、サイズ変更は不可
		_wm_pos_x, _wm_pos_y, _wm_width, _wm_height,
		NULL, //親ウィンドウのハンドル、親を作るときはNULL
		NULL, //メニューハンドル、クラスメニューを使うときはNULL
		hCurInst,
		this);

	if (!_hWnd) {
		MessageBox(NULL, "Failed to Create Window", "ERROR1-2", MB_OK);
		std::cout << "[ ERROR 1-2 ]  -> Failed to Create Window\n";
		return false;
	}

	myVideo.setMixType(0);

	CreateWindows(_hWnd);
	
	setSpeed(100);
	setBright(100);
	setVolume(VOLUME_DEFAULT);
	setContrast(100);
	setR_color(100);
	setG_color(100);
	setB_color(100);
	setMix(50);
	setMix_vol(50);


	ShowWindow(_hWnd, _nCmdShow);
	UpdateWindow(_hWnd);	
	SetTimer(_hWnd, TIMER_SCREEN, 30, NULL);
	SetTimer(_hWnd, TIMER_FOR_FPS, 1000, NULL);
	return true;
}

void SubWindow::setupImages() {
	bmp_speed = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_SPEED));
	bmp_volume = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_VOL));
	bmp_bright = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_BRIGHT));
	bmp_cont = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_CONTRAST));
	bmp_reset = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_RESET));
	bmp_reset2 = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_RESET2));
	bmp_movie = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_MOVIE));
	bmp_switch = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_SWAP_CH));
	bmp_play = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_PLAY));
	bmp_stop = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_PAUSE));
	bmp_R = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_COLOR_R));
	bmp_G = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_COLOR_G));
	bmp_B = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_COLOR_B));
	bmp_sound_on = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_VOL_ON));
	bmp_sound_off = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_VOL_OFF));
	bmp_repeat = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_REPEAT));
	bmp_swap = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_SWAP));
	bmp_insert = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_INSERT_MODE));
	bmp_no_insert = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_PLAY_MODE));
	bmp_reset_all = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_RESET_ALL));
	bmp_out_on = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_DUAL_OFF));
	bmp_out_off = LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_DUAL_OFF));


	img_movie = cvwin::BitmapToMat((HBITMAP)LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_MOVIE_WND)));
	img_select = cvwin::BitmapToMat((HBITMAP)LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_SELECT)));
	img_logo = cvwin::BitmapToMat((HBITMAP)LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_LOGO_IMG)));
	logo_img = cvwin::BitmapToMat((HBITMAP)LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_IMG_LOGO)));
	img_no_img = cvwin::BitmapToMat((HBITMAP)LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_NO_IMG)));
	CH1_no_img = cvwin::BitmapToMat((HBITMAP)LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_CH1_NO)));
	CH2_no_img = cvwin::BitmapToMat((HBITMAP)LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_CH2_NO)));
	bmp_cam_thumb = cvwin::BitmapToMat((HBITMAP)LoadBitmap(_hInst, MAKEINTRESOURCE(ID_BMP_CAM_THUMB)));

}


void SubWindow::CreateWindows(HWND hWnd) {

	InitCommonControls();
	//Group Box
	{
	hGrp_general = CreateWindow("BUTTON", "General", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 462, 297, 240, 262, hWnd, NULL, _hInst, NULL);
	hGrp_advanced = CreateWindow("BUTTON", "Advanced Editing", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 720, 297, 236, 262, hWnd, NULL, _hInst, NULL);
	hGrp_ch1 = CreateWindow("BUTTON", "CHANNEL 1", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 467, 20, 164, 91, hWnd, NULL, _hInst, NULL);
	hGrp_ch2 = CreateWindow("BUTTON", "CHANNEL 2", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 467, 140, 164, 145, hWnd, NULL, _hInst, NULL);
	hGrp_mix = CreateWindow("BUTTON", "MIXING", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 656, 20, 157, 272, hWnd, NULL, _hInst, NULL);
	hGrp_UDP = CreateWindow("BUTTON", "SYNC", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 834, 20, 230, 81, hWnd, NULL, _hInst, NULL);

	}

	//Edit Box
	{
		hEdit_log = CreateWindowEx(0, "EDIT", "log.....", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE, 825, 109, 250, 188, _hWnd, (HMENU)0, _hInst, NULL);
		hEdit_ch1 = CreateWindowEx(0, "EDIT", "channel1", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE, 542, 29, 85, 22, _hWnd, (HMENU)0, _hInst, NULL);
		hEdit_ch2 = CreateWindowEx(0, "EDIT", "channel2", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE, 542, 152, 85, 22, _hWnd, (HMENU)0, _hInst, NULL);

		hEdt_p_vol = CreateWindowEx(0, "EDIT", "100%", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE, 470, 509, 45, 22, _hWnd, (HMENU)0, _hInst, NULL);
		hEdt_p_speed = CreateWindowEx(0, "EDIT", "100%", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE, 515, 509, 45, 22, _hWnd, (HMENU)0, _hInst, NULL);
		hEdt_p_bright = CreateWindowEx(0, "EDIT", "100%", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE, 728, 509, 45, 22, _hWnd, (HMENU)0, _hInst, NULL);
		hEdt_p_cont = CreateWindowEx(0, "EDIT", "100%", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE, 771, 509, 45, 22, _hWnd, (HMENU)0, _hInst, NULL);
		hEdt_p_R = CreateWindowEx(0, "EDIT", "100%", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE, 833, 374, 45, 22, _hWnd, (HMENU)0, _hInst, NULL);
		hEdt_p_G = CreateWindowEx(0, "EDIT", "100%", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE, 866, 374, 45, 22, _hWnd, (HMENU)0, _hInst, NULL);
		hEdt_p_B = CreateWindowEx(0, "EDIT", "100%", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE, 904, 374, 45, 22, _hWnd, (HMENU)0, _hInst, NULL);
		hEdt_mix = CreateWindowEx(0, "EDIT", "100%", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE, 707, 152, 45, 22, _hWnd, (HMENU)0, _hInst, NULL);
		hEdt_mix_vol = CreateWindowEx(0, "EDIT", "100%", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE, 707, 223, 45, 22, _hWnd, (HMENU)0, _hInst, NULL);

		hEdit_UDPname = CreateWindowEx(0, "EDIT", "127.0.0.1", WS_CHILD | WS_VISIBLE | ES_CENTER, 930, 40, 110, 20, _hWnd, (HMENU)0, _hInst, NULL);

		SendMessage(hEdit_log, EM_SETLIMITTEXT, (WPARAM)sizeof(char) * 1000, 0);  //字数制限
		SendMessage(hEdit_ch1, EM_SETLIMITTEXT, (WPARAM)sizeof(char) * 100, 0);
		SendMessage(hEdit_ch2, EM_SETLIMITTEXT, (WPARAM)sizeof(char) * 100, 0);
		//SendDlgItemMessage(hEdit_log, )
		SendMessage(hEdit_log, EM_SETREADONLY, 1, 0);
		SendMessage(hEdit_UDPname, EM_SETREADONLY, 1, 0);

		SendMessage(hEdit_UDPname, EM_SETLIMITTEXT, (WPARAM)sizeof(char) * 16, 0);

	}

	//Buttons
	{
		hBgn_insert = CreateWindowEx(0, "BUTTON", "#config", WS_CHILD | WS_VISIBLE | BS_BITMAP, 965, 355, 110, 57, _hWnd, (HMENU)ID_BTN_INSERT, _hInst, NULL);
		hBtn_play = CreateWindowEx(0, "BUTTON", "#play", WS_CHILD | WS_VISIBLE | BS_BITMAP, 583, 485, 88, 30, _hWnd, (HMENU)ID_BTN_PLAY, _hInst, NULL);
		hBtn_SwitchCh = CreateWindowEx(0, "BUTTON", "#switch", WS_CHILD | WS_VISIBLE | BS_BITMAP, 675, 115, 88, 28, _hWnd, (HMENU)ID_BTN_SWITCH, _hInst, NULL);
		hBtn_mute = CreateWindowEx(0, "BUTTON", "#mute", WS_CHILD | WS_VISIBLE | BS_BITMAP, 584, 518, 88, 30, _hWnd, (HMENU)ID_BTN_MUTE, _hInst, NULL);
		hBtn_repeat = CreateWindowEx(0, "BUTTON", "#repeat", WS_CHILD | WS_VISIBLE | BS_BITMAP, 965, 304, 110, 30, _hWnd, (HMENU)ID_BTN_REPEAT, _hInst, NULL);
		
		//hBtn_reset_all = CreateWindowEx(0, "BUTTON", "#reset", WS_CHILD | WS_VISIBLE | BS_BITMAP, 965, 400, 110, 57, _hWnd, (HMENU)ID_BTN_RESET_ALL, _hInst, NULL);
		hBtn_output    = CreateWindowEx(0, "BUTTON", "#output", WS_CHILD | WS_VISIBLE | BS_BITMAP, 676, 78, 88, 30, _hWnd, (HMENU)ID_BTN_TOGLLE_OUTPUT, _hInst, NULL);

		hBtn_r_vol = CreateWindowEx(0, "BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP, 473, 329, 36, 21, _hWnd, (HMENU)ID_RESET_VOL, _hInst, NULL);
		hBtn_r_speed = CreateWindowEx(0, "BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP, 518, 329, 36, 21, _hWnd, (HMENU)ID_RESET_SPEED, _hInst, NULL);
		hBtn_r_cont = CreateWindowEx(0, "BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP, 771, 348, 36, 21, _hWnd, (HMENU)ID_RESET_CONT, _hInst, NULL);
		hBtn_r_bright = CreateWindowEx(0, "BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP, 726, 348, 36, 21, _hWnd, (HMENU)ID_RESET_BRIGHT, _hInst, NULL);
		Btn_r_RGB = CreateWindowEx(0, "BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP, 835, 344, 86, 27, _hWnd, (HMENU)ID_RESET_RGB, _hInst, NULL);
		Btn_r_mix = CreateWindowEx(0, "BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP, 765, 152, 36, 21, _hWnd, (HMENU)ID_RESET_MIX, _hInst, NULL);
		Btn_r_mix_vol = CreateWindowEx(0, "BUTTON", "", WS_CHILD | WS_VISIBLE | BS_BITMAP, 765, 222, 36, 21, _hWnd, (HMENU)ID_RESET_MIX_VOL, _hInst, NULL);

		SendMessage(hBtn_r_vol, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp_reset);
		SendMessage(hBtn_r_speed, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp_reset);
		SendMessage(hBtn_r_cont, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp_reset);
		SendMessage(hBtn_r_vol, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp_reset);
		SendMessage(hBtn_r_bright, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp_reset);
		SendMessage(Btn_r_mix, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp_reset);
		SendMessage(Btn_r_mix_vol, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp_reset);
		SendMessage(Btn_r_RGB, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp_reset2);  //bmp_reset2
		SendMessage(hBtn_play, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp_play);
		SendMessage(hBtn_SwitchCh, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp_switch);
		SendMessage(hBtn_mute, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp_sound_on);
		SendMessage(hBtn_repeat, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp_repeat);
		SendMessage(hBgn_insert, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp_no_insert);
		//SendMessage(hBtn_reset_all, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp_reset_all);
		SendMessage(hBtn_output, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)bmp_out_off);
	}

	//Track bar
	{
		hTrack_vol = CreateWindowEx(0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_TOOLTIPS | TBS_VERT, 478, 358, 33, 157, _hWnd, (HMENU)ID_TRACK_VOL, _hInst, NULL);
		hTrack_speed = CreateWindowEx(0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_TOOLTIPS | TBS_VERT, 523, 358, 33, 157, _hWnd, (HMENU)ID_TRACK_SPEED, _hInst, NULL);
		hTrack_cont = CreateWindowEx(0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_TOOLTIPS | TBS_VERT, 777, 373, 30, 134, _hWnd, (HMENU)ID_TRACK_CONT, _hInst, NULL);
		hTrack_bright = CreateWindowEx(0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_TOOLTIPS | TBS_VERT, 734, 373, 30, 134, _hWnd, (HMENU)ID_TRACK_BRI, _hInst, NULL);
		hTrack_R = CreateWindowEx(0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_TOOLTIPS | TBS_VERT, 834, 394, 30, 130, _hWnd, (HMENU)ID_TRACK_R, _hInst, NULL);
		hTrack_G = CreateWindowEx(0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_TOOLTIPS | TBS_VERT, 876, 394, 30, 130, _hWnd, (HMENU)ID_TRACK_G, _hInst, NULL);
		hTrack_B = CreateWindowEx(0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_TOOLTIPS | TBS_VERT, 905, 394, 30, 130, _hWnd, (HMENU)ID_TRACK_B, _hInst, NULL);
		hTrack_mix = CreateWindowEx(0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS, 668, 187, 140, 30, _hWnd, (HMENU)ID_TRACK_MIX, _hInst, NULL);
		hTrack_vol_mix = CreateWindowEx(0, TRACKBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS, 668, 253, 140, 30, _hWnd, (HMENU)ID_TRACK_MIX_V, _hInst, NULL);

		SendMessage(hTrack_vol, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(-100, 0)); //範囲の設定
		SendMessage(hTrack_cont, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(-150, -50)); //範囲の設定
		SendMessage(hTrack_bright, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(-150, -50)); //範囲の設定
		SendMessage(hTrack_speed, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(-150, -50)); //範囲の設定
		SendMessage(hTrack_R, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(-115, -85)); //範囲の設定
		SendMessage(hTrack_G, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(-115, -85)); //範囲の設定
		SendMessage(hTrack_B, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(-115, -85)); //範囲の設定
		SendMessage(hTrack_vol_mix, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 100)); //範囲の設定
		SendMessage(hTrack_mix, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 100)); //範囲の設定
	}

	//Radio Buttons
	{
		hRadio_sound_ch1 = CreateWindowEx(0, "BUTTON", "AUDIO", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 554, 64, 76, 22, _hWnd, (HMENU)ID_RADIO_SOUND_CH1, _hInst, NULL);
		hRadio_sound_ch2 = CreateWindowEx(0, "BUTTON", "AUDIO", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 554, 182, 76, 22, _hWnd, (HMENU)ID_RADIO_SOUND_CH2, _hInst, NULL);
		SendMessage(hRadio_sound_ch1, BM_SETCHECK, BST_UNCHECKED, 0);
		SendMessage(hRadio_sound_ch1, BM_SETCHECK, BST_UNCHECKED, 0);
	}

	//STATIC　文字
	HWND hWnd1, hWnd2, hWnd3, hWnd4, hWnd5, hWnd6;
	{
		hWnd1 = CreateWindow("STATIC", "OUTPUT (30fps, preview window)", WS_CHILD | WS_VISIBLE | SS_CENTER, 64, 10, 272, 33, hWnd, (HMENU)1, _hInst, NULL);
		hWnd2 = CreateWindow("STATIC", "OUTPUT", WS_CHILD | WS_VISIBLE | SS_CENTER, 675, 58, 119, 15, hWnd, (HMENU)1, _hInst, NULL);
		hWnd3 = CreateWindow("STATIC", "MASTER OUT", WS_CHILD | WS_VISIBLE | SS_CENTER, 725, 313, 111, 25, hWnd, (HMENU)1, _hInst, NULL);
		//[Ver4.0 追加]
		hWnd4 = CreateWindow("STATIC", "STATUS=", WS_CHILD | WS_VISIBLE | SS_LEFT, 843, 42, 80, 25, hWnd, (HMENU)1, _hInst, NULL);
		hWnd5 = CreateWindow("STATIC", "NAME=",   WS_CHILD | WS_VISIBLE | SS_LEFT, 843, 68, 80, 25, hWnd, (HMENU)1, _hInst, NULL);

	}

	//ステータスバー
	{
		hStatusBar = CreateWindowEx(0, STATUSCLASSNAME, NULL, WS_CHILD | CCS_BOTTOM | WS_VISIBLE, 0, 0, 0, 0, hWnd, (HMENU)ID_STATUS_BAR, _hInst, NULL);
		SendMessage(hStatusBar, SB_SIMPLE, 0, 0);
		SendMessage(hStatusBar, SB_SETTEXT, 0 | 0, (LPARAM)"Starting Process......");
	}
	
	//コンボボックス
	{
		hComb_MixType = CreateWindow("COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 670, 35, 127, 105, hWnd, (HMENU)ID_COMBO_MIX_TYPE, _hInst, NULL);
		hCombInputType = CreateWindow("COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 479, 240, 127, 105, hWnd, (HMENU)ID_COMBO_IN_TYPE, _hInst, NULL);
		hBombOutDisplayNum = CreateWindow("COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_SORT | CBS_DROPDOWNLIST, 965, 430, 110, 119, hWnd, (HMENU)ID_COMBO_OUT_DISPLAY_NUM, _hInst, NULL);
		hCombMovieNomalSize = CreateWindow("COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 965, 470, 110, 119, hWnd, (HMENU)ID_COMBO_NORMAL_MOVIE_SIZE, _hInst, NULL);

		hComb_UDPstatus = CreateWindow("COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 930, 65, 110, 80, hWnd, (HMENU)ID_COMBO_UDP, _hInst, NULL);

		for (int i = 0; i < 3; i++) { SendMessage(hComb_UDPstatus, CB_ADDSTRING, 0, (LPARAM)UDPstatus[i]); }

		for (int i = 0; i < 5; i++) {
			SendMessage(hComb_MixType, CB_ADDSTRING, 0, (LPARAM)MixType[i]);
			SendMessage(hCombInputType, CB_ADDSTRING, 0, (LPARAM)InputType[i]);
			SendMessage(hBombOutDisplayNum, CB_ADDSTRING, 0, (LPARAM)OutDiaplayNum[i]);
			SendMessage(hCombMovieNomalSize, CB_ADDSTRING, 0, (LPARAM)MovieDefaultSize[i]);
		}

		SendMessage(hComb_MixType, CB_SETCURSEL, 0, 0);
		SendMessage(hCombInputType, CB_SETCURSEL, 0, 0);
		SendMessage(hBombOutDisplayNum, CB_SETCURSEL, 1, 0);
		SendMessage(hCombMovieNomalSize, CB_SETCURSEL, 1, 0);
		SendMessage(hComb_UDPstatus, CB_SETCURSEL, 0, 0);
	}

	//各Bitmapの表示用HDCの設定
	{
		HDC hdc;
		hdc = GetDC(_hWnd);
		hMdc_bmp_speed = CreateCompatibleDC(hdc);  SelectObject(hMdc_bmp_speed, bmp_speed);
		hMdc_bmp_volume = CreateCompatibleDC(hdc); SelectObject(hMdc_bmp_volume, bmp_volume);
		hMdc_bmp_bright = CreateCompatibleDC(hdc); SelectObject(hMdc_bmp_bright, bmp_bright);
		hMdc_bmp_cont = CreateCompatibleDC(hdc);  SelectObject(hMdc_bmp_cont, bmp_cont);
		hMdc_bmp_movie = CreateCompatibleDC(hdc);  SelectObject(hMdc_bmp_movie, bmp_movie);
		hMdc_bmp_R = CreateCompatibleDC(hdc);  SelectObject(hMdc_bmp_R, bmp_R);
		hMdc_bmp_G = CreateCompatibleDC(hdc);  SelectObject(hMdc_bmp_G, bmp_G);
		hMdc_bmp_B = CreateCompatibleDC(hdc);  SelectObject(hMdc_bmp_B, bmp_B);


		ReleaseDC(_hWnd, hdc);
	}

	//文字のフォントを設定
	{
		SendMessage(hBtn_play, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		//SendMessage(hBtn_save, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		//SendMessage(Btn_r_RGB, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0)); 
		//SendMessage(Btn_r_mix, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		//SendMessage(Btn_r_mix_vol, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hEdit_log, WM_SETFONT, (WPARAM)hFont_small, MAKELPARAM(FALSE, 0));
		SendMessage(hEdit_ch1, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hEdit_ch2, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hEdt_p_vol, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hEdt_p_speed, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hEdt_p_cont, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hEdt_p_bright, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hEdt_p_R, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hEdt_p_G, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hEdt_p_B, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hEdt_mix_vol, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hEdt_mix, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hEdit_UDPname, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));

		SendMessage(hRadio_sound_ch1, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hRadio_sound_ch2, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
	
		SendMessage(hGrp_general, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hGrp_advanced, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hGrp_ch1, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hGrp_ch2, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hGrp_mix, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hStatusBar, WM_SETFONT, (WPARAM)hFont_small, MAKELPARAM(FALSE, 0));
		SendMessage(hComb_MixType, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hCombInputType, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hBombOutDisplayNum, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hCombMovieNomalSize, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hComb_UDPstatus, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));

		SendMessage(hWnd1, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hWnd2, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hWnd3, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hWnd4, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
		SendMessage(hWnd5, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(FALSE, 0));
	}
	//std::cout << "Finished Creating widgets\n";

}

bool SubWindow::MainLoop() {
	MSG msg;
	BOOL bRet = GetMessage(&msg, NULL, 0, 0);
	if (bRet != 0) {
		if (bRet == -1) {
			return false;
		}
		else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return true;
}

void SubWindow::setWindowSize(int w, int h) { _wm_width = w; _wm_height = h; }
void SubWindow::setWindowPos(int x, int y) { _wm_pos_x = x; _wm_pos_y = y; }
void SubWindow::setWindowName(LPCSTR window_name) { _window_name = window_name; }
void SubWindow::setAppName(LPCSTR app_name) { _app_name = app_name; }


LRESULT CALLBACK SubWindow::MessageRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	SubWindow* app;
	if (msg == WM_CREATE) {
		app = (SubWindow*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)app);

	}
	else {
		app = (SubWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	}
	return app->WndProc(hWnd, msg, wParam, lParam);
}

LRESULT SubWindow::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	HDC hdcStatic;
	uint8_t buf;
	int num, i;
	HDROP hdrop;
	TCHAR filename[100];
	char mouse_pos[50];
	WINDOWINFO windowinfo;
	POINT dropped_point;
	int new_scene = 5;

	switch (msg) {
	case WM_CREATE:
		Cnv_movie.hinfo  = { sizeof(BITMAPINFOHEADER), CNV_MOVIE_WIDTH,  CNV_MOVIE_HTIGHT, 1, 24, BI_RGB,0, 0, 0, 0, 0,{ NULL,NULL,NULL } };
		Cnv_select.hinfo = { sizeof(BITMAPINFOHEADER), CNV_SELECT_WIDTH, CNV_SELECT_HTIGHT,1, 24, BI_RGB,0, 0, 0, 0, 0,{ NULL,NULL,NULL } };
		//Cnv_logo.hinfo   = { sizeof(BITMAPINFOHEADER), CNV_LOGO_WIDTH,   CNV_LGO_HEIGHT,   1, 24, BI_RGB,0, 0, 0, 0, 0,{ NULL,NULL,NULL } };

		Cnv_ch1.hinfo = { sizeof(BITMAPINFOHEADER), CNV_LOGO_WIDTH,   CNV_LGO_HEIGHT,   1, 24, BI_RGB,0, 0, 0, 0, 0,{ NULL,NULL,NULL } };
		Cnv_ch2.hinfo = { sizeof(BITMAPINFOHEADER), CNV_LOGO_WIDTH,   CNV_LGO_HEIGHT,   1, 24, BI_RGB,0, 0, 0, 0, 0,{ NULL,NULL,NULL } };

		hdc_movie  = Cnv_movie.DoubleBuffer_Create(hWnd, CNV_MOVIE_WIDTH,  CNV_MOVIE_HTIGHT);
		hdc_select = Cnv_select.DoubleBuffer_Create(hWnd,CNV_SELECT_WIDTH, CNV_SELECT_HTIGHT);
		hdc_ch1 = Cnv_ch1.DoubleBuffer_Create(hWnd, CNV_THUMB_WIDTH, CNV_THUMB_HEIGHT);
		hdc_ch2 = Cnv_ch2.DoubleBuffer_Create(hWnd, CNV_THUMB_WIDTH, CNV_THUMB_HEIGHT);

		resetSubWindow();
		reWriteBitmaps(_hWnd);

		InvalidateRect(hWnd, NULL, FALSE);
		myVideo.ChangeScene(1);
		break;

	case WM_TIMER:
		switch (wp) {
		case TIMER_SCREEN:
			buf = UDPobj.receive_byte();
			doUDPAction(buf);

			_wait_time = myVideo.getWaitTime();
			SetTimer(hWnd, TIMER_SCREEN, _wait_time, NULL); //_wait_time
			_PRrate = MAX(MIN(((3333 - _wait_time * 100) / 33), 100), 0);
			percentage[0] = myVideo.getCFN(0) * 100. / _all_frame_num[0];
			percentage[1] = myVideo.getCFN(1) * 100. / _all_frame_num[1];
			if (!GetAsyncKeyState(VK_LBUTTON)) SetFocus(_hWnd);
			updateSubWindow();
			break;

		case TIMER_MOVIE_LOAD:
			KillTimer(_hWnd, TIMER_MOVIE_LOAD);
			int percentage;
			percentage = myVideo.ResizeVideo(20);
			AddLogText("rendering -> " + std::to_string(percentage) + "%\r\n");

			if(myVideo.getResizeFinished()){
				myVideo.setOneVideo();
				TogglePlayPause();
				resetSubWindow();
				updateSubWindow();
				ShowSelectWindow();
				AddLogText("video loaded !!\r\n");
				InvalidateRect(hWnd, NULL, FALSE);
				SetTimer(_hWnd, TIMER_SCREEN, 30, NULL);
			}else {
				SetTimer(_hWnd, TIMER_MOVIE_LOAD, 10, NULL);
			}
			break;

		case TIMER_FOR_FPS:
			ShowFPS();
			myVideo.skip_number_total = 0;
			break;

		case TIMER_CLIENT_SETUP:
			std::cout << "send data!!\n";
			UDPobj.listten_bind();
			break;


		case TIMER_SERVER_SETUP:
			std::cout << "listening.....\n";
			UDPobj.listten_bind();
			std::string info; 
			info = UDPobj.getClientInfo();
			AddLogText(info);

			break;

		}
		break;

	case WM_PAINT:
		reWriteBitmaps(hWnd);
		resetSubWindow();
		ChangeScene();
		ShowMovieWindow();
		ShowSelectWindow();
		ShowLogoWindow();
		break;

	case WM_CHAR:
		int r;
		r = doKeyAction(wp);
		if (r == 0) return 0;
		break;
		
	case WM_CTLCOLORSTATIC:
		HBRUSH hBrush_BackWindow;
		hBrush_BackWindow = CreateSolidBrush(BACKGROUND_COLOR);
		hdcStatic = (HDC)wp;
		SetTextColor(hdcStatic, RGB(255, 255, 255));
		SetBkColor(hdcStatic, BACKGROUND_COLOR);
		return (long)hBrush_BackWindow;
	case WM_CTLCOLOREDIT:
		HBRUSH hBrush_BackEdit;
		hBrush_BackEdit = CreateSolidBrush(BACK_EDIT_COLOR);
		hdcStatic = (HDC)wp;
		SetTextColor(hdcStatic, RGB(255, 255, 255));
		SetBkColor(hdcStatic, BACK_EDIT_COLOR);
		return (long)hBrush_BackEdit;

	case WM_COMMAND:
		doCommandAction(wp, hWnd);
		break;

	case WM_NOTIFY:
		switch (LOWORD(wp)) {
			case ID_TRACK_SPEED:setSpeed(- SendMessage(hTrack_speed, TBM_GETPOS, 0, 0)); break;
			case ID_TRACK_VOL:  setVolume( - SendMessage(hTrack_vol,     TBM_GETPOS, 0, 0)); break;
			case ID_TRACK_CONT: setContrast(-SendMessage(hTrack_cont,    TBM_GETPOS, 0, 0)); break;
			case ID_TRACK_BRI:  setBright( - SendMessage(hTrack_bright,  TBM_GETPOS, 0, 0)); break;
			case ID_TRACK_R:    setR_color( -SendMessage(hTrack_R,       TBM_GETPOS, 0, 0)); break;
			case ID_TRACK_G:    setG_color( -SendMessage(hTrack_G,       TBM_GETPOS, 0, 0)); break;
			case ID_TRACK_B:    setB_color(- SendMessage(hTrack_B,       TBM_GETPOS, 0, 0)); break;
			case ID_TRACK_MIX:  setMix(SendMessage(hTrack_mix,     TBM_GETPOS, 0, 0)); break;
			case ID_TRACK_MIX_V:setMix_vol(SendMessage(hTrack_vol_mix, TBM_GETPOS, 0, 0)); break;
		}
		break;

	case WM_DROPFILES:
		KillTimer(_hWnd, TIMER_SCREEN);
		AddLogText("File Dropped\n");
		GetCursorPos(&dropped_point);
		windowinfo.cbSize = sizeof(WINDOWINFO);
		GetWindowInfo(hWnd, &windowinfo);

		dropped_point.x -= windowinfo.rcWindow.left + 3;
		dropped_point.y -= windowinfo.rcWindow.top + 22;
		new_scene = MAX(MIN(((dropped_point.x - CNV_SELECT_POS_X) / 72) + 6 * ((dropped_point.y - 56 - CNV_SELECT_POS_Y) / 68), 17),0);

		hdrop = (HDROP)wp;
		num = DragQueryFile(hdrop, -1, NULL, 0);

		for (i = 0; i < num; i++) {DragQueryFile(hdrop, i, filename, sizeof(filename) / sizeof(TCHAR));}
		DragFinish(hdrop);
	
		AddLogText(filename);
		AddLogText(std::to_string(new_scene)+"\n");
		
		myVideo.playing = true;
		TogglePlayPause();
		AddLogText("get new video!!\n");
		myVideo.LoadOneVideo(filename, myVideo.group_reserve, new_scene);
		SetTimer(_hWnd, TIMER_MOVIE_LOAD, 10, NULL);
		//SetTimer(_hWnd, TIMER_SCREEN, 30, NULL);
		break;

	case WM_CLOSE:
		//std::cout << "[ INFO 2-8 ]  -> WndProc >>> WM_CLOSE";
		ExitSubWindow();
		PostQuitMessage(0);
		//mainWin.ExitSystem();
		break;

	case WM_DESTROY:
		//std::cout << "[ INFO 2-8 ]  -> WndProc >>> WM_DESTROY";
		DeleteDC(hdc_movie);
		KillTimer(hWnd, TIMER_SCREEN);
		ExitSubWindow();
		PostQuitMessage(0);
		if (IsWindow(hDlgUDP)) DestroyWindow(hDlgUDP);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wp, lp);
}


/* *******************************************************************
***   setup___  
***   セットアップ時に表示されるWindow（pure OpenCVで起動）
******************************************************************* */
void SubWindow::setupWidowStart() {
	cv::namedWindow(SETUP_WINDOW_NAME, cv::WINDOW_NORMAL);
	cv::imshow(SETUP_WINDOW_NAME, logo_img);
	cv::waitKey(1);
}
void SubWindow::setupWinInfo(cv::String info_c){
	cv::Mat Rendered_img = logo_img.clone();

	cv::putText(Rendered_img, info_c, cv::Point(210, 256), MY_FONT, 0.8, cv::Scalar(200, 200, 200), 0,16);
	cv::imshow(SETUP_WINDOW_NAME, Rendered_img);
	cv::waitKey(1);
}
void SubWindow::setupWindowDestroy() {
	cv::destroyWindow(SETUP_WINDOW_NAME);
}


/* *******************************************************************
***                     描画関連
******************************************************************* */
// 機能 -> 動画と、Progress Barを更新
// 起動 -> WM_TIMERで起動
void SubWindow::updateSubWindow() {
	img_current_frame = myVideo.getNextFrame();
	if (isDual) cv::imshow(MAIN_WINDOW_NAME, img_current_frame);

	cv::Mat resized_img;
	cv::resize(img_current_frame, resized_img, cv::Size(), (double)CNV_MOVIE_WIDTH / img_current_frame.cols, (double)CNV_MOVIE_HTIGHT / img_current_frame.rows);

	Cnv_movie.Convert(hdc_movie, resized_img);
	HDC hdc = GetDC(_hWnd);
	BitBlt(hdc, CNV_MOVIE_POS_X, CNV_MOVIE_POS_Y, CNV_MOVIE_WIDTH, CNV_MOVIE_HTIGHT, hdc_movie, 0, 0, SRCCOPY);
	//mainWin.updateWindow(img_current_frame);

	SelectObject(hdc, hFont);
	char szStr[20], szStr2[20], szStr3[20], szStr4[20], szStr5[20], szStr6[20];
	wsprintf(szStr, "t %d / %d   ", myVideo.getCFN(0) / 30, _all_time[0]);
	wsprintf(szStr3, "t %d / %d  ", myVideo.getCFN(1) / 30, _all_time[1]);

	wsprintf(szStr2, "f %d / %d  ", myVideo.getCFN(0), _all_frame_num[0]);
	wsprintf(szStr4, "f %d / %d  ", myVideo.getCFN(1), _all_frame_num[1]);

	wsprintf(szStr5, "PR_rate : %d   ", _PRrate);

	SetBkColor(hdc, RGB(0, 0, 0));  //文字の背景色を設定
	SetTextColor(hdc, RGB(255, 255, 255));  //文字の色を設定

	TextOut(hdc, 560, 345, (LPCTSTR)szStr, (int)strlen(szStr));
	TextOut(hdc, 560, 360, (LPCTSTR)szStr2, (int)strlen(szStr2));
	TextOut(hdc, 560, 390, (LPCTSTR)szStr3, (int)strlen(szStr3));
	TextOut(hdc, 560, 405, (LPCTSTR)szStr4, (int)strlen(szStr4));
	TextOut(hdc, 560, 445, (LPCTSTR)szStr5, (int)strlen(szStr5));

	//std::cout << percentage[0] << ", " << percentage[1] << ", " << _PRrate << "\n";
	DrawProgressBar(hdc, 565, 320, percentage[0]);
	DrawProgressBar(hdc, 565, 380, percentage[1]);
	DrawProgressBar(hdc, 565, 435, _PRrate);

	ReleaseDC(_hWnd, hdc);
}

void SubWindow::ShowFPS() {
	HDC hdc = GetDC(_hWnd);
	
	SelectObject(hdc, hFont);
	char szStr6[20];
	
	wsprintf(szStr6, "FPS : %d   ", 30 - myVideo.skip_number_total);
	SetBkColor(hdc, RGB(0, 0, 0));  //文字の背景色を設定
	SetTextColor(hdc, RGB(255, 255, 255));  //文字の色を設定

	TextOut(hdc, 560, 460, (LPCTSTR)szStr6, (int)strlen(szStr6));
	ReleaseDC(_hWnd, hdc);
}

// 機能 -> ビットマップを再描画
// 起動 -> WM_PAINT時と初回起動時
void SubWindow::reWriteBitmaps(HWND hWnd) {
	HDC hdc;
	PAINTSTRUCT ps;

	//###################  Bitmapを描画   #########################	
	hdc = BeginPaint(hWnd, &ps);
	BitBlt(hdc, 517, 525, 35, 35, hMdc_bmp_speed, 0, 0, SRCCOPY);
	BitBlt(hdc, 474, 525, 35, 35, hMdc_bmp_volume, 0, 0, SRCCOPY);
	BitBlt(hdc, 672, 143, 35, 35, hMdc_bmp_volume, 0, 0, SRCCOPY);
	BitBlt(hdc, 732, 525, 35, 35, hMdc_bmp_bright, 0, 0, SRCCOPY);
	BitBlt(hdc, 768, 525, 35, 35, hMdc_bmp_cont, 0, 0, SRCCOPY);
	BitBlt(hdc, 672, 220, 35, 35, hMdc_bmp_movie, 0, 0, SRCCOPY);
	BitBlt(hdc, 834, 525, 35, 35, hMdc_bmp_R, 0, 0, SRCCOPY);
	BitBlt(hdc, 872, 525, 35, 35, hMdc_bmp_G, 0, 0, SRCCOPY);
	BitBlt(hdc, 907, 525, 35, 35, hMdc_bmp_B, 0, 0, SRCCOPY);

	EndPaint(hWnd, &ps);
}

void SubWindow::ShowMovieWindow() {
	HDC hdc;
	Cnv_movie.Convert(hdc_movie, img_current_frame);
	hdc = GetDC(_hWnd);
	BitBlt(hdc, CNV_MOVIE_POS_X, CNV_MOVIE_POS_Y, CNV_MOVIE_WIDTH, CNV_MOVIE_HTIGHT, hdc_movie, 0, 0, SRCCOPY);
	ReleaseDC(_hWnd, hdc);
}
void SubWindow::ShowSelectWindow() {
	HDC hdc;
	Cnv_select.Convert(hdc_select, img_select_current2);
	hdc = GetDC(_hWnd);
	BitBlt(hdc, CNV_SELECT_POS_X, CNV_SELECT_POS_Y, CNV_SELECT_WIDTH, CNV_SELECT_HTIGHT, hdc_select, 0, 0, SRCCOPY);
	SelectObject(hdc, hFont);
	ReleaseDC(_hWnd, hdc);
}
void SubWindow::ShowLogoWindow() {
	//HDC hdc;
	//Cnv_logo.Convert(hdc_logo, img_logo);
	//hdc = GetDC(_hWnd);
	//BitBlt(hdc, CNV_LOGO_POS_X, CNV_LOGO_POS_Y, CNV_LOGO_WIDTH, CNV_LGO_HEIGHT, hdc_logo, 0, 0, SRCCOPY);
	//ReleaseDC(_hWnd, hdc);
}

// 機能 -> 動画選択画面の更新、CHANNELのサムネイルの更新
// 起動 -> 動画のキーが押されたとき　OR　動画が取り込まれたとき
void SubWindow::resetSubWindow() {
	img_select_current = img_select.clone();
	//LOGの更新

	std::string AA = "reserve group change \r\n-->" + std::to_string(myVideo.group_reserve) + "\r\n";
	AddLogText(AA);

	for (int j = 0; j < 18; j++) {
		int X_index = int(j / 6);
		int Y_index = j % 6;
		cv::Mat Roi = img_select_current(cv::Rect(16 + Y_index * 67, 56 + X_index * 60, 64, 40));
		myVideo.Thumbnails[myVideo.group_reserve][j].copyTo(Roi);
	}
}

void SubWindow::DrawProgressBar(HDC hdc,int posX, int posY, int rate) {
	HBRUSH hBrush,hBrush2;
	HPEN hPen;

	hBrush = CreateSolidBrush(RGB(255, 255, 255));
	hPen = (HPEN)GetStockObject(NULL_PEN);
	SelectObject(hdc, hBrush);
	SelectObject(hdc, hPen);
	Rectangle(hdc, posX, posY, posX+108, posY+12);
	DeleteObject(hBrush);

	hBrush2 = CreateSolidBrush(RGB(255, 0, 0));
	SelectObject(hdc, hBrush2);
	Rectangle(hdc, posX+1, posY+1, posX+1+int(1.06*(double)MIN(MAX(rate,0), 100)), posY+11);
	DeleteObject(hPen);
	DeleteObject(hBrush2);
}

void SubWindow::ShowChannelThumb() {
	HDC hdc;
	hdc = GetDC(_hWnd);


	current_ch1_thumb = myVideo.Thumbnails[myVideo.group_playing[0]][myVideo.playing_scene[0]].clone();
	current_ch2_thumb = myVideo.Thumbnails[myVideo.group_playing[1]][myVideo.playing_scene[1]].clone();

	if (playing_ch == 0) {
		cv::rectangle(current_ch1_thumb, cv::Point(0, 0), cv::Point(CNV_THUMB_WIDTH, CNV_THUMB_HEIGHT), cv::Scalar(255, 255, 0), 3);
		cv::rectangle(current_ch2_thumb, cv::Point(0, 0), cv::Point(CNV_THUMB_WIDTH, CNV_THUMB_HEIGHT), cv::Scalar(255, 255, 255), 3);
	}else {
		cv::rectangle(current_ch1_thumb, cv::Point(0, 0), cv::Point(CNV_THUMB_WIDTH, CNV_THUMB_HEIGHT), cv::Scalar(255, 255, 255), 3);
		cv::rectangle(current_ch2_thumb, cv::Point(0, 0), cv::Point(CNV_THUMB_WIDTH, CNV_THUMB_HEIGHT), cv::Scalar(255, 255, 0), 3);
	}

	Cnv_ch1.Convert(hdc_ch1, current_ch1_thumb);
	Cnv_ch2.Convert(hdc_ch2, current_ch2_thumb);
	BitBlt(hdc, CNV_THUMB_CH_POS_X, CNV_THUMB_CH2_POS_Y, CNV_THUMB_WIDTH, CNV_THUMB_HEIGHT, hdc_ch2, 0, 0, SRCCOPY);
	BitBlt(hdc, CNV_THUMB_CH_POS_X, CNV_THUMB_CH1_POS_Y, CNV_THUMB_WIDTH, CNV_THUMB_HEIGHT, hdc_ch1, 0, 0, SRCCOPY);

	std::string ch1_str = std::to_string(myVideo.group_playing[0]) + "-" + std::to_string(myVideo.playing_scene[0]) + ".mp4";
	std::string ch2_str = std::to_string(myVideo.group_playing[1]) + "-" + std::to_string(myVideo.playing_scene[1]) + ".mp4";

	SendMessage(hEdit_ch1, WM_SETTEXT, 0, (LPARAM)(ch1_str.c_str()));
	SendMessage(hEdit_ch2, WM_SETTEXT, 0, (LPARAM)(ch2_str.c_str()));


	if (myVideo.getChannelSoundEnabled(0)) { SendMessage(hRadio_sound_ch1, BM_SETCHECK, BST_CHECKED, 0); }
	else{SendMessage(hRadio_sound_ch1, BM_SETCHECK, BST_UNCHECKED, 0);}

	if (myVideo.getChannelSoundEnabled(1)) { SendMessage(hRadio_sound_ch2, BM_SETCHECK, BST_CHECKED, 0); }
	else { SendMessage(hRadio_sound_ch2, BM_SETCHECK, BST_UNCHECKED, 0); }

	ReleaseDC(_hWnd, hdc);
}

//SELECT Windowに表示される枠の更新　all_frame_numの更新
void SubWindow::ChangeScene() {
	int X_index = int(myVideo.playing_scene[playing_ch] / 6);
	int Y_index = myVideo.playing_scene[playing_ch] % 6;

	//std::cout << "\nY_index ->" << Y_index << "\n";
	//std::cout << "X_index ->" << X_index << "\n";

	img_select_current2 = img_select_current.clone();

	if (playing_ch != 1 || myVideo.getInputType() == 0) cv::rectangle(img_select_current2, cv::Point(16 + Y_index * 67, 56 + X_index * 60), cv::Point(78 + Y_index * 67, 97 + X_index * 60), cv::Scalar(255, 255, 0), 2);

	cv::rectangle(img_select_current2, cv::Point(132 + myVideo.group_reserve * 30, 0), cv::Point(155 + myVideo.group_reserve * 30, 23), cv::Scalar(0, 255, 255), 2);

	for (int i = 0; i < 2; i++) {
		_all_frame_num[i] = myVideo.all_frame_nums[myVideo.group_playing[i]][myVideo.playing_scene[i]];
		_all_time[i] = _all_frame_num[i] / 30;
	}

	ShowChannelThumb();

	//LOG textの変更
	cv::String AA = "(grp, scn, ch) = " + std::to_string(myVideo.group_playing[playing_ch]) +", "+ std::to_string(myVideo.playing_scene[playing_ch]) + ", " + std::to_string(myVideo.playing_ch)+  "\r\n";
	AddLogText(AA);
}


/* *******************************************************************
***                     メインの処理
******************************************************************* */

void SubWindow::ExitSubWindow() {
	DestroyWindow(hBgn_insert);
	DestroyWindow(hBtn_play);
	DestroyWindow(hEdit_log);
	DestroyWindow(hEdit_ch1);
	DestroyWindow(hEdit_ch2);
	DestroyWindow(hTrack_vol);
	DestroyWindow(hTrack_cont);
	DestroyWindow(hTrack_bright);
	DestroyWindow(hTrack_speed);
	DestroyWindow(hRadio_sound_ch1);
	DestroyWindow(_hWnd);
	DeleteObject(hFont);

	DeleteObject(bmp_volume);
	//mainWin.ExitSystem();
	cv::destroyAllWindows();
	myVideo.ExitSystem();
}

void SubWindow::ChangeSpeedSub(int speed) {
	myVideo.speed = speed;
	myVideo.start_time = clock() + int((double)myVideo.getCFN(playing_ch) / (30.0 * (double)(1 + myVideo.speed) * 100000));
	SetStatusbarText("再生速度が変更されました。トラックバー又は, Pg UP / Pg Downキーで操作できます");
	std::string AAA = "play speed up -->" + std::to_string(myVideo.speed) + "%\r\n";
	AddLogText(AAA);
}
void SubWindow::ChangeVolumeSub(int volume) {
	myVideo.volume = volume;
	SetStatusbarText("音量が変更されました。トラックバーの上のボタンでリセットできます");
	std::string AAA = "play volume up -->" + std::to_string(myVideo.speed) + "%\r\n";
	AddLogText(AAA);
}
void SubWindow::ChangeContrastSub(int contrast) {myVideo.contrast = contrast;}
void SubWindow::ChangeBrightNessSub(int brightness) { myVideo.brightness = brightness; }

int SubWindow::doKeyAction(WPARAM key){
	cv::String AAA;
	switch (key) {
	case VK_ESCAPE:
		UDPobj.send_all(UDP_CMD_ESCAPE);
		AddLogText("\r\nEskape Key was pressed !\r\n ------    QUITTING PROCESS   ------ \r\n\r\n");
		PostQuitMessage(0);
		ExitSubWindow();
		break;
		
	case VK_UP:
		UDPobj.send_all(UDP_CMD_SPEED_UP);
		setSpeed(MAX(myVideo.speed + 1, 30));
		break;

	case VK_DOWN:
		UDPobj.send_all(UDP_CMD_SPEED_DOWN);
		myVideo.setSpeed(MAX(myVideo.speed - 1, 30));
		break;

	case VK_SPACE:
		UDPobj.send_all(UDP_CMD_STOP_PLAY);
		SetStatusbarText("Space keyで動画の再生/一時停止ができます");
		TogglePlayPause();
		break;
		
	default:
		int key_int = (int)key;
		//Group変更
		if (KEY_0 <= key_int && key_int <= KEY_0 + 10) {
			SetStatusbarText("再生Groupが変更されました。動画を再生するにはその動画のアルファベットキーを押して下さい");
			myVideo.group_reserve = key_int - KEY_0;
			resetSubWindow();
			ChangeScene();
			ShowSelectWindow();

		//Scene変更
		}else if (KEYNUM_TO_SCENE.count(key_int) > 0) {
			int scene2 = KEYNUM_TO_SCENE[key];
			if (myVideo.activate_movie[myVideo.group_reserve][scene2] && myVideo.getMixType() != 5) {
				if (GetKeyState(VK_TAB) & 0x8000) {
					SetStatusbarText("Tabキーが押されているので裏CHの動画選択を変更します -> " + std::to_string(scene2));
					myVideo.ChangeScene_otherCH(scene2);
					ShowChannelThumb();
				}else {
					SetStatusbarText("動画を再生します. -> " + std::to_string(scene2));
					myVideo.ChangeScene(scene2);
					ChangeScene();
					ShowSelectWindow();
					UDPobj.send_all((char)(scene2 * 10 + myVideo.group_reserve));
				}
			}else if (myVideo.getMixType() != 5) {
				SetStatusbarText("動画を再生するには、「INSERT MOVIE」ボタンを押してモードを切り替えてください");
			}
		}
		break;		
	}
	return 0;
}

int SubWindow::doCommandAction(WPARAM wp, HWND hWnd) {
	int type;
	bool checked, result;

	switch (HIWORD(wp)) {
	case BN_CLICKED:
		switch (LOWORD(wp)) {
		case ID_BTN_PLAY:TogglePlayPause();	break;
		case ID_BTN_MUTE:ToggleAudio();	break;
		case ID_BTN_SWITCH: TogglePlayChannel(); break;
		case ID_BTN_REPEAT: ToggleRepeatMode(); break;
		case ID_BTN_INSERT: ToggleEditMoviesMode(); break;

		case ID_BTN_TOGLLE_OUTPUT:
			if (isDual){
				//mainWin.ExitSystem();
				cv::destroyWindow(MAIN_WINDOW_NAME);
				cv::destroyAllWindows();
				isDual = false;

				AddLogText("output stopped\r\n");
				SetStatusbarText("デュアルモニタをOFFにしました。ONにするにはもう一度ボタンを押してください");
				SendDlgItemMessage(_hWnd, ID_BTN_TOGLLE_OUTPUT, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp_out_off);
			}else{
				cv::namedWindow(MAIN_WINDOW_NAME, cv::WINDOW_NORMAL);
				type = MAX(0, SendMessage(hBombOutDisplayNum, CB_GETCURSEL, 0, 0));
				cv::moveWindow(MAIN_WINDOW_NAME, type * 2000, 0);
				cv::setWindowProperty(MAIN_WINDOW_NAME, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
				isDual = true;

				SendDlgItemMessage(_hWnd, ID_BTN_TOGLLE_OUTPUT, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp_out_on);
				SetStatusbarText("デュアルモニタをONにしました。OFFにするにはもう一度ボタンを押してください。モニター位置は右下のCombo Boxから選択できます");
				AddLogText("dual output started\r\n");
			}

		case ID_RESET_VOL:setVolume(VOLUME_DEFAULT); break;
		case ID_RESET_SPEED: setSpeed(100); break;
		case ID_RESET_CONT:setContrast(100); break;
		case ID_RESET_BRIGHT:setBright(100); break;
		case ID_RESET_RGB:setR_color(100); setG_color(100); setB_color(100); break;
		case ID_RESET_MIX:setMix(50); break;
		case ID_RESET_MIX_VOL:setMix_vol(50); break;
		}

	//ダイアログのメッセージが変更された
	case CBN_SELCHANGE:
		switch (LOWORD(wp)) {
		case ID_COMBO_MIX_TYPE:
			SetStatusbarText("2つのチャンネルの動画の再生パターンを変更します。");
			int type;
			type = myVideo.setMixType(MAX(0, SendMessage(hComb_MixType, CB_GETCURSEL, 0, 0)));
			SendMessage(hComb_MixType, CB_SETCURSEL, type, 0);
			playing_ch = myVideo.playing_ch;
			ChangeScene();
			break;

		case ID_COMBO_IN_TYPE:
			type = MAX(0, SendMessage(hCombInputType, CB_GETCURSEL, 0, 0));
			if (type == myVideo.getInputType()) return 0;
			bool result;
			result = myVideo.setInputType(type);
			if (!result) {
				AddLogText("cannot open camera -> CAM" + std::to_string(type) + "\r\n");
				ChangeScene();
				ShowSelectWindow();
				SetStatusbarText("2CHで外部カメラを利用しようとしましたが、デバイスが見つかりませんでした。違う番号を試してください");
			}else {
				if (type > 0) {
					AddLogText("CAM open succeed\r\n");
					SetStatusbarText("2CHで外部カメラを利用します。動画モードに戻すには「Movie」を選択してください");
					playing_ch = 1;
					myVideo.setMixType(playing_ch);
					ShowChannelThumb();
					ShowSelectWindow();
					SendMessage(hComb_MixType, CB_SETCURSEL, playing_ch, 0);
				}else { 
					SetStatusbarText("2CHが動画モードになりました。外部カメラを使用する場合は「CAM1」～「CAM3」を選択してください。");
					ChangeScene();
					playing_ch = myVideo.playing_ch;
					ShowChannelThumb();
					AddLogText("returned to Movie mode\r\n");
				}
			}
			if(myVideo.getInputType() < 5) 	SendMessage(hCombInputType, CB_SETCURSEL, myVideo.getInputType(), 0);
			break;

		case ID_COMBO_OUT_DISPLAY_NUM:
			SetStatusbarText("デュアルモニターのDisplay番号が変更されました。別のモニター（プロジェクター）に映像が表示されます");
			type = MAX(0, SendMessage(hBombOutDisplayNum, CB_GETCURSEL, 0, 0));
			//mainWin.setWindowPos(type * 2000, 0);
			if (isDual) {
				cv::moveWindow(MAIN_WINDOW_NAME, type * 2000, 0);
				cv::setWindowProperty(MAIN_WINDOW_NAME, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
			}
			AddLogText("setWindow -> " + std::to_string(type) + "\r\n");
			break;

		case ID_COMBO_NORMAL_MOVIE_SIZE:
			SetStatusbarText("デュアルモニターのDisplay番号が変更されました。別のモニター（プロジェクター）に映像が表示されます");
			type = MAX(0, SendMessage(hCombMovieNomalSize, CB_GETCURSEL, 0, 0));
			myVideo.setNormalMovieSize(type);
			AddLogText("change movie size -> " + std::to_string(type) + "\r\n");
			break;

		case ID_COMBO_UDP:
			type = MAX(0, SendMessage(hComb_UDPstatus, CB_GETCURSEL, 0, 0));
			switch (type) {
			case 0: //None:
				SetStatusbarText("UDP通信をOFFにします");
				UDPobj.setStatus(0);
				UDPobj.close();
				SendMessage(hEdit_UDPname, WM_SETTEXT, 0, (LPARAM)"***********");
				AddLogText("UDP connection -> OFF\r\n");
				break;

			case 2: //Client
				SetStatusbarText("UDP通信をクライアントに設定します。他のPCの操作が同期されます");
				UDPobj.close();
				UDPobj.setStatus(1);
				UDPobj.setup(UDP_CON_PORT, std::string("127.0.0.1"));
				DialogBox(_hInst, "DLG_UDP_CLIENT", hWnd, (DLGPROC)ClientDlgProc);

				SendMessage(hEdit_UDPname, WM_SETTEXT, 0, (LPARAM)UDP_address_text);
				AddLogText("UDP connection -> client\r\n");
				break;


			case 1: //server
				SetStatusbarText("UDP通信をサーバーに設定します。このPCでの操作内容は他のPCで同期されます");
				UDPobj.close();
				UDPobj.setStatus(2);
				UDPobj.setup(UDP_CON_PORT, "0.0.0.0");
				DialogBox(_hInst, "DLG_UDP_SERVER", hWnd, (DLGPROC)ServerDlgProc);

				SendMessage(hEdit_UDPname, WM_SETTEXT, 0, (LPARAM)"0.0.0.0");
				AddLogText("UDP connection -> server\r\n");
				break;

			}

			AddLogText("change movie size -> " + std::to_string(type) + "\r\n");
			break;
		}

	default:
		switch (LOWORD(wp)) {
		case IDM_END:
			SendMessage(_hWnd, WM_CLOSE, 0, 0);
			break;

		case IDM_ABOUT:
			MessageBox(NULL, "Firefly ver3-1", "About", MB_OK);
			break;

		case ID_RADIO_SOUND_CH1:
			SetStatusbarText("チェックがONの時はCH1から音声が流れています。OFFにすると音声がstopします。再び音声をONにするには動画のキーアルファベットを押してください");
			checked = SendMessage(hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED;
			result = myVideo.setChannelSoundEnabled(0, checked);
			ShowChannelThumb();
			break;

		case ID_RADIO_SOUND_CH2:
			SetStatusbarText("チェックがONの時はCH2から音声が流れています。OFFにすると音声がstopします。再び音声をONにするには動画のキーアルファベットを押してください");
			checked = SendMessage(hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED;
			result = myVideo.setChannelSoundEnabled(1, checked);
			ShowChannelThumb();
			break;

		case IDM_EDIT:
			if (!edit_movie) {
				ToggleEditMoviesMode();
			}else {
				SetStatusbarText("現在、動画読み込みモードです。動画ファイルをドラッグ＆ドロップしてください");
			}
			break;

		case IDM_RETURN_PLAY_MODE:
			if (edit_movie) {
				ToggleEditMoviesMode();
			}
			else {
				SetStatusbarText("現在、動画再生モードです");
			}
			break;

		case IDM_HELP:
			ShellExecute(_hWnd, "open", "how_to_use.pdf", NULL, NULL, SW_SHOW);
			break;
		}
	}
	return 0;
	
}

//再生/一時停止の変更
void SubWindow::TogglePlayPause() {
	if (myVideo.playing) {
		AddLogText("\--> pause video\r\n");
		SetStatusbarText("一時停止しいました。再生するにはスペースキーまたは「Pause」ボタンを押してください");

		myVideo.pausing_time = clock() - myVideo.start_time;
		SendDlgItemMessage(_hWnd, ID_BTN_PLAY, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp_stop);
		myVideo.playing = false;
	} else {
		AddLogText("--> restart video\r\n");
		SetStatusbarText("動画を再生します。一時停止するにはスペースキーまたは「Play」ボタンを押してください");
		myVideo.playing = true;
		SendDlgItemMessage(_hWnd, ID_BTN_PLAY, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp_play);
		myVideo.start_time = clock() - myVideo.pausing_time;
	}
	ChangeScene();
}

void SubWindow::ToggleAudio() {
	if (myVideo.volume == 0) {  //mute
		AddLogText("audio on (20)\r\n");
		SendDlgItemMessage(_hWnd, ID_BTN_MUTE, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp_sound_on);
		setVolume(VOLUME_DEFAULT);
	} else {
		AddLogText("mute all audio\r\n");
		//SendDlgItemMessage(_hWnd, ID_BTN_MUTE, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp_sound_off);
		SendDlgItemMessage(_hWnd, ID_BTN_MUTE, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp_sound_off);
		setVolume(0);
	}
}

void SubWindow::TogglePlayChannel() {
	if (playing_ch == 0) { playing_ch = 1; }
	else { playing_ch = 0; }

	myVideo.setMixType(playing_ch);
	ChangeScene();
	SendMessage(hComb_MixType, CB_SETCURSEL, playing_ch, 0);
	std::string AAA = "channel changed -->" + std::to_string(playing_ch) + "%\r\n";
	AddLogText(AAA);
}

void SubWindow::ToggleEditMoviesMode() {
	if (edit_movie) {
		AddLogText("movie play mode\r\n");
		SetStatusbarText("動画再生モードです。動画を読み込むには「INSERT VIDEO」ボタンを押してください");
		edit_movie = false;
		DragAcceptFiles(_hWnd, FALSE);
		myVideo.setMixType(0);
		SendDlgItemMessage(_hWnd, ID_BTN_INSERT, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp_no_insert);
	}else {
		AddLogText("movie edit mode\r\n");
		SetStatusbarText("動画挿入モードです。目的の動画（.mp4）を左下の動画選択画面にドラッグ＆ドロップして下さい。ドロップした場所に動画が挿入されます");
		edit_movie = true;
		playing_ch = 0;
		myVideo.setMixType(5);
		SendMessage(hComb_MixType, CB_SETCURSEL, 0, 0);
		myVideo.repeat = true;
		ChangeScene();
		DragAcceptFiles(_hWnd, TRUE);
		SendDlgItemMessage(_hWnd, ID_BTN_INSERT, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp_insert);
	}
}

void SubWindow::ToggleRepeatMode() {
	if (myVideo.repeat) {
		AddLogText("swap channel mode\r\n");
		SetStatusbarText("swapモードです。片方のChannelの動画が最後まで再生されると、もう片方の動画が自動的に再生されます。");
		myVideo.repeat = false;
		SendDlgItemMessage(_hWnd, ID_BTN_REPEAT, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp_swap);
	}
	else {
		SetStatusbarText("repeatモードです。特定の動画をリピートします");
		AddLogText("repeat channel mode\r\n");
		myVideo.repeat = true;
		SendDlgItemMessage(_hWnd, ID_BTN_REPEAT, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp_repeat);
	}
}

//Videoの再生各種設定の変更
void SubWindow::setSpeed(int A_new) {
	myVideo.setSpeed(A_new);
	SendDlgItemMessage(_hWnd, ID_TRACK_SPEED, TBM_SETPOS, TRUE, - A_new);
	std::string AAA = std::to_string(A_new) + "%";
	SetWindowText(hEdt_p_speed, (LPCTSTR)AAA.c_str());
}
void SubWindow::setBright(int A_new) {
	myVideo.setBright(A_new);
	SendDlgItemMessage(_hWnd, ID_TRACK_BRI, TBM_SETPOS, TRUE, -A_new);

	if (myVideo.contrast == 100 && myVideo.brightness == 100) {	myVideo.effect1 = false;
	}else { myVideo.effect1 = true; }

	std::string AAA =std::to_string(A_new) + "%";
	SetWindowText(hEdt_p_bright, (LPCTSTR)AAA.c_str());
}

void SubWindow::setVolume(int A_new) {
	myVideo.setVolume(A_new);
	SendDlgItemMessage(_hWnd, ID_TRACK_VOL, TBM_SETPOS, TRUE, -A_new);
	std::string AAA = std::to_string(A_new) + "%";
	SetWindowText(hEdt_p_vol, (LPCTSTR)AAA.c_str());
	AddLogText("Volume -> ");
	AddLogText(AAA);
	AddLogText("\r\n");
}
void SubWindow::setContrast(int A_new) {
	myVideo.setContrast(A_new);
	SendDlgItemMessage(_hWnd, ID_TRACK_CONT, TBM_SETPOS, TRUE, -A_new);
	
	if (myVideo.contrast == 100 && myVideo.brightness == 100) {	myVideo.effect1 = false;
	}else{myVideo.effect1 = true;}

	std::string AAA = std::to_string(A_new) + "%";
	SetWindowText(hEdt_p_cont, (LPCTSTR)AAA.c_str());
}
void SubWindow::setR_color(int A_new) {
	myVideo.setR_color(A_new);
	SendDlgItemMessage(_hWnd, ID_TRACK_R, TBM_SETPOS, TRUE, -A_new);

	if (myVideo.R_color == 100 && myVideo.G_color == 100 && myVideo.B_color == 100) {
		myVideo.effect2 = false;
	}else { myVideo.effect2 = true; }

	std::string AAA = std::to_string(A_new) + "%";
	SetWindowText(hEdt_p_R, (LPCTSTR)AAA.c_str());
}
void SubWindow::setG_color(int A_new) {
	myVideo.setG_color(A_new);
	SendDlgItemMessage(_hWnd, ID_TRACK_G, TBM_SETPOS, TRUE, -A_new);

	if (myVideo.R_color == 100 && myVideo.G_color == 100 && myVideo.B_color == 100) {
		myVideo.effect2 = false;
	}else { myVideo.effect2 = true; }

	std::string AAA = std::to_string(A_new) + "%";
	SetWindowText(hEdt_p_G, (LPCTSTR)AAA.c_str());
}
void SubWindow::setB_color(int A_new) {
	myVideo.setB_color(A_new);
	SendDlgItemMessage(_hWnd, ID_TRACK_B, TBM_SETPOS, TRUE, -A_new);

	if (myVideo.R_color == 100 && myVideo.G_color == 100 && myVideo.B_color == 100) {
		myVideo.effect2 = false;
	}else { myVideo.effect2 = true; }

	std::string AAA = std::to_string(A_new) + "%";
	SetWindowText(hEdt_p_B, (LPCTSTR)AAA.c_str());
}
void SubWindow::setMix(int A_new) {
	SendDlgItemMessage(_hWnd, ID_TRACK_MIX, TBM_SETPOS, TRUE, A_new);
	std::string AAA = std::to_string(A_new) + "%";
	SetWindowText(hEdt_mix, (LPCTSTR)AAA.c_str());
}
void SubWindow::setMix_vol(int A_new) {
	myVideo.setMix(A_new);
	SendDlgItemMessage(_hWnd, ID_TRACK_MIX_V, TBM_SETPOS, TRUE, A_new);
	std::string AAA = std::to_string(A_new) + "%";
	SetWindowText(hEdt_mix_vol, (LPCTSTR)AAA.c_str());
}

void SubWindow::SetStatusbarText(std::string str) {SendMessage(hStatusBar, SB_SETTEXT, SB_SIMPLEID | SBT_NOBORDERS, (LPARAM)(str.c_str()));}
void SubWindow::AddLogText(std::string new_text) {
	LogText += new_text;
	SendMessage(hEdit_log, WM_SETTEXT, 0, (LPARAM)(LogText.c_str()));
	SendMessage(hEdit_log, WM_VSCROLL, SB_BOTTOM, 0);
}



/* *******************************************************************
***                     UDP接続関連
******************************************************************* */
BOOL CALLBACK ClientDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {
	static HWND hParent;

	switch (msg) {
	case WM_INITDIALOG:
		hParent = GetParent(hDlg);
		SetTimer(hParent, TIMER_CLIENT_SETUP, 300, NULL);
		SetDlgItemText(hDlg, ID_DLG_UDP_EDIT, "127.0.0.1");
		return TRUE;

	case WM_KEYDOWN:
		if (wp == VK_RETURN) {
			GetDlgItemText(hDlg, ID_DLG_UDP_EDIT, UDP_address_text, (int)sizeof(UDP_address_text) - 1);
			UDPobj.close();
			UDPobj.setup(UDP_CON_PORT, std::string(UDP_address_text));
			std::cout << "IP address change!! -> ";
			std::cout << std::string(UDP_address_text);
			return TRUE;
		}
		break;

	//case WM_TIMER:
	//	std::cout << "send data!!\n";
	//	UDPobj.listten_bind();
	//	break;

	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDCANCEL:
			GetDlgItemText(hDlg, ID_DLG_UDP_EDIT, UDP_address_text, (int)sizeof(UDP_address_text) - 1);
			UDPobj.close();
			UDPobj.setup(UDP_CON_PORT, std::string(UDP_address_text));
			std::cout << "IP address change!! -> ";
			std::cout << std::string(UDP_address_text);
			return TRUE;

		case IDOK:
			GetDlgItemText(hDlg, ID_DLG_UDP_EDIT, UDP_address_text, (int)sizeof(UDP_address_text) - 1);
			InvalidateRect(hParent, NULL, TRUE);
			UDPobj.start_main_process();
			EndDialog(hDlg, IDOK);
			DestroyWindow(hDlg);
			KillTimer(hParent, TIMER_CLIENT_SETUP);
			return TRUE;

		case ID_DLG_UDP_EDIT:
			if (HIWORD(wp) == EN_UPDATE) {
				GetDlgItemText(hDlg, ID_DLG_UDP_EDIT, UDP_address_text, (int)sizeof(UDP_address_text) - 1);
				UDPobj.close();
				UDPobj.setup(UDP_CON_PORT, std::string(UDP_address_text));
				std::cout << "IP address change!! -> ";
				std::cout << std::string(UDP_address_text);
				return TRUE;
			}
		}

		return FALSE;
	}
	return FALSE;
}



BOOL CALLBACK ServerDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp) {
	static HWND hParent;
	std::string info;
	switch (msg) {
	case WM_INITDIALOG:
		hParent = GetParent(hDlg);
		SetTimer(hParent, TIMER_SERVER_SETUP, 100, NULL);
		SetDlgItemText(hDlg, ID_DLG_UDP_EDIT, "getting Client list......");
		return TRUE;

	case WM_KEYDOWN:
		if (wp == VK_RETURN) {
			DestroyWindow(hDlg);
		}
		break;

	//case WM_TIMER:
	//	UDPobj.listten_bind();
	//	info = UDPobj.getClientInfo();
	//	SetDlgItemText(hDlg, ID_DLG_UDP_EDIT, info.c_str());
	//	break;

	case WM_COMMAND:
		if (LOWORD(wp) == IDOK) {
			GetDlgItemText(hDlg, ID_DLG_UDP_EDIT, UDP_address_text, (int)sizeof(UDP_address_text) - 1);
			KillTimer(hParent, TIMER_SERVER_SETUP);
			UDPobj.start_main_process();

			InvalidateRect(hParent, NULL, TRUE);
			EndDialog(hDlg, IDOK);
			DestroyWindow(hDlg);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}


inline void SubWindow::doUDPAction(uint8_t data) {
	printf("main recv %d \n", data);
	switch (data) {
	case 0xFF :
		return;

	case UDP_CMD_ESCAPE:
		AddLogText("\r\nEskape Key was pressed !\r\n ------    QUITTING PROCESS   ------ \r\n\r\n");
		PostQuitMessage(0);
		ExitSubWindow();
		break;

	case UDP_CMD_SPEED_UP:
		setSpeed(MAX(myVideo.speed + 1, 30));
		break;

	case UDP_CMD_SPEED_DOWN:
		myVideo.setSpeed(MAX(myVideo.speed - 1, 30));
		break;

	case UDP_CMD_STOP_PLAY:
		SetStatusbarText("Space keyで動画の再生/一時停止ができます");
		TogglePlayPause();
		break;

	case UDP_CMD_VOLUME_DOWN:
	case UDP_CMD_VOLUME_UP:
		break;

	default:
		if (data < 200) {
			int next_play_group = data % 10;
			int next_play_scene = data / 10;

			if (next_play_group != myVideo.group_reserve) {
				SetStatusbarText("再生Groupが変更されました。動画を再生するにはその動画のアルファベットキーを押して下さい");
				myVideo.group_reserve = next_play_group;
				//resetSubWindow();
				//ChangeScene();
				//ShowSelectWindow();
			}

			SetStatusbarText("動画を再生します. -> " + std::to_string(next_play_scene));
			myVideo.ChangeScene(next_play_scene);
			ChangeScene();
			ShowSelectWindow();

			SetStatusbarText("サーバーPCの操作によって再生動画が変わりました");
			return;
		}
		break;

	}
	
}


