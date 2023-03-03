#include "MainWindow.h"
#include <windows.h>
#include <time.h>     // for clock()

#pragma comment(lib,"Winmm.lib")//この行の代わりに「追加の依存ファイル」に追加しても良い
#include <mmsystem.h>


void MainWindow::MainWindowSetup() {
	//cv::namedWindow(MAIN_WINDOW_NAME, cv::WINDOW_NORMAL);
	cv::namedWindow(MAIN_WINDOW_NAME, cv::WINDOW_AUTOSIZE);

	cv::moveWindow(MAIN_WINDOW_NAME, 1925, 0);
	//cv::setWindowProperty(MAIN_WINDOW_NAME, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
}


void MainWindow::ChangeScene(int scene_str) {
	return;
}



void MainWindow::ExitMainWin() {
	cv::destroyWindow(MAIN_WINDOW_NAME);
}
	/*
	for(int i = 0; i < GROUP_NUM; i++) {
		for (auto itr = MovieCaps_p[i].begin(); itr != MovieCaps_p[i].end(); ++itr) {
			std::cout << "key = " << itr->first           // キーを表示
				<< ", val = " << itr->second << "\n";    // 値を表示
		}
	}
				self.MovieCaps[i][name].release()
				cv2.destroyWindow(MAIN_WINDOW_NAME)
*/

/*
void MainWindow::getFirstFrame(cv::Mat *movie) {
	, group, scene) :
	self.MovieCaps[group][scene].set(cv2.CAP_PROP_POS_FRAMES, 20)
		_, frame = self.MovieCaps[group][scene].read()
		self.MovieCaps[group][scene].set(cv2.CAP_PROP_POS_FRAMES, 0)
		return cv2.resize(frame, (63, 40))
}


*/