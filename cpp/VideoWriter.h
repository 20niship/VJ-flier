
#pragma once
#include <windows.h>
# include <opencv2/opencv.hpp>
# include <opencv2/highgui.hpp>
# include <opencv2/videoio.hpp>
# include <stdio.h>
#include <opencv2/core.hpp>

#include <stdio.h>
#include <iostream>


class VideoWriter {
public:
	bool setVideo(std::string, std::string, int, int, bool);
	void update(int);
	bool getFinished() { return write_finished; }
	int progress_percent = 0;

private:
	int new_width = 1920;
	int new_height = 1080;
	int old_width = 1920;
	int old_height = 1080;
	int all_frame_num = 0;
	cv::VideoCapture Cap_old;
	cv::VideoCapture Cap_new;
	std::string filename;
	std::string extension = ".mp4";
	bool write_finished = true;
	int CFN = 0;
	cv::VideoWriter writer;
};
