#pragma once

# include <stdio.h>
# include <opencv2/opencv.hpp>
# include <opencv2/highgui.hpp>
# include <opencv2/videoio.hpp>

#include "SubWindow.h"
#include "MainWindow.h"

# include <iostream>

#define ESCAPE_KEY 27
#define SPACE_KEY 32
#define SPEED_UP_KEY 43
#define SPEED_DOWN_KEY 45
#define KEY_A 47
#define KEY_N 58

class runApp {
public:
	const std::string NUM_TO_SCENE[18] = { "q", "w", "e", "r", "t", "y", "a", "s", "d", "f", "g", "h", "z", "x", "c", "v", "b", "n" };

	 std::map<int, int> KEYNUM_TO_SCENE = {
		{113, 0}, //Q
		{119, 1}, //W
		{101, 2},  //E
		{114, 3},  //R
		{116, 4},  //T
		{121, 5},  //Y
		{ 97, 6},  //A
		{115, 7},  //S
		{100, 8},  //D
		{102, 9},  //F
		{103, 10},  //G
		{104, 11},  //H
		{122, 12},  //Z
		{120, 13},  //X
		{ 99, 14},  //C
		{118, 15},  //V
		{ 98, 16},  //B
		{110, 17},  //N
	};
	

	SubWindow Subwin;
	MainWindow Mainwin;

	cv::VideoCapture MovieCaps[10][18] = {0};

	void MainLoop();
	void LoadMovies();
	void setup();
	void ExitSystem();
	int meter_move = 0;

	unsigned int group_reserve = 0;
	unsigned int group_playing = 0;
	unsigned int playing_scene = 0;

	void CallBackFunc(int, int, int);

private:
	double speed = 1.0;
	double contrast = 0.0;
	double brightness = 0.0;
	double volume = 1.0;

	bool activate_audio[10][18] = { false };
	bool activate_movie[10][18] = { false };


	cv::Mat *Thumb_imgs[10][18];
	bool running = true;
	clock_t start_time;
	clock_t pausing_time;

	cv::Mat img;

	int CFN = 0;
	int max_frame = 0;

	bool playing = true;

	void doKeyAction(int);
	bool isDual = false;

	void ChangeScene(int);
	void getFrames();
	// void CallBackFunc(int, int, int, int, void *);

};


struct mouseParam {
	int x;
	int y;
	int event;
	int flags;
};


