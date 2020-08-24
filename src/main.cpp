#include "stdafx.h"
#include "VideoPlayer.h"
#include <iostream>

//C++������ͷ�ļ�������
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_video.h"
};
//SDL���FFmpeg�����������
#define SDL_MAIN_HANDLED
#define __STDC_CONSTANT_MACROS

using namespace std;
using std::cout;
using std::endl;
int main(int argc, char* argv[])
{
	if (argc != 2) {
		cout << "Wrong parameter number!" << endl;
		exit(-1);
	}
	VideoPlayer video(argv[1]);
	video.init();
	video.play(video.getVideoChannel());
	return 0;
}