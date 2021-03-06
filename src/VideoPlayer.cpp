// VideoPlayer.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "VideoPlayer.h"
#include <iostream>
#include <thread>
#include <conio.h>

bool VideoPlayer::pause_flag = false;
VideoPlayer::VideoPlayer(char *fPath) {
	filePath = string(fPath);
	pause_flag = false;
}

VideoPlayer::~VideoPlayer() {
	SDL_Quit();
	sws_freeContext(p_scaleCtx);
	if (!p_avFrameYUV) {
		av_frame_free(&p_avFrameYUV);
	}
	if (!p_avFrame) {
		av_frame_free(&p_avFrame);
	}
	avcodec_close(p_CodeCtx);
	avformat_close_input(&p_FormatCtx);
}

int VideoPlayer::findVideoChannel() {
	for (int i = 0; i<p_FormatCtx->nb_streams; i++)
	{
		if (p_FormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			video_channel = i;
			break;
		}
	}
	if (video_channel == -1)
	{
		std::cout << "Fail to find video stream!" << std::endl;
		return RETURN_ON_FAIL;
	}
	return RETURN_ON_SUCCESS;
}

int VideoPlayer::init_av_info() {
	if ( !(p_FormatCtx = avformat_alloc_context()) ) {
		std::cout << "No enough memory!" << std::endl;
		return RETURN_ON_FAIL;
	}
	if (avformat_open_input(&p_FormatCtx, filePath.c_str(), nullptr, nullptr) != 0)
	{
		std::cout << "Fail to open input stream!" << std::endl;
		return RETURN_ON_FAIL;
	}
	if (avformat_find_stream_info(p_FormatCtx, nullptr) < 0)
	{
		std::cout << "Failed to get information!" << std::endl;
		return RETURN_ON_FAIL;
	}
	if ((video_channel = findVideoChannel()) == RETURN_ON_FAIL) {
		std::cout << "Failed to find video channel!" << std::endl;
		return RETURN_ON_FAIL;
	}
	p_avpacket = (AVPacket*)av_malloc(sizeof(AVPacket));
	p_avFrame = av_frame_alloc();
	p_avFrameYUV = av_frame_alloc();
	if (!(p_avpacket || p_avFrame || p_avFrameYUV)) {
		std::cout << "No enough memory!" << std::endl;
		return RETURN_ON_FAIL;
	}
	p_CodeCtx = p_FormatCtx->streams[video_channel]->codec;
	p_avcodec = avcodec_find_decoder(p_CodeCtx->codec_id);
	if (p_avcodec == nullptr)
	{
		std::cout << "Failed to find decoder!" << std::endl;
		return RETURN_ON_FAIL;
	}
	if (avcodec_open2(p_CodeCtx, p_avcodec, NULL) < 0)
	{
		std::cout << "decoder opened failed" << std::endl;
		return -1;
	}
	buffer = (unsigned char*)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, p_CodeCtx->width, p_CodeCtx->height));
	avpicture_fill((AVPicture*)p_avFrameYUV, buffer, AV_PIX_FMT_YUV420P, p_CodeCtx->width, p_CodeCtx->height);
	p_scaleCtx = sws_getContext(p_CodeCtx->width, p_CodeCtx->height,
		p_CodeCtx->pix_fmt, p_CodeCtx->width, p_CodeCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	return RETURN_ON_SUCCESS;
}

int VideoPlayer::init_SDL() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER))
	{
		std::cout << "SDL initialization failed" << std::endl;
		return -1;
	}
	SDL_window = SDL_CreateWindow("Simple Video Player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, p_CodeCtx->width, p_CodeCtx->height, SDL_WINDOW_OPENGL);
	if (!SDL_window)
	{
		std::cout << "Failed to open SDL window!" << std::endl;
		return RETURN_ON_FAIL;
	}
	if (!(SDL_renderer = SDL_CreateRenderer(SDL_window, -1, 0))) {
		std::cout << "Failed to get SDL renderer!" << std::endl;
		return RETURN_ON_FAIL;
	}
	SDL_texture = SDL_CreateTexture(SDL_renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, p_CodeCtx->width, p_CodeCtx->height);
	if (SDL_texture == nullptr) {
		std::cout << "Failed to get SDL texture!" << std::endl;
		return RETURN_ON_FAIL;
	}
	rec.x = 0;
	rec.y = 0;
	rec.w = p_CodeCtx->width;
	rec.h = p_CodeCtx->height;
	return RETURN_ON_SUCCESS;
}

int refresh_event(void *video) {
	while (true) {
		if (!VideoPlayer::pause_flag) {
			SDL_Event e;
			e.type = VideoPlayer::REFRESH_EVENT;
			SDL_PushEvent(&e);
			SDL_Delay(100);
		}
	}
	return 0;
}
void VideoPlayer::init() {
	if (init_av_info() == RETURN_ON_FAIL || init_SDL() == RETURN_ON_FAIL)
		exit(-1);
	refresh_th = SDL_CreateThread(refresh_event, nullptr, nullptr);

}

void VideoPlayer::play(int channel) {
	int got_picture, ret;
	for (;;) {
		SDL_WaitEvent(&event);
		if (event.type == REFRESH_EVENT) {
			while (1) {
				if (av_read_frame(p_FormatCtx, p_avpacket) < 0)
					exit(0);
				if (p_avpacket->stream_index == video_channel)
					break;
			}
			ret = avcodec_decode_video2(p_CodeCtx, p_avFrame, &got_picture, p_avpacket);
			if (ret < 0) {
				std::cout << "Fail to decode frame!" << std::endl;
				return;
			}
			if (got_picture) {
				sws_scale(p_scaleCtx, (const unsigned char* const*)p_avFrame->data, p_avFrame->linesize, 0, 
					p_CodeCtx->height, p_avFrameYUV->data, p_avFrameYUV->linesize);
				SDL_UpdateTexture(SDL_texture, &rec, p_avFrameYUV->data[0], p_avFrameYUV->linesize[0]);
				SDL_RenderClear(SDL_renderer);  
				SDL_RenderCopy(SDL_renderer, SDL_texture, NULL, NULL);
				SDL_RenderPresent(SDL_renderer);
			}
			av_free_packet(p_avpacket);
		}
		else if (event.type == SDL_KEYUP) {
			switch (event.key.keysym.sym) {
			case SDLK_F10:
				pause_flag = !pause_flag;

				break;
			}
		}
		else if (event.type == SDL_QUIT) {
			exit(0);
		}
	}
}