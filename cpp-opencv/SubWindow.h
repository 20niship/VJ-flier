# include <opencv2/opencv.hpp>
# include <opencv2/highgui.hpp>
# include <opencv2/videoio.hpp>
#include <windows.h>
#include <stdio.h>
#include <time.h>     // for clock()


#define SUB_WINDOW_NAME "sub_win"
#define SETUP_WINDOW_NAME "setup"

#define METER_LINE_Y_TOP 384
#define METTER_LINE_Y_BOTTOM 557

#define VOLUME_X 488
#define SPEED_X 561
#define CONTRAST_X 778
#define LIGHT_X 849

#define METTER_LINE_COLOR  cv::Scalar(146, 146, 146)


class SubWindow {
public:
	int current_all_frame_num;

	void subWindowSetup();
	void setupWinInfo(cv::String);
	void setupWindowDestroy();
	void resetSubWindow(cv::String);
	void updateSubWindow(int, int, cv::Mat);
	void ChangeScene(int);
	void ExitSubWindow();

	cv::Mat Thumbnails[10][18];

	int group_reserve = 1;
	int group_playing = 1;
	int playing_scene = 1;

	cv::Mat bg_img;

	void ChangeSpeedSub(int);
	void ChangeVolumeSub(int);
	void ChangeContrastSub(int);
	void ChangeBrightNessSub(int);
	void setThumb(int, int, cv::Mat);

private:
	cv::Mat logo_img;

	cv::Mat current_monitor_img1;
	cv::Mat current_monitor_img2;

	cv::Mat Roi_speed;
	cv::Mat Roi_volume;
	cv::Mat Roi_contrast;
	cv::Mat Roi_brightness;

	cv::Mat meter_image;

	cv::Mat Thumb_img[10][18];

	int percentage = 0;

	int speed_s = 50;
	int volume_s = 50;
	int brightness_s = 50;
	int contrast_s = 50;
};


