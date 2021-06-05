#include "VideoWriter.h"


/*  ///////////////////////////////////////////////////////
//           �E�B���h�E�֘A�̏����pclass                 //
//                                                       //
/////////////////////////////////////////////////////////*/

void VideoWriter::update(int loop) {
	cv::Mat image_before, image_after;

	for (int i = 0; i < loop; i++) {
		Cap_old.read(image_before);
		CFN++;

		if (image_before.empty()) {
			write_finished = true;
			writer.release();
			writer.~VideoWriter();
			return;
		}
		cv::resize(image_before, image_after, cv::Size(), (double)new_width / image_before.cols, (double)new_height / image_before.rows);
		writer.write(image_after);
	}
	progress_percent = CFN * 100 / all_frame_num;
}


bool VideoWriter::setVideo(std::string old_filename, std::string new_filename, int New_width, int New_height, bool overwrite) {
	//�o�͂��铮��t�@�C���̐ݒ�����܂��B�g���q��WMV1�ŁA���b15�t���[���A�摜�c���T�C�Y��1024*1024�B
	new_width = New_width;
	new_height = New_height;

	write_finished = false;
	Cap_old = cv::VideoCapture(old_filename);
	if (!Cap_old.isOpened()) {
		MessageBox(NULL, "���͂��ꂽ�p�X�ɓ��悪���݂��܂���", "ERROR", MB_OK);
		return false;
	}

	struct stat buf;
	if (stat(new_filename.c_str(), &buf) == 0) {
		MessageBox(NULL, "�o�͐�ɓ����̓���t�@�C��������܂��BOverwrite��true�ɂ��Ă�������", "ERROR", MB_OK);
		return false;
	}

	all_frame_num = MAX(Cap_old.get(cv::CAP_PROP_FRAME_COUNT), 1);
	old_width = Cap_old.get(cv::CAP_PROP_FRAME_WIDTH);
	old_height = Cap_old.get(cv::CAP_PROP_FRAME_HEIGHT);


	std::cout << "\n\n\n*****************   Video Resizer   **********************\n";
	std::cout << "original file name -> " << old_filename << ")\n";
	std::cout << "resized  file name -> " << new_filename << ")\n";
	std::cout << "  size (before)    -> (" << old_width << ", " << old_height << ")\n";
	std::cout << "  size (after)     -> (" << new_width << ", " << new_height << ")\n";
	std::cout << "  frame number     -> " << all_frame_num << "\n";
	std::cout << "*****************   Video Resizer   **********************\n";


	//http://shibafu3.hatenablog.com/entry/2016/11/13/151118
	int codec[4];
	codec[0] = cv::VideoWriter::fourcc('m', 'p', '4', 'v');
	codec[1] = cv::VideoWriter::fourcc('h', '2', '6', '4');
	codec[2] = cv::VideoWriter::fourcc('x', '2', '6', '4');
	codec[3] = cv::VideoWriter::fourcc('m', 'p', 'g', 's');

	CFN = 0;

	for (int i = 0; i < 4; i++) {
		writer = cv::VideoWriter(new_filename, codec[i], 30, cv::Size(new_width, new_height));
		if (writer.isOpened())return true;
	}

	//writer.open(new_filename, codec = cv::VideoWriter::fourcc('m', 'p', '4', 'v'), 30, cv::Size(new_width, new_height));

	//����t�@�C���������ƍ�ꂽ���̔���B
 	MessageBox(NULL, "�r�f�I��open�ɂł��܂���", "ERROR", MB_OK);
	return false;
}

