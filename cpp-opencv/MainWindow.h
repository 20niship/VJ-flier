#pragma once
# include <stdio.h>
# include <opencv2/opencv.hpp>
# include <opencv2/highgui.hpp>
# include <opencv2/videoio.hpp>


#define GROUP_NUM 10
#define SCENE_NUM 18
#define MAIN_WINDOW_NAME "screen"

class MainWindow {
public:
	void MainWindowSetup();
	void ChangeScene(int);
	void ExitMainWin();

	int group_playing = 0;
	int playing_scene = 0;
	int group_reserve = 0;
};


