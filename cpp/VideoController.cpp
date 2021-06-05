#include "Video.h"
#include "resource.h"
#include  "bmp_to_mat.h"

bool ChangeVolume(int nVolume, bool bScalar){
	HRESULT hr = NULL;
	bool decibels = false;
	bool scalar = false;
	double newVolume = (double)nVolume / 100.;

	CoInitialize(NULL);
	IMMDeviceEnumerator *deviceEnumerator = NULL;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
		__uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
	IMMDevice *defaultDevice = NULL;

	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
	deviceEnumerator->Release();
	deviceEnumerator = NULL;

	IAudioEndpointVolume *endpointVolume = NULL;
	hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume),
		CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
	defaultDevice->Release();
	defaultDevice = NULL;

	// -------------------------
	float currentVolume = 0;
	endpointVolume->GetMasterVolumeLevel(&currentVolume);
	//printf("Current volume in dB is: %f\n", currentVolume);

	hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);


	if (bScalar == false)
	{
		hr = endpointVolume->SetMasterVolumeLevel((float)newVolume, NULL);
	}
	else if (bScalar == true)
	{
		hr = endpointVolume->SetMasterVolumeLevelScalar((float)newVolume, NULL);
	}
	endpointVolume->Release();

	CoUninitialize();

	return FALSE;
}

void VideoController::setThumbs(cv::Mat no_img, cv::Mat cam_thumb_img) {
	//Subwin.setupWinInfo("Loading Thumbnails...");
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 18; j++) {
			cv::Mat frame;
			if (activate_movie[i][j]) {
				MovieCaps[i][j].set(cv::CAP_PROP_POS_FRAMES, 20);
				MovieCaps[i][j] >> frame;
				MovieCaps[i][j].set(cv::CAP_PROP_POS_FRAMES, 0);
				cv::resize(frame, frame, cv::Size(), 64.0 / frame.cols, 40.0 / frame.rows);
			}else {
				frame = no_img;
			}
			Thumbnails[i][j] = frame;
		}
	}
	Thumbnails[10][2] = cam_thumb_img;
}

void VideoController::LoadMovies() {
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 18; j++) {
			struct stat buf;
			std::string A;
			for (int extension_num = 0; extension_num < 4; extension_num++) {
				A = "data/" + std::to_string(i) + "-" + std::to_string(j) + Extension_text[extension_num];
				cv::VideoCapture cap(A);

				if (cap.isOpened()){
					std::cout << A << "\n";

					MovieCaps[i][j] = cap;
					activate_movie[i][j] = true;
					all_frame_nums[i][j] = MAX(MovieCaps[i][j].get(cv::CAP_PROP_FRAME_COUNT), 1);

					std::string Audio_A = "data/" + std::to_string(i) + "-" + std::to_string(j) + ".wav";
					if (stat(Audio_A.c_str(), &buf) == 0) {
						exist_audio[i][j] = true;
						activate_audio[i][j] = true;
					}
					std::cout << "Load  ->" << A << ", Frame = " << all_frame_nums[i][j]  << "\n";
					break;
				}
				else {
					all_frame_nums[i][j] = 1;
				}
			}
		}
	}

}

void VideoController::ExitSystem() {
	//release cap
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 18; j++) {
			if (activate_movie[i][j]) {
				//std::cout << "movie released --> " << i << j << "\n";
				MovieCaps[i][j].release();
			}
		}
	}
}

cv::Mat VideoController::getNextFrame() {
	//std::cout << CFN[0] << ", " << CFN[1]  << ", " << CFN[0] - (MovieCaps[group_playing[0]][playing_scene[0]]).get(cv::CAP_PROP_POS_FRAMES)  << ", " << CFN[1] - (MovieCaps[group_playing[1]][playing_scene[1]]).get(cv::CAP_PROP_POS_FRAMES) << "\n";

	if (playing) {
		getNextFrame_mixed();

		if (effect1) {Mixed_img.convertTo(img_CH2, -1, (double)contrast/100.0, brightness-100);
		}else {img_CH2 = Mixed_img;}

		if (effect2) {
			img_CH2.copyTo(current_frame_img);
			for (int x = 0; x < img_CH2.cols - 1; x++) {
				for (int y = 0; y < img_CH2.rows - 1; y++) {
					current_frame_img.at<cv::Vec3b>(y, x)[0] = MIN(254, MAX(img_CH2.at<cv::Vec3b>(y, x)[0] + R_color - 100, 0));
					current_frame_img.at<cv::Vec3b>(y, x)[1] = MIN(254, MAX(img_CH2.at<cv::Vec3b>(y, x)[1] + G_color - 100, 0));
					current_frame_img.at<cv::Vec3b>(y, x)[2] = MIN(254, MAX(img_CH2.at<cv::Vec3b>(y, x)[2] + B_color - 100, 0));
				}
			}
		}
		else { current_frame_img = img_CH2; }
	}
	return current_frame_img;

}

void VideoController::getNextFrame_mixed() {
	switch (Mix_type) {
	case EDIT_MOVIE:
	case MIX_CH1_ONLY:
		MovieCaps[group_playing[0]][playing_scene[0]].read(img_CH1);		
		if (img_CH1.empty()) { resetVideo(0, true); MovieCaps[group_playing[0]][playing_scene[0]].read(img_CH1); }
		Mixed_img = img_CH1;
		CFN[0]++;
		return;

	case MIX_CH2_ONLY:
		MovieCaps[group_playing[1]][playing_scene[1]].read(img_CH2);
		if (Input_type == 0) { CFN[1]++;}
		if (img_CH2.empty()) { resetVideo(1, true); MovieCaps[group_playing[1]][playing_scene[1]].read(img_CH2); }
		Mixed_img = img_CH2;
		return;

	case MIX_ADD:
		CFN[0]++;		CFN[1]++;
		MovieCaps[group_playing[0]][playing_scene[0]].read(img_CH1);
		MovieCaps[group_playing[1]][playing_scene[1]].read(img_CH2);
		if (img_CH1.empty()) { resetVideo(0, true); MovieCaps[group_playing[0]][playing_scene[0]].read(img_CH1);}
		if (img_CH2.empty()) { resetVideo(1, true); MovieCaps[group_playing[1]][playing_scene[1]].read(img_CH2);}

		if (!movie_normal_size[group_playing[0]][playing_scene[0]]) { cv::resize(img_CH1, img_CH1, cv::Size(), (double)normal_wnd_width / img_CH1.cols, (double)normal_wnd_height / img_CH1.rows); }
		if (!movie_normal_size[group_playing[1]][playing_scene[1]]) { cv::resize(img_CH2, img_CH2, cv::Size(), (double)normal_wnd_width / img_CH2.cols, (double)normal_wnd_height / img_CH2.rows); }
		cv::addWeighted(img_CH1, alpha, img_CH2, 1.0 - alpha, 0.0, Mixed_img);
		return;

	case MIX_SUB:
		CFN[0]++;		CFN[1]++;
		MovieCaps[group_playing[0]][playing_scene[0]].read(img_CH1);
		MovieCaps[group_playing[1]][playing_scene[1]].read(img_CH2);
		if (img_CH1.empty()) { resetVideo(0, true); MovieCaps[group_playing[0]][playing_scene[0]].read(img_CH1); }
		if (img_CH2.empty()) { resetVideo(1, true); MovieCaps[group_playing[1]][playing_scene[1]].read(img_CH2); }

		if (!movie_normal_size[group_playing[0]][playing_scene[0]]) { cv::resize(img_CH1, img_CH1, cv::Size(), (double)normal_wnd_width / img_CH1.cols, (double)normal_wnd_height / img_CH1.rows); }
		if (!movie_normal_size[group_playing[1]][playing_scene[1]]) { cv::resize(img_CH2, img_CH2, cv::Size(), (double)normal_wnd_width / img_CH2.cols, (double)normal_wnd_height / img_CH2.rows); }
		cv::addWeighted(img_CH1, alpha, img_CH2, alpha - 1, 0.0, Mixed_img);
		return;

	case IMG_MULTIPLICATION:
		CFN[0]++;		CFN[1]++;
		MovieCaps[group_playing[0]][playing_scene[0]].read(img_CH1);
		MovieCaps[group_playing[1]][playing_scene[1]].read(img_CH2);
		if (img_CH1.empty()) { resetVideo(0, true); MovieCaps[group_playing[0]][playing_scene[0]].read(img_CH1); }
		if (img_CH2.empty()) { resetVideo(1, true); MovieCaps[group_playing[1]][playing_scene[1]].read(img_CH2); }

		if (!movie_normal_size[group_playing[0]][playing_scene[0]]) { cv::resize(img_CH1, img_CH1, cv::Size(), (double)normal_wnd_width / img_CH1.cols, (double)normal_wnd_height / img_CH1.rows); }
		if (!movie_normal_size[group_playing[1]][playing_scene[1]]) { cv::resize(img_CH2, img_CH2, cv::Size(), (double)normal_wnd_width / img_CH2.cols, (double)normal_wnd_height / img_CH2.rows); }

		img_CH1.copyTo(Mixed_img);
		for (int x = 0; x < img_CH1.cols - 1; x++) {
			for (int y = 0; y < img_CH1.rows - 1; y++) {
				Mixed_img.at<cv::Vec3b>(y, x)[0] = img_CH1.at<cv::Vec3b>(y, x)[0] * img_CH2.at<cv::Vec3b>(y, x)[0] / 255;
				Mixed_img.at<cv::Vec3b>(y, x)[1] = img_CH1.at<cv::Vec3b>(y, x)[1] * img_CH2.at<cv::Vec3b>(y, x)[1] / 255;
				Mixed_img.at<cv::Vec3b>(y, x)[2] = img_CH1.at<cv::Vec3b>(y, x)[2] * img_CH2.at<cv::Vec3b>(y, x)[2] / 255;
			}
		}
		return;

	}
}

void VideoController::resetVideo(int channel, bool mute) {
	std::cout << "reset -> " << channel << "\n";

	if(mute && channel ==playing_ch) PlaySound(NULL, NULL, 0);
	CFN[channel] = 0;
	MovieCaps[group_playing[channel]][playing_scene[channel]].set(cv::CAP_PROP_POS_FRAMES, 0);

	if (activate_audio[group_playing[channel]][playing_scene[channel]] && channel == playing_ch) {
		std::string Audio_A = "data/" + std::to_string(group_playing[channel]) + "-" + std::to_string(playing_scene[channel]) + ".wav";
		PlaySound(Audio_A.c_str(), NULL, SND_FILENAME | SND_ASYNC);
	}
	if (channel == playing_ch) start_time = clock();

	if (!repeat) {
		if (playing_ch == 0) { setMixType(1); }
		else { setMixType(0); }
	}
}

//chは変わらずに、sceneだけ変更
void VideoController::ChangeScene(int scene_num) {
	if (Input_type > 0 && playing_ch == 1) return;  //CAM入力で動画の内容を変更しようとした場合は変更を許可しない
	PlaySound(NULL, NULL, 0);
	//std::cout << "scene changed to ->" << scene_num << "\n";
	MovieCaps[group_playing[playing_ch]][playing_scene[playing_ch]].set(cv::CAP_PROP_POS_FRAMES, 0);
	CFN[playing_ch] = 0;
	playing = true;
	activate_audio[group_reserve][playing_scene[playing_ch]] = exist_audio[group_reserve][playing_scene[playing_ch]];
	group_playing[playing_ch] = group_reserve;
	MovieCaps[group_reserve][playing_scene[playing_ch]].set(cv::CAP_PROP_POS_FRAMES, 0);
	playing_scene[playing_ch] = scene_num;
	if (activate_audio[group_reserve][playing_scene[playing_ch]]) {
		std::string Audio_A = "data/" + std::to_string(group_reserve) + "-" + std::to_string(playing_scene[playing_ch]) + ".wav";
		PlaySound(Audio_A.c_str(), NULL, SND_FILENAME | SND_ASYNC);
	}
	start_time = clock();
}

void VideoController::ChangeScene_otherCH(int scene_num) {
	if (Input_type > 0 && playing_ch == 0) return;  //CAM入力で動画の内容を変更しようとした場合は変更を許可しない
	group_playing[abs(playing_ch - 1)] = group_reserve;
	playing_scene[abs(playing_ch - 1)] = scene_num;
	resetVideo(abs(playing_ch - 1), false);
}

int VideoController::getWaitTime() {
	if (Input_type >0 && playing_ch == 1) return 0; //カメラだったら即リリース

	clock_t end = clock();     // 終了時間
	int time1 = CFN[playing_ch] * 100000 / (30 * speed) - (end - start_time);
	//std::cout << skip_number_total << ", " << skip_number << "\n";
	if (time1 < -100 && playing == true && Mix_type != EDIT_MOVIE) {
		skip_number = -time1 / 100;
		skip_number_total += skip_number;
		SkipFrames(skip_number);
	}else { skip_number = 0; }
	return MIN(MAX(time1, 0), 100);
}

void VideoController::SkipFrames(int loop) {
	for (int i = 0; i < loop; i++) {
		switch (Mix_type) {
		case EDIT_MOVIE:
		case MIX_CH1_ONLY:
			MovieCaps[group_playing[0]][playing_scene[0]].read(img_CH1);
			CFN[0]++;
			return;

		case MIX_CH2_ONLY:
			MovieCaps[group_playing[1]][playing_scene[1]].read(img_CH2);
			CFN[1]++;
			return;

		case MIX_ADD:
			CFN[0]++;		CFN[1]++;
			MovieCaps[group_playing[0]][playing_scene[0]].read(img_CH1);
			MovieCaps[group_playing[1]][playing_scene[1]].read(img_CH2);
			return;

		case MIX_SUB:
			CFN[0]++;		CFN[1]++;
			MovieCaps[group_playing[0]][playing_scene[0]].read(img_CH1);
			MovieCaps[group_playing[1]][playing_scene[1]].read(img_CH2);
			return;

		case IMG_MULTIPLICATION:
			CFN[0]++;		CFN[1]++;
			MovieCaps[group_playing[0]][playing_scene[0]].read(img_CH1);
			MovieCaps[group_playing[1]][playing_scene[1]].read(img_CH2);
			return;

		}
	}
}

void VideoController::LoadOneVideo(std::string filename, int New_group, int New_scene) {
	struct stat buf;
	//const char *ext_new_video = strrchr(filename.c_str(), '.');
	new_group = New_group;
	new_scene = New_scene;
	Old_File_Name = filename;

	New_File_name = "data/" + std::to_string(new_group) + "-" + std::to_string(new_scene) + ".mp4";

	std::cout << New_File_name << "\n";

	if (stat(filename.c_str(), &buf) != 0) return;

	cv::VideoCapture cap(filename);
	int width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
	int height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	all_frame_nums[new_group][new_scene] = MAX(cap.get(cv::CAP_PROP_FRAME_COUNT), 1);

	int answer;

	//もとのファイルの削除
	cv::VideoCapture cap2;
	std::string AA2;

	for (int i = 0; i < 4; i++) {
		AA2 = "data/" + std::to_string(new_group) + "-" + std::to_string(new_scene) + Extension_text[i];
		if (stat(AA2.c_str(), &buf) == 0) {
			MovieCaps[new_group][new_scene].release();
			MovieCaps[new_group][new_scene].~VideoCapture();
			remove(AA2.c_str());
			MessageBox(NULL, (LPCSTR)("すでに" + AA2 + "が存在していたので削除しました。").c_str(), "CAUTION", MB_OK);
			if (exist_audio[new_group][new_scene]) {
				PlaySound(NULL, NULL, 0);
				remove(("data/" + std::to_string(new_group) + "-" + std::to_string(new_scene) + ".wav").c_str());
				MessageBox(NULL, (LPCSTR)("すでに" + AA2 + "の音源が存在していたので削除しました。").c_str(), "CAUTION", MB_OK);
			}
		}
	}


	need_resize = false;
	resize_finished = false;
	cap.release();

	if (width != normal_wnd_width || height != normal_wnd_height) {
		std::string msg = "この動画のサイズは(" + std::to_string(width) + ", " + std::to_string(height) + ") ですが、 (" + std::to_string(normal_wnd_width) + "*" + std::to_string(normal_wnd_height) + ")を推奨します。\n推奨サイズでない場合、FPSが落ちる場合があります。\nサイズを変更しますか？（この作業には少し時間がかかります）";
		answer = MessageBox(NULL, (LPCSTR)(msg.c_str()), "ERROR", MB_YESNO);
		if (answer != IDYES) {
			movie_normal_size[new_group][new_scene] = false; 
			need_resize = false;
			return;
		}else{
			bool result = myWriter.setVideo(Old_File_Name, New_File_name, normal_wnd_width, normal_wnd_height, true);
			if (result) { 
				movie_normal_size[new_group][new_scene] = true;
				need_resize = true;
			}else{ 
				MessageBox(NULL, (LPCSTR)"動画のリサイズに失敗したので、そのままのサイズでコピーします", "NOTICE", MB_OK); 
				movie_normal_size[new_group][new_scene] = false;
			}
			return;
		}


		//std::cout << "cap resizing......\n";
		//std::string command = "ffmpeg.exe -y -i " + filename + "-vf scale=1920:-1" + A;
		//system(command.c_str());
	}
	else { movie_normal_size[new_group][new_scene] = true; return; }

}

int VideoController::ResizeVideo(int loop) {
	if (need_resize) {
		myWriter.update(20);
		resize_finished = myWriter.getFinished();
	}else {
		bool result = CopyFile((LPCSTR)(Old_File_Name.c_str()), (LPCSTR)(New_File_name.c_str()), TRUE);

		std::cout << "video copying from " << Old_File_Name << " to " << New_File_name<<"\n";
		std::cout << "result " << result << "\n";

		resize_finished = true;
	}
	return myWriter.progress_percent;
}

void VideoController::setOneVideo() {
	//音声取り込み
	std::string msg = "この動画の音声をONにしますか？";
	int answer = MessageBox(NULL, (LPCSTR)(msg.c_str()), "ERROR", MB_YESNO);
	if (answer == IDYES) {
		std::string  command = "ffmpeg.exe -y -i " + Old_File_Name + " -vn -acodec pcm_s16le -ar 44100 -ac 2 " + "data/" + std::to_string(new_group) + "-" + std::to_string(new_scene) + ".wav";
		activate_audio[new_group][new_scene] = true;
		exist_audio[new_group][new_scene] = true;
		system(command.c_str());
	}
	else {
		activate_audio[new_group][new_scene] = false;
		exist_audio[new_group][new_scene] = false;
	}

	struct stat buf;

	if (stat(New_File_name.c_str(), &buf) != 0)return;

	cv::VideoCapture cap3(New_File_name);

	std::cout << New_File_name;
	MovieCaps[new_group][new_scene].release();

	MovieCaps[new_group][new_scene] = cap3;
	activate_movie[new_group][new_scene] = true;
	all_frame_nums[new_group][new_scene] = MAX(MovieCaps[new_group][new_scene].get(cv::CAP_PROP_FRAME_COUNT), 1);
	std::cout << "all_frame --> " << all_frame_nums[new_group][new_scene] << "\n";
	std::cout << MovieCaps[new_group][new_scene].get(cv::CAP_PROP_FRAME_WIDTH);
	std::cout << MovieCaps[new_group][new_scene].get(cv::CAP_PROP_FRAME_HEIGHT);

	cv::Mat frame;
	MovieCaps[new_group][new_scene].set(cv::CAP_PROP_POS_FRAMES, 20);
	MovieCaps[new_group][new_scene] >> frame;
	MovieCaps[new_group][new_scene].set(cv::CAP_PROP_POS_FRAMES, 0);
	cv::resize(frame, frame, cv::Size(), 64.0 / frame.cols, 40.0 / frame.rows);

	Thumbnails[new_group][new_scene] = frame;
	std::cout << "importing normalized video4......";

}

int VideoController::setMixType(int type) {
	//カメラからの画像を表示するときは、サイズが異なる可能性があるので合成は不可
	if (Input_type > 0 && type > 1) { return Mix_type; }

	Mix_type = type;

	if (type<2) {
		playing_ch = type;
		CFN[0] = 0; CFN[1] = 0;
		MovieCaps[group_playing[0]][playing_scene[0]].set(cv::CAP_PROP_POS_FRAMES, 0);
		MovieCaps[group_playing[1]][playing_scene[1]].set(cv::CAP_PROP_POS_FRAMES, 0);

		PlaySound(NULL, NULL, 0);
		if (activate_audio[group_playing[playing_ch]][playing_scene[playing_ch]]) {
			std::string Audio_A = "data/" + std::to_string(group_playing[playing_ch]) + "-" + std::to_string(playing_scene[playing_ch]) + ".wav";
			PlaySound(Audio_A.c_str(), NULL, SND_FILENAME | SND_ASYNC);
		}
		start_time = clock();
		return Mix_type;
	}
	
	bool repeat = true;

	if (type == EDIT_MOVIE) {
		PlaySound(NULL, NULL, 0);
		MovieCaps[group_playing[0]][playing_scene[0]].set(cv::CAP_PROP_POS_FRAMES, 0);
		MovieCaps[group_playing[1]][playing_scene[1]].set(cv::CAP_PROP_POS_FRAMES, 0);
		playing_ch = 0;
		group_playing[0] = 10;
		playing_scene[0] = 1;
		CFN[0] = 0;
		MovieCaps[10][1].set(cv::CAP_PROP_POS_FRAMES, 0);
		start_time = clock();
	}

	return Mix_type;
}

bool VideoController::setInputType(int type) {
	if (type == 0) {
		//TODO : カメラからのCAPをRelease
		Input_type = type;
		group_reserve = _tmp_playing_group;
		group_playing[1] = _tmp_playing_group;
		playing_scene[1] = _tmp_playing_scene;
		resetVideo(1, true);
		MovieCaps[10][2].release();
		start_time = clock();
		return true;
	}else {
		if (Mix_type == 5) return false;
		cv::VideoCapture cap(type - 1);
		if (!cap.isOpened()){
			return false;
		}
		
		//CAMが正常に動作している時
		PlaySound(NULL, NULL, 0);
		MovieCaps[group_playing[1]][playing_scene[1]].set(cv::CAP_PROP_POS_FRAMES, 0);
		_tmp_playing_scene = playing_scene[1];
		_tmp_playing_group = group_playing[1];

		int height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
		int width  = cap.get(cv::CAP_PROP_FRAME_WIDTH);
		if (width != 1920 || height != 1080) {
			setMixType(1);
		}
		Input_type = type;
		group_playing[1] = 10;
		playing_scene[1] = 2;
		MovieCaps[10][2] = cap;
		all_frame_nums[10][2] = 100;
		setMixType(1);
		std::cout << "cap set!!\n";
		return true;
	}
}


//動画の描画とは無関係なもの
void VideoController::setFPS(int FPS) { fps = FPS; }
void VideoController::setVolume(int Volume) { volume = Volume; ChangeVolume(volume, true); }
void VideoController::setAudioEnable(bool aa) { enable_audio = aa; }
void VideoController::setSpeed(int new_value) {
	start_time = clock() - CFN[playing_ch] * 100000 / (30 * speed);
	speed = new_value; 
};

void VideoController::setBright(int new_value) { brightness = new_value; };
void VideoController::setContrast(int new_value) { contrast = new_value; };
void VideoController::setVolume_mix(int new_value) { Mix_vol_rate = new_value; };
void VideoController::setMix(int new_value) { alpha = MIN(1, MAX((double)new_value / 100.0, 0)); };
void VideoController::setR_color(int new_value) { R_color = new_value; };
void VideoController::setG_color(int new_value) { G_color = new_value; };
void VideoController::setB_color(int new_value) { B_color = new_value; };
int VideoController::getCFN(int i) { return CFN[i]; }


bool VideoController::getChannelSoundEnabled(int channel) {
	return activate_audio[group_playing[channel]][playing_scene[channel]];
}

bool VideoController::setChannelSoundEnabled(int channel, bool enable) {
	activate_audio[group_playing[channel]][playing_scene[channel]] = enable && exist_audio[group_playing[channel]][playing_scene[channel]];

	if (activate_audio[group_playing[channel]][playing_scene[channel]] == false) {
		PlaySound(NULL, NULL, 0);
	}
	return true;
}

void VideoController::setNormalMovieSize(int type) {
	normal_wnd_height = NormalWindow_height[type];
	normal_wnd_width = NormalWindow_width[type];

	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 18; j++) {
			if (activate_movie[i][j]) {
				int width  = MovieCaps[i][j].get(cv::CAP_PROP_FRAME_WIDTH);
				int height = MovieCaps[i][j].get(cv::CAP_PROP_FRAME_HEIGHT);

				if (width != NormalWindow_width[type] || height != NormalWindow_height[type]) {
					std::cout << "check windw size  -> (" << i << ", " << j << ") -> need resize \n";
					movie_normal_size[i][j] = false;
				}else {
					std::cout << "check windw size  -> (" << i << ", " << j << ") -> normal size. OK\n";
					movie_normal_size[i][j] = true;
				}

			}else {	movie_normal_size[i][j] = false;}
			
		}
	}

}