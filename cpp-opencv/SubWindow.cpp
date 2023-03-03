#include "SubWindow.h"

#define MY_FONT cv::FONT_HERSHEY_SIMPLEX


void SubWindow::subWindowSetup() {
	logo_img = cv::imread("figures/logo.jpg");
	bg_img = cv::imread("figures/bg.jpg");
	meter_image = cv::imread("figures/meter.png");

	current_monitor_img2 = bg_img.clone();

	cv::Mat Roi_speed = current_monitor_img2(cv::Rect(552, 543 - int(speed_s  * 0.9), 25, 37));
	cv::Mat Roi_volume = current_monitor_img2(cv::Rect(480, 523 - int(volume_s * 1.8), 25, 37));
	cv::Mat Roi_contrast = current_monitor_img2(cv::Rect(765, 543 - int(contrast_s * 0.9), 25, 37));
	cv::Mat Roi_brightness = current_monitor_img2(cv::Rect(338, 543 - int(brightness_s * 0.9), 25, 37));

	ChangeBrightNessSub(50);
	ChangeContrastSub(80);
	ChangeVolumeSub(10);
	ChangeSpeedSub(100);

	cv::namedWindow(SETUP_WINDOW_NAME, cv::WINDOW_NORMAL);
	cv::imshow(SETUP_WINDOW_NAME, logo_img);
}


void SubWindow::setupWinInfo(cv::String info_c){
	cv::Mat Rendered_img = logo_img.clone();

	cv::putText(Rendered_img, info_c, cv::Point(210, 256), MY_FONT, 0.8, cv::Scalar(200, 200, 200), 0,16);
	cv::imshow(SETUP_WINDOW_NAME, Rendered_img);
	cv::waitKey(1);
}


void SubWindow::setupWindowDestroy() {
	cv::destroyWindow(SETUP_WINDOW_NAME);
	cv::namedWindow(SUB_WINDOW_NAME, cv::WINDOW_NORMAL);
	cv::resizeWindow(SUB_WINDOW_NAME, 1072, 602);
	cv::moveWindow(SUB_WINDOW_NAME, 30, 30);
}

void SubWindow::resetSubWindow(cv::String label_text) {
	//current_monitor_img2 = bg_img.clone();
	
	/*
	for name in self.Thumbnails[self.group_reserve].keys() :
	key_pos = np.argwhere(np.array(self.keyBoard_lists) == name)

	self.current_monitor_img2[410 + key_pos[0][0] * 60:450 + key_pos[0][0] * 60, 35 + key_pos[0][1] * 67 : 98 + key_pos[0][1] * 67] = \
	self.Thumbnails[self.group_reserve][name]
	*/
	cv::rectangle(current_monitor_img2, cv::Point(574, 207), cv::Point(1064, 338), cv::Scalar(0, 0, 0), -1);
	cv::putText(current_monitor_img2, label_text, cv::Point(680, 280), MY_FONT, 0.5, cv::Scalar(255, 255, 255), 0, 16);

	ChangeScene(-1);
}



void SubWindow::updateSubWindow(int CFN, int PRrate, cv::Mat current_frame2) {
	cv::Mat image = current_monitor_img1.clone();
	
	percentage = int(CFN * 103. / current_all_frame_num);

	cv::rectangle(image, cv::Point(590, 404), cv::Point(592 + percentage, 412), cv::Scalar(0, 0, 255), -1);

	cv::String CFN_char2 = std::to_string(CFN) + "/";
	CFN_char2 += std::to_string(current_all_frame_num);
	
	cv::String CFN_char3 = "P-R : " + std::to_string(PRrate);
	CFN_char3 += "%";

	cv::putText(image, CFN_char2, cv::Point(580, 430), MY_FONT, 0.5, cv::Scalar(255, 255, 255), 0, 16);
	cv::putText(image, CFN_char3, cv::Point(580, 470), MY_FONT, 0.5, cv::Scalar(255, 255, 255), 0, 16);
	
	cv::Mat resized_img;
	cv::resize(current_frame2, resized_img, cv::Size(), 468.0/ current_frame2.cols, 335.0/ current_frame2.rows);

	cv::Mat Roi1 = image(cv::Rect(23, 8, 468, 335));
	resized_img.copyTo(Roi1);

	for (int j = 0; j < 18; j++) {
		int X_index = int(j / 6);
		int Y_index = j % 6;	
		cv::Mat Roi = image(cv::Rect(35 + Y_index * 67, 410 + X_index * 60, 63, 40));
		Thumbnails[group_reserve][j].copyTo(Roi);
	}

	cv::imshow(SUB_WINDOW_NAME, image);
}


void SubWindow::ChangeScene(int all_frame_num) {
	int X_index = int(playing_scene/6);
	int Y_index = playing_scene % 6;

	std::cout << "Y_index ->" << Y_index << "\n";
	std::cout << "X_index ->" << X_index << "\n";


	current_monitor_img1 = current_monitor_img2.clone();
	
	//group˜g
	cv::rectangle(current_monitor_img1, cv::Point(35 + Y_index * 67, 410 + X_index * 60), cv::Point(98 + Y_index * 67, 450 + X_index * 60), cv::Scalar(255, 255, 0), 2);
	
	//scene˜g
	cv::rectangle(current_monitor_img1, cv::Point(123 + group_reserve * 30, 354), cv::Point(147 + group_reserve * 30, 376), cv::Scalar(0, 255, 255), 2);

	cv::String AA = "Playing->" + std::to_string(group_playing); 
	AA += std::to_string(playing_scene);

	cv::putText(current_monitor_img1, AA, cv::Point(600, 250), MY_FONT, 0.8, cv::Scalar(255, 255, 255), 2, 16);
	
	if (all_frame_num != -1) current_all_frame_num = all_frame_num;
	
}


void SubWindow::ExitSubWindow() {
	std::cout << "sub window destroy";
	cv::destroyWindow(SUB_WINDOW_NAME);
}


void SubWindow::setThumb(int group,int scene, cv::Mat Frame_img) {
	Thumbnails[group][scene] = Frame_img;
}


void SubWindow::ChangeSpeedSub(int speed) {
	if (speed == -1)return;
	speed_s = speed;
	cv::rectangle(current_monitor_img2, cv::Point(SPEED_X-13, METER_LINE_Y_TOP-10), cv::Point(SPEED_X+15, METTER_LINE_Y_BOTTOM +10), cv::Scalar(0, 0, 0), -1);
	cv::line(current_monitor_img2, cv::Point(SPEED_X, METER_LINE_Y_TOP), cv::Point(SPEED_X, METTER_LINE_Y_BOTTOM), METTER_LINE_COLOR, 2);
	Roi_speed = current_monitor_img2(cv::Rect(SPEED_X-13, 543 - int(speed_s  * 0.9), 25, 37));
	meter_image.copyTo(Roi_speed);
}


void SubWindow::ChangeVolumeSub(int volume) {
	if (volume == -1)return;
	volume_s = volume;
	cv::rectangle(current_monitor_img2, cv::Point(VOLUME_X - 13, METER_LINE_Y_TOP - 10), cv::Point(VOLUME_X + 13, METTER_LINE_Y_BOTTOM + 3), cv::Scalar(0, 0, 0), -1);
	cv::line(current_monitor_img2, cv::Point(VOLUME_X, METER_LINE_Y_TOP), cv::Point(VOLUME_X, METTER_LINE_Y_BOTTOM), METTER_LINE_COLOR, 2);
	Roi_volume = current_monitor_img2(cv::Rect(480, 543 - int(volume_s * 1.0), 25, 37));
	meter_image.copyTo(Roi_volume);
}

void SubWindow::ChangeContrastSub(int contrast) {
	if (contrast == -1)return;
	contrast_s = contrast;
	cv::rectangle(current_monitor_img2, cv::Point(CONTRAST_X - 13, METER_LINE_Y_TOP - 10), cv::Point(CONTRAST_X + 13, METTER_LINE_Y_BOTTOM -2), cv::Scalar(0, 0, 0), -1);
	cv::line(current_monitor_img2, cv::Point(CONTRAST_X, METER_LINE_Y_TOP), cv::Point(CONTRAST_X, METTER_LINE_Y_BOTTOM), METTER_LINE_COLOR, 2);
	Roi_contrast = current_monitor_img2(cv::Rect(765, 545 - int(contrast_s * 0.95), 25, 37));
	meter_image.copyTo(Roi_contrast);
}

void SubWindow::ChangeBrightNessSub(int brightness) {
	if (brightness == -1)return;
	brightness_s = brightness;
	cv::rectangle(current_monitor_img2, cv::Point(LIGHT_X - 13, METER_LINE_Y_TOP - 10), cv::Point(LIGHT_X + 13, METTER_LINE_Y_BOTTOM -2), cv::Scalar(0, 0, 0), -1);
	cv::line(current_monitor_img2, cv::Point(LIGHT_X, METER_LINE_Y_TOP), cv::Point(LIGHT_X, METTER_LINE_Y_BOTTOM), METTER_LINE_COLOR, 2);
	Roi_brightness = current_monitor_img2(cv::Rect(836, 545 - int(brightness_s * 0.95), 25, 37));
	meter_image.copyTo(Roi_brightness);
}
