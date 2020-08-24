#pragma once
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_video.h"
};
#include <string>
using std::string;
class VideoPlayer {
private:
	enum {RETURN_ON_SUCCESS = 0, RETURN_ON_FAIL = -1};
	string filePath;

	AVFormatContext* p_FormatCtx;
	AVCodecContext* p_CodeCtx;
	AVCodec* p_avcodec;
	AVPacket* p_avpacket;
	AVFrame* p_avFrame, *p_avFrameYUV;
	SwsContext* p_scaleCtx;

	unsigned char *buffer;
	int video_channel = -1;
	int *audio_channel;
	int au_nb;

	SDL_Window *SDL_window;
	SDL_Renderer *SDL_renderer;
	SDL_Texture *SDL_texture;
	SDL_Rect rec;
	SDL_Event event;
	SDL_Thread * refresh_th, *key_th;

	int findVideoChannel();
	int init_av_info();
	int init_SDL();
public:
	enum { REFRESH_EVENT = SDL_USEREVENT + 1 };
	static bool pause_flag;
	explicit VideoPlayer(char *);
	virtual ~VideoPlayer();
	void init();
	void play(int channel);
	int getVideoChannel() { return video_channel; }
	//void play();
};