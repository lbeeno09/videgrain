#pragma once

#include <cstdint>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
}

class VideoReader
{
public:
	VideoReader();
	~VideoReader();

	bool open(const char* filename);
	bool readFrame(uint8_t* outData, int targetWidth, int targetHeight);
	bool seekFrame(int64_t frameIdx);
	void close();

	int width;
	int height;
	double fps;

	int64_t totalFrames;
	int64_t currentFrameIndex;

private:
	AVFormatContext* formatCtx;
	AVCodecContext* codecCtx;
	SwsContext* swsCtx;
	AVFrame* frame;
	AVPacket* packet;
	int videoStreamIdx;

};