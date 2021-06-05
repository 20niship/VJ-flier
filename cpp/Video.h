#pragma once

#include <windows.h>
#include <cmath>
#include <opencv2/opencv.hpp>
#pragma comment(lib,"Winmm.lib")

#include <time.h>     // for clock()
#include <mmsystem.h>
#include <string.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include "VideoWriter.h"

#define CAP_NOT_EXIST (-2)
#define CAP_REPEATED (-1)
#define CAP_NEXT__READ_SUCCESS 1
#define CAP_FINISHED 0

#define MIX_CH1_ONLY 0
#define MIX_CH2_ONLY 1
#define MIX_ADD      2
#define MIX_SUB      3
#define IMG_MULTIPLICATION 4
#define EDIT_MOVIE   5


class VideoController {
public:
	int status = CAP_NOT_EXIST;
	
	//functions
	cv::Mat getNextFrame();
	void resetVideo(int, bool);

	int  getWaitTime();
	void setVolume(int);
	void setAudioEnable(bool);
	void setFPS(int);
	void setSpeed(int);
	void setBright(int);
	void setContrast(int);
	void setVolume_mix(int);
	void setMix(int);
	void setR_color(int);
	void setG_color(int);
	void setB_color(int);
	int setMixType(int);
	bool setInputType(int);
	bool setChannelSoundEnabled(int, bool);

	int getMixType() { return Mix_type; }
	int getCFN(int i);
	int getInputType() { return Input_type; }
	bool getChannelSoundEnabled(int);

	void LoadMovies();
	void setThumbs(cv::Mat, cv::Mat);
	void ChangeScene(int);
	void ChangeScene_otherCH(int);
	void ExitSystem();
	void setNormalMovieSize(int);
	void LoadOneVideo(std::string, int, int);
	void setOneVideo();
	int ResizeVideo(int);
	bool getResizeFinished() { return resize_finished; }

	const int NormalWindow_width[5] = { 1920, 1080, 720, 2560, 4096 };
	const int NormalWindow_height[5] = { 1080, 720, 480, 1440, 2160 };

	//settings
	int speed = 100;
	int contrast = 100;
	int brightness = 100;
	int volume = 100;
	int fps = 30;
	int R_color = 100;
	int G_color = 100;
	int B_color = 100;
	int Mix_rate = 100;
	int Mix_vol_rate = 100;
	bool enable_audio = true;
	bool repeat = true;
	bool CAM_opend = false;
	int playing_ch = 0;
	int skip_number = 0;
	int skip_number_total = 0;

	//status
	int group_reserve = 1;
	int group_playing[2] = {1,1};
	int playing_scene[2] = {1,1};
	bool activate_audio[11][18] = { false };

	int all_frame_nums[11][18];
	bool playing = false;
	bool effect1 = false; //コントラストと明るさの調節
	bool effect2 = false; //RGBカラーの強さ設定

	//その他変数
	cv::Mat Thumbnails[11][18];
	clock_t start_time;
	clock_t pausing_time;
	double alpha = 0.5;
	bool activate_movie[11][18] = { false };
	bool exist_audio[11][18] = { false };
	bool movie_normal_size[11][18] = { false };
	cv::Mat Mixed_img;

private:
	int CFN[2] = { 0,0 };
	void getNextFrame_mixed();
	void SkipFrames(int);
	cv::VideoCapture MovieCaps[11][18] = { 0 };  //Group 11はカメラ用
	cv::Mat current_frame_img;
	cv::Mat img_CH1, img_CH2;
	int Mix_type = MIX_CH1_ONLY;
	int Input_type = 0;
	int _tmp_playing_scene = 1;  //CH2がCAMを使っているときに、
	int _tmp_playing_group = 1;  //元々再生していた動画を記録するため
	int normal_wnd_width = 1080;
	int normal_wnd_height = 720;
	const std::string Extension_text[4] = { ".mp4", ".avi", ".mov", ".wmv" };

	//以降、動画取り込み時専用
	VideoWriter myWriter;
	std::string New_File_name, Old_File_Name;
	int new_group;
	int new_scene;
	bool resize_finished = true;
	bool need_resize = false;
};