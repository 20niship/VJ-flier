#include "runApp.h"

runApp* g_ptr;


void onMouse(int eventType, int x, int y, int flags, void* param){
	g_ptr->CallBackFunc(eventType, x, y);
}


void runApp::CallBackFunc(int eventType, int x, int y) {
	if (eventType == cv::EVENT_LBUTTONDOWN) {
		std::cout << "pos-->" << x << y<<"\n";
		/*if ((x < 440) && (y > 390)) {

			int width = int((x - 35) / 67);
			int	height = int((y - 390) / 60);
			std::cout << "movie choosed -> start configuration\n\n";
			//TODO : Edit Movie
		}*/

		if ((373 < y) && (562 > y)) {
			if ((480 < x) && (x < 510)) { meter_move = 1; }
			else if ((550 < x) && (x < 580)) { meter_move = 2; }
			else if ((750 < x) && (x < 790)) { meter_move = 3; }//Contrast
			else if ((830 < x) && (x < 870)) { meter_move = 4; } //Brightness
			std::cout << "meter move" << meter_move << "\n";

		}
		else if ((x > 938) && (y > 350) && (x < 1057)) {
			if (y < 383) { 
				Subwin.resetSubWindow("Audio MUTE!");
				PlaySound(NULL, NULL, 0); }
			else if ((y > 399) && (y > 445)) {
				std::cout << "Edit Movie!!!" << meter_move << "\n";
				//TODO:Edit Movie
			}
			else if ((y > 459) && (y > 504)) {
				std::cout << "Save data!!!" << meter_move << "\n";
				//TODO:Save data				
			}
			else if ((y > 516) && (y > 545)) {
				std::cout << "DEMO!!!" << meter_move << "\n";
				//TODO:Save data	
			}
			else if ((y > 560) && (y > 588)) {
				running = false;
			}

			return;
		}
	}

	if (eventType == cv::EVENT_MOUSEMOVE) {
		if (meter_move == 1) {
			volume = MAX((double(562 - y) / 180.0), 0);
			volume = MIN(1.00, volume);
			std::cout << "volume change --> " << volume << "\n";
			cv::String AAA = "volume change -->" + std::to_string(volume*100.0) + "%";
			Subwin.resetSubWindow(AAA);
			Subwin.ChangeVolumeSub(int(volume*100.0));
			/*
			if self.sound_enable[self.group_playing][self.playing_scene] :
			self.sounds[self.group_playing][self.playing_scene].set_volume(self.volume / 100.)
			*/
		}
		else if (meter_move == 2) {
			speed = MAX((double(562 - y) / 94.5), 0.3);
			speed = MIN(2.00, speed);
			std::cout << "speed change --> " << speed << "\n";
			cv::String AAA = "play speed change -->" + std::to_string(speed*100.0) + "%";
			Subwin.resetSubWindow(AAA);
			Subwin.ChangeSpeedSub(int(speed*100.0));
		}
		else if (meter_move == 3) {
			contrast = MAX((double(562 - y) / 0.945), 30);
			contrast = MIN(170, contrast);
			std::cout << "contrast change --> " << contrast << "\n";
			cv::String AAA = "play speed change -->" + std::to_string(contrast) + "%";
			Subwin.resetSubWindow(AAA);
			Subwin.ChangeContrastSub(contrast);
		}
		else if (meter_move == 4) {
			brightness = MAX((double(562 - y) / 0.945), 30);
			brightness = MIN(170, brightness);
			std::cout << "brightness change --> " << brightness << "\n";
			cv::String AAA = "brightness change -->" + std::to_string(brightness) + "%";
			Subwin.resetSubWindow(AAA);
			Subwin.ChangeBrightNessSub(brightness);
		}

		return;
	}

	if (eventType == cv::EVENT_LBUTTONUP) {
		meter_move = 0;
	}
}


void runApp::setup() {
	Subwin.subWindowSetup();

	Subwin.setupWinInfo("--- SETUP START ---");
	if (isDual) { Mainwin.MainWindowSetup(); }

	Subwin.setupWinInfo("Loading Movies...");
	LoadMovies();

	group_playing = 1;
	playing_scene = 1;
	group_reserve = 1;

	getFrames();
	std::cout << "finish thumb setting!\n";
	
	mouseParam mouseEvent;

	Subwin.setupWindowDestroy();

	cv::setMouseCallback(SUB_WINDOW_NAME, onMouse, &mouseEvent);
	
	std::cout << "----   SETUP FINISHED   MAINLOOP START!! -----  \n\n\n";
}

void runApp::ExitSystem() {
	Subwin.ExitSubWindow();
	Mainwin.ExitMainWin();

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 18; j++) {
			if (activate_movie[i][j]) {
				std::cout << "movie released --> " << i << j << "\n";
				MovieCaps[i][j].release();
			}
		}
	}
}

void runApp::MainLoop() {
	CFN = 0;
	max_frame = MovieCaps[group_playing][playing_scene].get(cv::CAP_PROP_FRAME_COUNT); //ƒtƒŒ[ƒ€”


	Subwin.ChangeScene(max_frame);

	std::cout << "max_frame --> " << max_frame << "\n";
	PlaySound("C:/Users/Owner/Desktop/Cpp_Source/OPENCV/hello_world/video/1a.wav", NULL, SND_FILENAME | SND_ASYNC);
	start_time = clock();


	while (running) {
		if (max_frame < CFN+3) {
			CFN = 0;
			MovieCaps[group_playing][playing_scene].set(cv::CAP_PROP_POS_FRAMES, 0);
			start_time = clock();
		}

		if (playing) {
			MovieCaps[group_playing][playing_scene] >> img;
			CFN++;
		}

		clock_t end = clock();

		int wait_time = int((double)CFN / (30.0 * speed) * 1000.0 - (double)(end - start_time));
		int Processsing_ratio = MIN(int((1 - wait_time / 33.3) * 100), 1000);
		
		//std::cout << wait_time << "\n";
		
		if (isDual) {
			cv::imshow(MAIN_WINDOW_NAME, img);
		}
		Subwin.updateSubWindow(CFN, Processsing_ratio, img);

		int key = cv::waitKey(int(min(max(wait_time, 1), 50))); // •\Ž¦‚Ì‚½‚ß‚É1ms‘Ò‚Â

		doKeyAction(key);

	}
			
	PlaySound(NULL, NULL, 0);
	std::cout << "program finished\n";

}


void runApp::doKeyAction(int key) {
	if (key == -1) {return;}

	if (key == ESCAPE_KEY) {
		running = false;
		return;
	}

	if (key == SPEED_DOWN_KEY) {
		speed = MAX(speed - 0.01, 0.3);
		cv::String AAA = "play speed up -->" + std::to_string(speed) + "%";
		Subwin.resetSubWindow(AAA);
		Subwin.ChangeSpeedSub(int(speed*100.0));
		return;
	}

	if (key == SPEED_UP_KEY) {
		speed = MIN(speed + 0.01, 3.0);
		cv::String AAA = "play speed up -->" + std::to_string(speed) + "%";
		Subwin.resetSubWindow(AAA);
		Subwin.ChangeSpeedSub(int(speed*100.0));
		return;
	}
	if (key == SPACE_KEY) {
		if (playing) {
			Subwin.resetSubWindow("PAUSE");
			pausing_time = clock() - start_time;
			playing = false;
		}else{
			playing = true;
			start_time = clock()-pausing_time;
			Subwin.resetSubWindow("RESTART");
		}
	}

	std::cout << "key --> " << key << "\n";

	if (KEYNUM_TO_SCENE.count(key) > 0) {
		int scene2 = KEYNUM_TO_SCENE[key];
		if (activate_movie[group_reserve][scene2]) {
			playing_scene = scene2;
			Subwin.playing_scene = scene2;
			std::cout << "scene changed to ->" << playing_scene << "\n";
			ChangeScene(playing_scene);
		}
	}

	if ((key > 48) && (key < 58)) {
		group_reserve = key - 48;
		Subwin.group_reserve = key - 48;
		Subwin.resetSubWindow("");
	}

}


void runApp::ChangeScene(int scene_num) {
	PlaySound(NULL, NULL, 0);
	std::cout << "scene changed to ->" << scene_num << "\n";
	CFN = 0;
	group_playing = group_reserve;
	max_frame = MovieCaps[group_reserve][playing_scene].get(cv::CAP_PROP_FRAME_COUNT);
	MovieCaps[group_reserve][playing_scene].set(cv::CAP_PROP_POS_FRAMES, 0);
	Subwin.ChangeScene(max_frame);
	//Mainwin.ChangeScene();

	if (activate_audio[group_reserve][playing_scene]) {
		std::string Audio_A = "data/" + std::to_string(group_reserve) + "-" + std::to_string(playing_scene) + ".wav";
		PlaySound(Audio_A.c_str(), NULL, SND_FILENAME | SND_ASYNC);
	}
	start_time = clock();
}

void runApp::LoadMovies() {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 18; j++) {
			struct stat buf;
			std::string A = "data/" + std::to_string(i) + "-" + std::to_string(j) + ".mp4";

			if (stat(A.c_str(), &buf) == 0) {
				cv::VideoCapture cap(A);
				MovieCaps[i][j] = cap;
				activate_movie[i][j] = true;

				std::string Audio_A = "data/" + std::to_string(i) + "-" + std::to_string(j) + ".wav";
				if (stat(Audio_A.c_str(), &buf) == 0) {
					activate_audio[i][j] = true;
					Subwin.setupWinInfo("Load Audio ->" + A);

				}
				Subwin.setupWinInfo("Load ->" + A);
			}
		}
	}
}


void runApp::getFrames() {
	Subwin.setupWinInfo("Loading Thumbnails...");

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 18; j++) {
			cv::Mat frame;
			if (activate_movie[i][j]) {
				MovieCaps[i][j].set(cv::CAP_PROP_POS_FRAMES, 20);
				MovieCaps[i][j] >> frame;
				MovieCaps[i][j].set(cv::CAP_PROP_POS_FRAMES, 0);
				cv::resize(frame, frame, cv::Size(), 63.0 / frame.cols, 40.0 / frame.rows);
				cv::String A = "Thumbs/" + std::to_string(i) + "-" + std::to_string(j) + ".jpg";
				cv::imwrite(A, frame);
			}else {
				frame = cv::imread("figures/no_img.jpg");
			}

			Subwin.setThumb(i, j, frame);
		}
	}
	Subwin.setupWinInfo("finished load_img...");

}



