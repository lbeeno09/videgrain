#pragma once

#include <cstdint>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include <vector>

class VideoReader
{
public:
	VideoReader();
	~VideoReader();

	bool open(const char* filename);
	void decodePacket(AVPacket* pkt, std::vector<uint8_t>& buffer);	
	void seekFrame(int64_t frameIdx);
	void close();

	AVFormatContext* GetFormatCtx() const { return formatCtx; }
	int GetVideoStreamIdx() const { return videoStreamIdx; }

	int width;
	int height;
	double fps;

	int64_t totalFrames;
	int64_t currentFrameIndex;

private:
	AVFormatContext* formatCtx;
	AVCodecContext* codecCtx;
	AVFrame* frame;
	SwsContext* swsCtx = nullptr;
	int videoStreamIdx;
};